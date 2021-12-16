/**
  ******************************************************************************
  * @file    i2c.h
  * @brief   This file contains all the function prototypes for
  *          the i2c.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "util.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#ifndef MAX_NBYTE_SIZE
#define MAX_NBYTE_SIZE   0xFF
#endif

#ifndef SHT3X_I2C_TIMEOUT
#define SHT3X_I2C_TIMEOUT 30
#endif
/* USER CODE END Private defines */

void MX_I2C1_Init(void);

/* USER CODE BEGIN Prototypes */
void i2c_enable_interrupts(I2C_TypeDef *i2c);
void i2c_disable_interrupts(I2C_TypeDef *i2c);

Err i2c_master_write(I2C_TypeDef *i2c, uint8_t addr, uint8_t *b, uint8_t length, uint32_t request);
Err i2c_master_read(I2C_TypeDef *i2c, uint8_t addr, uint8_t *b, uint8_t length, uint32_t request);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
