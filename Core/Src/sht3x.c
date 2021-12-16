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
 * sht3x.c
 ************************************************************************************************/

#include "sht3x.h"
#include "util.h"
#include "settings.h"
#include "i2c.h"
#include "board.h"


#define SHT3X_READ_OUT_STATUS_REG_LENGTH      (3U)
#define SHT3X_SINGLE_SHOT_MODE_LENGTH         (6U)
#define SHT3X_PRRIODICAL_SHOT_MODE_LENGTH     (6U)
#define SHT3X_CMD_LEN                         (2U)
#define SHT3X_WRITE_DATA_CHECKSUM_STATUS_BIT  (0U)
#define SHT3X_COMMAND_STATUS_BIT              (1U)
#define SHT3X_SYSTEM_RESET_DETECTED_BIT       (4U)
#define SHT3X_T_TRAKING_ALERT_BIT             (10U)
#define SHT3X_RH_TRAKING_ALERT_BIT            (11U)
#define SHT3X_HEATER_STATUS_BIT               (13U)
#define SHT3X_ALERT_PENDING_STATUS_BIT        (15U)
#define SHT3X_LOWREP_MEASUREMENT_DURATION     (4U)
#define SHT3X_MIDREP_MEASUREMENT_DURATION     (6U)
#define SHT3X_HIGHREP_MEASUREMENT_DURATION    (15U)
#define SHT3X_DEFAULT_STATUS_REGISTER_VALUE   (0x8010)

/* Registers addresses */
typedef enum
{
    SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH_ENABLE    = 0x2c06,
    SHT3X_COMMAND_MEASURE_MEDIUMREP_STRETCH_ENABLE  = 0x2c0D,
    SHT3X_COMMAND_MEASURE_LOWREP_STRETCH_ENABLE     = 0x2c10,
    SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH_DISABLE   = 0x2400,
    SHT3X_COMMAND_MEASURE_MEDIUMREP_STRETCH_DISABLE = 0x240B,
    SHT3X_COMMAND_MEASURE_LOWREP_STRETCH_DISABLE    = 0x2416,
    SHT3X_COMMAND_MEASURE_HIGHREP_05HZ              = 0x2032,
    SHT3X_COMMAND_MEASURE_MEDIUMREP_05HZ            = 0x2024,
    SHT3X_COMMAND_MEASURE_LOWREP_05HZ               = 0x202F,
    SHT3X_COMMAND_MEASURE_HIGHREP_1HZ               = 0x2130,
    SHT3X_COMMAND_MEASURE_MEDIUMREP_1HZ             = 0x2126,
    SHT3X_COMMAND_MEASURE_LOWREP_1HZ                = 0x212D,
    SHT3X_COMMAND_MEASURE_HIGHREP_2HZ               = 0x2236,
    SHT3X_COMMAND_MEASURE_MEDIUMREP_2HZ             = 0x2220,
    SHT3X_COMMAND_MEASURE_LOWREP_2HZ                = 0x222B,
    SHT3X_COMMAND_MEASURE_HIGHREP_4HZ               = 0x2334,
    SHT3X_COMMAND_MEASURE_MEDIUMREP_4HZ             = 0x2322,
    SHT3X_COMMAND_MEASURE_LOWREP_4HZ                = 0x2329,
    SHT3X_COMMAND_MEASURE_HIGHREP_10HZ              = 0x2737,
    SHT3X_COMMAND_MEASURE_MEDIUMREP_10HZ            = 0x2721,
    SHT3X_COMMAND_MEASURE_LOWREP_10HZ               = 0x272A,
    SHT3X_COMMAND_FETCH_DATA                        = 0xE000,
    SHT3X_COMMAND_PM_WITH_ART                       = 0x2B32,
    SHT3X_COMMAND_STOP_PERIODIC_DATA_ACQ_MODE       = 0x3093,
    SHT3X_COMMAND_SOFT_RESET                        = 0x30A2,
    SHT3X_COMMAND_RESET_THROUGH_GENERAL_CALL        = 0x0006,
    SHT3X_COMMAND_HEATER_ENABLE                     = 0x306D,
    SHT3X_COMMAND_HEATER_DISABLE                    = 0x3066,
    SHT3X_COMMAND_READ_STATUS                       = 0xF32D,
    SHT3X_COMMAND_CLEAR_STATUS                      = 0x3041,
} sht3x_command_t;

static sht3x_handle_t s_handle;
__IO uint32_t sht3x_timer = SHT3X_TIMER_MS;
static bool is_clock_stretching_activated = false;
static __IO bool is_sht3x_presence = true;

