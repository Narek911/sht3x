#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "settings.h"

#define SKIP_ROM                (0x00)
#define NO_SKIP_ROM             (0x01)
#define RESOLUTION_9BIT         (0x1F)
#define RESOLUTION_10BIT        (0x3F)
#define RESOLUTION_11BIT        (0x5F)
#define RESOLUTION_12BIT        (0x7F)

extern __IO uint32_t ds18b20_timer;
extern float temperature;

typedef struct {
	uint8_t mode;
	uint8_t resolution;
	uint8_t temp_high_trigger;
	uint8_t temp_low_trigger;
	GPIO_TypeDef *GPIOx;
	uint32_t odr_bit;
	uint32_t idr_bit;
} ds18b20_cfg_t;

int ds18b20_loop(void);
bool ds18b20_init(ds18b20_cfg_t *init_cfg);
bool is_ds18b20_active(void);

#endif /* __DS18B20_H__ */
