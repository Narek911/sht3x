#ifndef __SHT3X_H__
#define __SHT3X_H__

#include <stdbool.h>

#include "i2c.h"
#include "board.h"

#define SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW  0x44
#define SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_HIGH 0x45

extern __IO uint32_t sht3x_timer;

typedef struct {
    SHT3X_I2C_HANDLER *i2c_handle;
    uint16_t device_address;
} sht3x_handle_t;

bool sht3x_init(sht3x_handle_t *handle);
int  sht3x_loop(void);
bool is_sht3x_active(void);

#endif /* __SHT3X_H__ */