static uint8_t calc_crc(const uint8_t *data, size_t length);
static bool sht3x_read_status(sht3x_handle_t *handle, uint16_t *status);
static bool sht3x_read_temperature_and_humidity(sht3x_handle_t *handle, float *temperature, float *humidity);
static bool sht3x_clear_status_register(sht3x_handle_t *handle) __attribute__((unused));
static bool sht3x_soft_reset(sht3x_handle_t *handle);
static bool sht3x_general_call_reset(sht3x_handle_t *handle) __attribute__((unused));
static bool sht3x_stop_periodic_data_acq_mode(sht3x_handle_t *handle) __attribute__((unused));
static bool sht3x_periodical_read_temperature_and_humidity(sht3x_handle_t *handle, float *temperature, float *humidity) __attribute__((unused));
static bool sht3x_enable_periodic_data_acq_mode(sht3x_handle_t *handle) __attribute__((unused));
static bool sht3x_art_activate(sht3x_handle_t *handle) __attribute__((unused));
static bool sht3x_is_heater_enabled(sht3x_handle_t *handle) __attribute__((unused));
static bool sht3x_is_alert_pending(sht3x_handle_t *handle) __attribute__((unused));
static bool sht3x_is_rh_pending_alert(sht3x_handle_t *handle) __attribute__((unused));
static bool sht3x_is_t_tracking_alert(sht3x_handle_t *handle) __attribute__((unused));
static bool sht3x_is_system_reset_detected(sht3x_handle_t *handle) __attribute__((unused));
static bool sht3x_is_last_command_failed(sht3x_handle_t *handle) __attribute__((unused));
static bool sht3x_is_last_tx_checksum_failed(sht3x_handle_t *handle) __attribute__((unused));

static uint8_t calc_crc(const uint8_t *data, size_t length)
{
    uint8_t crc = 0xff;

    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (size_t j = 0; j < 8; j++) {
            if ((crc & 0x80u) != 0) {
                crc = (uint8_t)((uint8_t)(crc << 1u) ^ 0x31u);
            } else {
                crc <<= 1u;
            }
        }
    }

    return crc;
}

static bool sht3x_send_command(sht3x_handle_t *handle, sht3x_command_t command)
{
    uint8_t command_buffer[SHT3X_CMD_LEN] = {GET_BYTE(command, 1), GET_BYTE(command, 0)};

#if (ACTIVE_STM_DRIVER == HAL_DRIVER)
    if (HAL_I2C_Master_Transmit(handle->i2c_handle, handle->device_address << 1u, command_buffer, sizeof(command_buffer),
                                SHT3X_I2C_TIMEOUT) != HAL_OK) {
        return false;
    }
#elif (ACTIVE_STM_DRIVER == LL_DRIVER)
    if (i2c_master_write(handle->i2c_handle,
            handle->device_address << 1U,
            command_buffer,
            sizeof command_buffer,
            LL_I2C_GENERATE_START_WRITE)) {
        is_sht3x_presence = false;
        return false;
    }
#endif

    return true;
}

bool sht3x_init(sht3x_handle_t *handle)
{
    if (!handle) {
        is_sht3x_presence = false;
        return false;
    }

    s_handle = *handle;

    if (!sht3x_soft_reset(handle)) {
        is_sht3x_presence = false;
        return false;
    }

    uint16_t status = 0;
    if (!sht3x_read_status(&s_handle, &status)) {
        is_sht3x_presence = false;
        return false;
    }

    is_sht3x_presence = true;

    return (status == SHT3X_DEFAULT_STATUS_REGISTER_VALUE);
}

