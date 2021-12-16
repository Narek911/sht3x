/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**I2C1 GPIO Configuration
  PA9   ------> I2C1_SCL
  PA10   ------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  /** I2C Initialization
  */
  LL_I2C_EnableAutoEndMode(I2C1);
  LL_I2C_DisableOwnAddress2(I2C1);
  LL_I2C_DisableGeneralCall(I2C1);
  LL_I2C_EnableClockStretching(I2C1);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.Timing = 0x0010061A;
  I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  I2C_InitStruct.DigitalFilter = 0;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C1, &I2C_InitStruct);
  LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/* USER CODE BEGIN 1 */
#if (ACTIVE_STM_DRIVER == LL_DRIVER)
void MX_I2C1_DeInit(void)
{
    LL_I2C_DeInit(I2C1);
    NVIC_DisableIRQ(I2C1_IRQn);
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C1);
}

void i2c_enable_interrupts(I2C_TypeDef *i2c)
{
    LL_I2C_ClearFlag_STOP(i2c);
    LL_I2C_ClearFlag_TXE(i2c);
    LL_I2C_ClearFlag_NACK(i2c);
    LL_I2C_ClearFlag_BERR(i2c);
    LL_I2C_ClearFlag_OVR(i2c);
    LL_I2C_ClearFlag_ARLO(i2c);

    LL_I2C_EnableIT_TX(i2c);
    LL_I2C_EnableIT_TC(i2c);
    LL_I2C_EnableIT_RX(i2c);
    LL_I2C_EnableIT_NACK(i2c);
    LL_I2C_EnableIT_ERR(i2c);
    LL_I2C_EnableIT_STOP(i2c);
}

void i2c_disable_interrupts(I2C_TypeDef *i2c)
{
    LL_I2C_DisableIT_TX(i2c);
    LL_I2C_DisableIT_TC(i2c);
    LL_I2C_DisableIT_RX(i2c);
    LL_I2C_DisableIT_NACK(i2c);
    LL_I2C_DisableIT_ERR(i2c);
    LL_I2C_DisableIT_STOP(i2c);

    LL_I2C_ClearFlag_STOP(i2c);
    LL_I2C_ClearFlag_TXE(i2c);
    LL_I2C_ClearFlag_NACK(i2c);
    LL_I2C_ClearFlag_BERR(i2c);
    LL_I2C_ClearFlag_OVR(i2c);
    LL_I2C_ClearFlag_ARLO(i2c);
}

Err i2c_master_write(I2C_TypeDef *i2c, uint8_t addr, uint8_t *b, uint8_t length, uint32_t request)
{
    uint32_t timeout;

    if (!length) {
        return ERR_CONFIG_INCOMPLETE;
    }

    if (!LL_I2C_IsEnabled(i2c)) {
        LL_I2C_Enable(i2c);
    }

    if (request == LL_I2C_GENERATE_START_WRITE) {
        timeout = SHT3X_I2C_TIMEOUT;

        while (LL_I2C_IsActiveFlag_BUSY(i2c)) {
            if (LL_SYSTICK_IsActiveCounterFlag()) {
                if (timeout-- == 0) {
                    return ERR_TIMEOUT_ERROR;
                }
            }
        }
    }

    LL_I2C_HandleTransfer(i2c, addr, LL_I2C_ADDRSLAVE_7BIT, length,
            ((length >= MAX_NBYTE_SIZE) ? LL_I2C_MODE_RELOAD : LL_I2C_MODE_AUTOEND), request);

    for (uint16_t i = 0U; i < length; i++) {
        timeout = SHT3X_I2C_TIMEOUT;
        while (!LL_I2C_IsActiveFlag_TXIS(i2c)) {
            if (LL_I2C_IsActiveFlag_NACK(i2c)) {
                return ERR_CONNECT_FAIL;
            }

            if (LL_SYSTICK_IsActiveCounterFlag()) {
                if (timeout-- == 0) {
                    return ERR_TIMEOUT_ERROR;
                }
            }
        }

        if (LL_I2C_IsActiveFlag_NACK(i2c)) {
            LL_I2C_ClearFlag_NACK(i2c);
        }
        LL_I2C_TransmitData8(i2c, b[i]);
    }

    timeout = SHT3X_I2C_TIMEOUT;
    while ((((length >= MAX_NBYTE_SIZE) ? LL_I2C_MODE_RELOAD : LL_I2C_MODE_AUTOEND) == LL_I2C_MODE_AUTOEND &&
           !LL_I2C_IsActiveFlag_STOP(i2c)) || (((length >= MAX_NBYTE_SIZE) ? LL_I2C_MODE_RELOAD : LL_I2C_MODE_AUTOEND) == LL_I2C_MODE_RELOAD &&
           !LL_I2C_IsActiveFlag_TCR(i2c)) || (((length >= MAX_NBYTE_SIZE) ? LL_I2C_MODE_RELOAD : LL_I2C_MODE_AUTOEND) != LL_I2C_MODE_AUTOEND &&
           ((length >= MAX_NBYTE_SIZE) ? LL_I2C_MODE_RELOAD : LL_I2C_MODE_AUTOEND) != LL_I2C_MODE_RELOAD &&
           !LL_I2C_IsActiveFlag_TC(i2c))) {
        if (LL_SYSTICK_IsActiveCounterFlag()) {
            if (timeout-- == 0) {
            return ERR_TIMEOUT_ERROR;
            }
        }
    }

    if (((length >= MAX_NBYTE_SIZE) ? LL_I2C_MODE_RELOAD : LL_I2C_MODE_AUTOEND) == LL_I2C_MODE_AUTOEND) {
        LL_I2C_ClearFlag_STOP(i2c);

        timeout = SHT3X_I2C_TIMEOUT;
        while (LL_I2C_IsActiveFlag_BUSY(i2c)) {
            if (LL_SYSTICK_IsActiveCounterFlag()) {
                if (timeout-- == 0U) {
                    return ERR_TIMEOUT_ERROR;
                }
            }
        }
    }

    return ERR_NO_ERR;
}

