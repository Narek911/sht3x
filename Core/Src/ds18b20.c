/*************************************************************************************************
 * LOCATOR CJSC
 *
 * CONFIDENTIAL AND PROPRIETARY
 * FOR USE BY AUTHORIZED PERSONS ONLY
 *
 * This is an unpublished work fully protected by the copyright
 * laws and is a trade secret belonging to the copyright holder.
 *
 * Copyright (c) 2007-2021 LOCATOR CJSC. All Rights Reserved
 *************************************************************************************************
 * ds18b20.c
 ************************************************************************************************/

#include "ds18b20.h"
#include "gpio.h"
#include "tim.h"
#include "util.h"

#define CMD_SEARCH_ROM          	(0xF0)
#define CMD_READ_ROM            	(0x33)
#define CMD_MATCH_ROM           	(0x55)
#define CMD_SKIP_ROM            	(0xCC)
#define CMD_ALARM_SEARCH        	(0xEC)
#define CMD_CONVERT_T           	(0x44)
#define CMD_READ_SCRATCHPAD     	(0xBE)
#define CMD_WRITE_SCRATCHPAD    	(0x4E)
#define CMD_COPY_SCRATCHPAD     	(0x48)
#define CMD_RECALL_EEPROM       	(0xB8)
#define CMD_READ_POWER_SUPPLY   	(0xB4)

#define CONVERSION_TIME_9BIT    	(93.74)
#define CONVERSION_TIME_10BIT   	(187.5)
#define CONVERSION_TIME_11BIT   	(375)
#define CONVERSION_TIME_12BIT   	(750)

#define TEMP_REG_DATA_MASK      	(0x07FF)
#define TEMP_REG_RESOLUTION_MASK    (0x000F)
#define TEMP_REG_SIGN_BIT           (11)

static ds18b20_cfg_t ds18b20_cfg;
static __IO bool is_ds18b20_presence = true;
__IO uint32_t ds18b20_timer = DS18B20_TIMER_MS;
float temperature = 0.0;

static bool ds18b20_reset(void);
static uint8_t ds18b20_read_bit(void);
static uint8_t ds18b20_read_byte(void);
static void ds18b20_write_bit(uint8_t bit);
static void ds18b20_write_byte(uint8_t data);
static void ds18b20_measure_temp(uint8_t mode,  uint8_t dev_num);
static bool ds18b20_get_sign(uint16_t data);
static float ds18b20_convert(uint16_t data);

static bool ds18b20_reset(void)
{
	bool status = false;

	ds18b20_cfg.GPIOx->ODR &= ~ds18b20_cfg.odr_bit;
	delay_us(485);
	ds18b20_cfg.GPIOx->ODR |= ds18b20_cfg.odr_bit;
	delay_us(65);
	status = ds18b20_cfg.GPIOx->IDR & ds18b20_cfg.idr_bit;
	delay_us(500);

	return (status ? 1 : 0);
}

static uint8_t ds18b20_read_bit()
{
	uint8_t bit = 0;

	ds18b20_cfg.GPIOx->ODR &= ~ds18b20_cfg.odr_bit;
	delay_us(2);
	ds18b20_cfg.GPIOx->ODR |= ds18b20_cfg.odr_bit;
	delay_us(13);
	bit = (ds18b20_cfg.GPIOx->IDR & ds18b20_cfg.idr_bit ? 1 : 0);
    delay_us(45);

	return bit;
}

static uint8_t ds18b20_read_byte()
{
	uint8_t data = 0;

	for (uint8_t i = 0; i < 8; i++) {
		data +=  ds18b20_read_bit() << i;
	}

	return data;
}

static void ds18b20_write_bit(uint8_t bit)
{
    ds18b20_cfg.GPIOx->ODR &= ~ds18b20_cfg.odr_bit;
	delay_us(bit ? 3 : 65);
	ds18b20_cfg.GPIOx->ODR |= ds18b20_cfg.odr_bit;
	delay_us(bit ? 65 : 3);
}

static void ds18b20_write_byte(uint8_t data)
{
	for (uint8_t i = 0; i < 8; i++) {
		ds18b20_write_bit(data >> i & 1);
		delay_us(5);
	}
}

bool ds18b20_init(ds18b20_cfg_t *init_cfg)
{
	if (init_cfg == NULL) {
	    is_ds18b20_presence = false;
	    return false;
	}

	ds18b20_cfg = *init_cfg;

	if (ds18b20_reset()) {
	    is_ds18b20_presence = false;
	    return false;
	}

	if (ds18b20_cfg.mode == SKIP_ROM) {
		/* SKIP ROM */
		ds18b20_write_byte(CMD_SKIP_ROM);
		/* WRITE SCRATCHPAD */
		ds18b20_write_byte(CMD_WRITE_SCRATCHPAD);
		/* TH REGISTER */
		ds18b20_write_byte(ds18b20_cfg.temp_high_trigger);
		/* TL REGISTER */
		ds18b20_write_byte(ds18b20_cfg.temp_low_trigger);
		/* Resolution 12 bit */
		ds18b20_write_byte(ds18b20_cfg.resolution);
	}

	 is_ds18b20_presence = true;

	return true;
}

static void ds18b20_measure_temp(uint8_t mode, uint8_t dev_num)
{
    if (ds18b20_reset()) {
    	is_ds18b20_presence = false;
    }

    if (mode == SKIP_ROM) {
    	ds18b20_write_byte(CMD_SKIP_ROM);
    }

    ds18b20_write_byte(CMD_CONVERT_T);
}

static void ds18b20_read_scratchpad(uint8_t mode, uint8_t *data,  uint8_t dev_num)
{
	uint8_t i;
    if (ds18b20_reset()) {
    	is_ds18b20_presence = false;
    }

    if (mode == SKIP_ROM) {
    	ds18b20_write_byte(CMD_SKIP_ROM);
    }
    ds18b20_write_byte(CMD_READ_SCRATCHPAD);

    for (i = 0; i < 8; i++) {
    	data[i] = ds18b20_read_byte();
    }
}

static bool ds18b20_get_sign(uint16_t data)
{
	return ((data & (1 << TEMP_REG_SIGN_BIT)) ? true : false);
}

static float ds18b20_convert(uint16_t data)
{
	float t;

	t = (float) ((data & TEMP_REG_DATA_MASK) >> 4);

	t += (float) (data & TEMP_REG_RESOLUTION_MASK) / 16.0f;

	return t;
}

int ds18b20_loop()
{
    if (ds18b20_timer == 0U) {
        if (!is_ds18b20_presence) {
            return 0;
        }

        uint16_t temp;
        uint8_t data[8] = {0};
        char c;

        ds18b20_measure_temp(ds18b20_cfg.mode, 0);

        LL_mDelay(CONVERSION_TIME_12BIT);

        ds18b20_read_scratchpad(ds18b20_cfg.mode, data, 0);

//        DBG("SCRATCHPAD: %02x %02x %02x %02x %02x %02x %02x %02x\n", data[0], data[1], data[2],
//        		data[3], data[4], data[5], data[6], data[7]);

        temp = 0;
        SET_BYTE(temp, data[0], 0);
        SET_BYTE(temp, data[1], 1);

        if (ds18b20_get_sign(temp)) {
        	c = '-';
        } else {
        	c = '+';
        }

        temperature = ds18b20_convert(temp);
        DBG("The Temperature is %c%.2f in celsius\n", c, temperature);
        (void) c;

        ds18b20_timer = DS18B20_TIMER_MS;
    }

    return 1;
}

bool is_ds18b20_active()
{
    return is_ds18b20_presence;
}