static bool sht3x_read_temperature_and_humidity(sht3x_handle_t *handle, float *temperature, float *humidity)
{
    int16_t temperature_raw = 0;
    uint16_t humidity_raw = 0;

    sht3x_send_command(handle, SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH_ENABLE);

    if (!is_clock_stretching_activated) {
#if (ACTIVE_STM_DRIVER == HAL_DRIVER)
        assert(handle->i2c_handle->Init.NoStretchMode == I2C_NOSTRETCH_DISABLE);
#elif (ACTIVE_STM_DRIVER == LL_DRIVER)
        LL_I2C_DisableClockStretching(handle->i2c_handle);
#endif
        LL_mDelay(1);
        is_clock_stretching_activated = true;
    }

    LL_mDelay(SHT3X_HIGHREP_MEASUREMENT_DURATION);

    uint8_t buffer[SHT3X_SINGLE_SHOT_MODE_LENGTH];

#if (ACTIVE_STM_DRIVER == HAL_DRIVER)
    if (HAL_I2C_Master_Receive(handle->i2c_handle, handle->device_address << 1u, buffer, sizeof(buffer), SHT3X_I2C_TIMEOUT) != HAL_OK) {
        return false;
    }
#elif (ACTIVE_STM_DRIVER == LL_DRIVER)
    if (i2c_master_read(handle->i2c_handle,
            handle->device_address << 1U,
            buffer,
            sizeof(buffer),
            LL_I2C_GENERATE_START_READ)) {
        is_sht3x_presence = false;
        return false;
    }
#endif

    uint8_t temperature_crc = calc_crc(buffer, 2);
    uint8_t humidity_crc = calc_crc(buffer + 3, 2);

    if (temperature_crc != buffer[2] || humidity_crc != buffer[5]) {
        return false;
    }

    SET_BYTE(temperature_raw, buffer[0], 1);
    SET_BYTE(temperature_raw, buffer[1], 0);
    SET_BYTE(humidity_raw, buffer[3], 1);
    SET_BYTE(humidity_raw, buffer[4], 0);

    *temperature = -45.0f + 175.0f * (float)temperature_raw / 65535.0f;
    *humidity = 100.0f * (float)humidity_raw / 65535.0f;

    return true;
}

static bool sht3x_enable_periodic_data_acq_mode(sht3x_handle_t *handle)
{
    is_clock_stretching_activated = false;

    return sht3x_send_command(handle, SHT3X_COMMAND_MEASURE_HIGHREP_1HZ);
}

static bool sht3x_periodical_read_temperature_and_humidity(sht3x_handle_t *handle, float *temperature, float *humidity)
{
    int16_t temperature_raw = 0;
    uint16_t humidity_raw = 0;

    sht3x_send_command(handle, SHT3X_COMMAND_FETCH_DATA);

    LL_mDelay(SHT3X_HIGHREP_MEASUREMENT_DURATION);

    uint8_t buffer[SHT3X_PRRIODICAL_SHOT_MODE_LENGTH];

#if (ACTIVE_STM_DRIVER == HAL_DRIVER)
    if (HAL_I2C_Master_Receive(handle->i2c_handle, handle->device_address << 1u, buffer, sizeof(buffer), SHT3X_I2C_TIMEOUT) != HAL_OK) {
        return false;
    }
#elif (ACTIVE_STM_DRIVER == LL_DRIVER)
    if (i2c_master_read(handle->i2c_handle,
            handle->device_address << 1U,
            buffer,
            sizeof(buffer),
            LL_I2C_GENERATE_START_READ)) {
        is_sht3x_presence = false;
        return false;
    }
#endif

    uint8_t temperature_crc = calc_crc(buffer, 2);
    uint8_t humidity_crc = calc_crc(buffer + 3, 2);

    if (temperature_crc != buffer[2] || humidity_crc != buffer[5]) {
        return false;
    }

    SET_BYTE(temperature_raw, buffer[0], 1);
    SET_BYTE(temperature_raw, buffer[1], 0);
    SET_BYTE(humidity_raw, buffer[3], 1);
    SET_BYTE(humidity_raw, buffer[4], 0);

    *temperature = -45.0f + 175.0f * (float)temperature_raw / 65535.0f;
    *humidity = 100.0f * (float)humidity_raw / 65535.0f;

    return true;
}

static bool sht3x_stop_periodic_data_acq_mode(sht3x_handle_t *handle)
{
    return sht3x_send_command(handle, SHT3X_COMMAND_STOP_PERIODIC_DATA_ACQ_MODE);
}

bool sht3x_set_header_enable(sht3x_handle_t *handle, bool enable)
{
    bool retval;

    if (enable) {
        retval = sht3x_send_command(handle, SHT3X_COMMAND_HEATER_ENABLE);
    } else {
        retval = sht3x_send_command(handle, SHT3X_COMMAND_HEATER_DISABLE);
    }

    LL_mDelay(1);

    return retval;
}

static bool sht3x_clear_status_register(sht3x_handle_t *handle)
{
    return sht3x_send_command(handle, SHT3X_COMMAND_CLEAR_STATUS);
}

static bool sht3x_soft_reset(sht3x_handle_t *handle)
{
    bool retval = sht3x_send_command(handle, SHT3X_COMMAND_SOFT_RESET);

    LL_mDelay(10);

    return retval;
}