Err i2c_master_read(I2C_TypeDef *i2c, uint8_t addr, uint8_t *b, uint8_t length, uint32_t request)
{
    uint32_t timeout;

    if (!LL_I2C_IsEnabled(i2c)) {
        LL_I2C_Enable(i2c);
    }

    if (request == LL_I2C_GENERATE_START_WRITE) {
        timeout = SHT3X_I2C_TIMEOUT;

        while (LL_I2C_IsActiveFlag_BUSY(i2c)) {
            if (LL_SYSTICK_IsActiveCounterFlag()) {
                if (timeout-- == 0) {
                    return ERR_TIMEOUT_ERROR;
                }
            }
        }
    }

    LL_I2C_HandleTransfer(i2c, addr, LL_I2C_ADDRSLAVE_7BIT, length,
    ((length >= MAX_NBYTE_SIZE)?LL_I2C_MODE_RELOAD:LL_I2C_MODE_AUTOEND), request);

    for (uint16_t i = 0U; i < length; i++) {
        timeout = SHT3X_I2C_TIMEOUT;
        while (!LL_I2C_IsActiveFlag_RXNE(i2c)) {
            if (LL_SYSTICK_IsActiveCounterFlag()) {
                if (timeout-- == 0) {
                    return ERR_TIMEOUT_ERROR;
                }
            }
        }
        b[i] = LL_I2C_ReceiveData8(i2c);
    }

    timeout = SHT3X_I2C_TIMEOUT;
    while ((((length >= MAX_NBYTE_SIZE) ? LL_I2C_MODE_RELOAD : LL_I2C_MODE_AUTOEND) == LL_I2C_MODE_AUTOEND &&
           !LL_I2C_IsActiveFlag_STOP(i2c)) || (((length >= MAX_NBYTE_SIZE) ? LL_I2C_MODE_RELOAD : LL_I2C_MODE_AUTOEND) == LL_I2C_MODE_RELOAD &&
           !LL_I2C_IsActiveFlag_TCR(i2c)) || (((length >= MAX_NBYTE_SIZE) ? LL_I2C_MODE_RELOAD : LL_I2C_MODE_AUTOEND) != LL_I2C_MODE_AUTOEND &&
           ((length >= MAX_NBYTE_SIZE) ? LL_I2C_MODE_RELOAD : LL_I2C_MODE_AUTOEND) != LL_I2C_MODE_RELOAD &&
           !LL_I2C_IsActiveFlag_TC(i2c))) {
        if (LL_SYSTICK_IsActiveCounterFlag()) {
            if (timeout-- == 0) {
                return ERR_TIMEOUT_ERROR;
            }
        }
    }

    if (((length >= MAX_NBYTE_SIZE)?LL_I2C_MODE_RELOAD:LL_I2C_MODE_AUTOEND) == LL_I2C_MODE_AUTOEND) {
        LL_I2C_ClearFlag_STOP(i2c);

        timeout = SHT3X_I2C_TIMEOUT;
        while (LL_I2C_IsActiveFlag_BUSY(i2c)) {
            if (LL_SYSTICK_IsActiveCounterFlag()) {
                if (timeout-- == 0U) {
                    return ERR_TIMEOUT_ERROR;
                }
            }
        }
    }

    return ERR_NO_ERR;
}

#endif
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