static bool sht3x_general_call_reset(sht3x_handle_t *handle)
{
    return sht3x_send_command(handle, SHT3X_COMMAND_RESET_THROUGH_GENERAL_CALL);
}

static bool sht3x_art_activate(sht3x_handle_t *handle)
{
    return sht3x_send_command(handle, SHT3X_COMMAND_PM_WITH_ART);
}

static bool sht3x_is_heater_enabled(sht3x_handle_t *handle)
{
    uint16_t status;

    if (!sht3x_read_status(handle, &status)) {
        return false;
    }

    return (bool)CHECK_BIT(status, SHT3X_HEATER_STATUS_BIT);
}

static bool sht3x_is_alert_pending(sht3x_handle_t *handle)
{
    uint16_t status;

    if (!sht3x_read_status(handle, &status)) {
        return false;
    }

    return (bool)CHECK_BIT(status, SHT3X_ALERT_PENDING_STATUS_BIT);
}

static bool sht3x_is_rh_pending_alert(sht3x_handle_t *handle)
{
    uint16_t status;

    if (!sht3x_read_status(handle, &status)) {
        return false;
    }

    return (bool)CHECK_BIT(status, SHT3X_RH_TRAKING_ALERT_BIT);
}

static bool sht3x_is_t_tracking_alert(sht3x_handle_t *handle)
{
    uint16_t status;

    if (!sht3x_read_status(handle, &status)) {
        return false;
    }

    return (bool)CHECK_BIT(status, SHT3X_T_TRAKING_ALERT_BIT);
}

static bool sht3x_is_system_reset_detected(sht3x_handle_t *handle)
{
    uint16_t status;

    if (!sht3x_read_status(handle, &status)) {
        return false;
    }

    return (bool)CHECK_BIT(status, SHT3X_SYSTEM_RESET_DETECTED_BIT);
}

static bool sht3x_is_last_command_failed(sht3x_handle_t *handle)
{
    uint16_t status;

    if (!sht3x_read_status(handle, &status)) {
        return false;
    }

    return (bool)CHECK_BIT(status, SHT3X_COMMAND_STATUS_BIT);
}

static bool sht3x_is_last_tx_checksum_failed(sht3x_handle_t *handle)
{
    uint16_t status;

    if (!sht3x_read_status(handle, &status)) {
        return false;
    }

    return (bool)CHECK_BIT(status, SHT3X_WRITE_DATA_CHECKSUM_STATUS_BIT);
}

static bool sht3x_read_status(sht3x_handle_t *handle, uint16_t *status)
{
    uint8_t status_reg_and_checksum[SHT3X_READ_OUT_STATUS_REG_LENGTH];
    uint16_t stat;

#if (ACTIVE_STM_DRIVER == HAL_DRIVER)
    if (HAL_I2C_Mem_Read(handle->i2c_handle, handle->device_address << 1u, SHT3X_COMMAND_READ_STATUS, 2, (uint8_t*)&status_reg_and_checksum,
                     sizeof(status_reg_and_checksum), SHT3X_I2C_TIMEOUT) != HAL_OK) {
          return false;
    }
#elif (ACTIVE_STM_DRIVER == LL_DRIVER)
    sht3x_send_command(handle, SHT3X_COMMAND_READ_STATUS);

    if (i2c_master_read(handle->i2c_handle,
            handle->device_address << 1U,
            status_reg_and_checksum,
            sizeof(status_reg_and_checksum),
            LL_I2C_GENERATE_START_READ)) {
         is_sht3x_presence = false;
         return false;
     }
#endif

    uint8_t calculated_crc = calc_crc(status_reg_and_checksum, 2);

    if (calculated_crc != status_reg_and_checksum[2]) {
        return false;
    }

    stat = 0;
    SET_BYTE(stat, status_reg_and_checksum[0], 1);
    SET_BYTE(stat, status_reg_and_checksum[1], 0);

    *status = stat;

    return true;
}

int sht3x_loop()
{
    if (sht3x_timer == 0U) {
        if (!is_sht3x_presence) {
            return 0;
        }

        float temp = 0.0;
        float hum = 0.0;

        sht3x_read_temperature_and_humidity(&s_handle, &temp, &hum);
        DBG("The Temperature is %.2f %.2f in celsius\n", temp, hum);

        sht3x_timer = SHT3X_TIMER_MS;
    }

    return 1;
}

bool is_sht3x_active()
{
    return is_sht3x_presence;
}

