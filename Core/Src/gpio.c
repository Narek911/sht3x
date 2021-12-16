/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "board.h"
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_ResetOutputPin(DS18B20_GPIO_Port, DS18B20_Pin);

  /**/
  LL_GPIO_ResetOutputPin(RST_GPIO_Port, RST_Pin);

  /**/
  LL_GPIO_ResetOutputPin(DC_GPIO_Port, DC_Pin);

  /**/
  LL_GPIO_ResetOutputPin(SPI_CS_GPIO_Port, SPI_CS_Pin);

  /**/
  LL_GPIO_ResetOutputPin(BSI_GPIO_Port, BSI_Pin);

  /**/
  LL_GPIO_ResetOutputPin(DSPIN_GPIO_Port, DSPIN_Pin);

  /**/
  LL_GPIO_ResetOutputPin(VDD_GPIO_Port, VDD_Pin);

  /**/
  LL_GPIO_ResetOutputPin(GND_GPIO_Port, GND_Pin);

  /**/
  LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin);

  /**/
  GPIO_InitStruct.Pin = DS18B20_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(DS18B20_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = BUSY_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(BUSY_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = RST_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(RST_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = DC_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(DC_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = SPI_CS_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(SPI_CS_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = BSI_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(BSI_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = DSPIN_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(DSPIN_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = VDD_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(VDD_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = GND_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(GND_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /**/
  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE12);

  /**/
  LL_GPIO_SetPinPull(EXT_INTERRUPT_GPIO_Port, EXT_INTERRUPT_Pin, LL_GPIO_PULL_NO);

  /**/
  LL_GPIO_SetPinMode(EXT_INTERRUPT_GPIO_Port, EXT_INTERRUPT_Pin, LL_GPIO_MODE_INPUT);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_12;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /* EXTI interrupt init*/
  NVIC_SetPriority(EXTI4_15_IRQn, 3);
  NVIC_EnableIRQ(EXTI4_15_IRQn);

}

/* USER CODE BEGIN 2 */

void MX_GPIO_ReInit(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (!LL_IOP_GRP1_IsEnabledClock(LL_IOP_GRP1_PERIPH_GPIOA)) {
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    }

    if (!LL_IOP_GRP1_IsEnabledClock(LL_IOP_GRP1_PERIPH_GPIOB)) {
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    }

    if (!LL_IOP_GRP1_IsEnabledClock(LL_IOP_GRP1_PERIPH_GPIOC)) {
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
    }

    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;

    GPIO_InitStruct.Pin = LL_GPIO_PIN_ALL;
#if defined(GPIOA_PINS_FOR_DEBUGGING)
    GPIO_InitStruct.Pin &= ~GPIOA_PINS_FOR_DEBUGGING;
#endif
#if defined(GPIOA_PINS_KEEP_WHEN_SLEEPING)
    GPIO_InitStruct.Pin &= ~GPIOA_PINS_KEEP_WHEN_SLEEPING;
#endif
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_ALL;
#if defined(GPIOB_PINS_KEEP_WHEN_SLEEPING)
    GPIO_InitStruct.Pin &= ~GPIOB_PINS_KEEP_WHEN_SLEEPING;
#endif
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_ALL;
#if defined(GPIOC_PINS_KEEP_WHEN_SLEEPING)
    GPIO_InitStruct.Pin &= ~GPIOC_PINS_KEEP_WHEN_SLEEPING;
#endif
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

#if defined(GPIOD)
    GPIO_InitStruct.Pin = LL_GPIO_PIN_ALL;
# if defined(GPIOD_PINS_KEEP_WHEN_SLEEPING)
    GPIO_InitStruct.Pin &= ~GPIOD_PINS_KEEP_WHEN_SLEEPING;
# endif
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif

#if defined(GPIOE)
    GPIO_InitStruct.Pin = LL_GPIO_PIN_ALL;
# if defined(GPIOE_PINS_KEEP_WHEN_SLEEPING)
    GPIO_InitStruct.Pin &= ~GPIOE_PINS_KEEP_WHEN_SLEEPING;
# endif
    LL_GPIO_Init(GPIOE, &GPIO_InitStruct);
#endif

#if defined(GPIOH)
    GPIO_InitStruct.Pin = LL_GPIO_PIN_ALL;
# if defined(GPIOH_PINS_KEEP_WHEN_SLEEPING)
    GPIO_InitStruct.Pin &= ~GPIOH_PINS_KEEP_WHEN_SLEEPING;
# endif
    LL_GPIO_Init(GPIOH, &GPIO_InitStruct);
#endif

    if (!(GPIOA_PINS_KEEP_WHEN_SLEEPING & RST_Pin)) {
        GPIO_InitStruct.Pin = RST_Pin;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(RST_GPIO_Port, &GPIO_InitStruct);
    }

    if (!(GPIOA_PINS_KEEP_WHEN_SLEEPING & DS18B20_Pin)) {
        GPIO_InitStruct.Pin = DS18B20_Pin;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(DS18B20_GPIO_Port, &GPIO_InitStruct);
    }

    if (!(GPIOA_PINS_KEEP_WHEN_SLEEPING & DC_Pin)) {
        GPIO_InitStruct.Pin = DC_Pin;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(DC_GPIO_Port, &GPIO_InitStruct);
    }

    if (!(GPIOA_PINS_KEEP_WHEN_SLEEPING & SPI_CS_Pin)) {
        GPIO_InitStruct.Pin = SPI_CS_Pin;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(SPI_CS_GPIO_Port, &GPIO_InitStruct);
    }

    if (!(GPIOA_PINS_KEEP_WHEN_SLEEPING & LL_GPIO_PIN_8)) {
        GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }

    GPIO_InitStruct.Pin = LED_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);


    GPIO_InitStruct.Pin = BSI_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(BSI_GPIO_Port, &GPIO_InitStruct);
}

void MX_GPIO_DeInit(void)
{
    LL_GPIO_InitTypeDef gpio_initstruct = {
        .Pin        = LL_GPIO_PIN_ALL,
        .Mode       = LL_GPIO_MODE_ANALOG,
        .Speed      = LL_GPIO_SPEED_FREQ_HIGH,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull       = LL_GPIO_PULL_NO,
        .Alternate  = LL_GPIO_AF_0
    };
    const uint32_t iop_grp1_gpiox_all =
            LL_IOP_GRP1_PERIPH_GPIOA |
            LL_IOP_GRP1_PERIPH_GPIOB |
            LL_IOP_GRP1_PERIPH_GPIOC |
#if defined(GPIOD)
            LL_IOP_GRP1_PERIPH_GPIOD |
#endif
#if defined(GPIOE)
            LL_IOP_GRP1_PERIPH_GPIOE |
#endif
#if defined(GPIOH)
            LL_IOP_GRP1_PERIPH_GPIOH |
#endif
            0U;
    const uint32_t iop_grp1_gpiox_to_disable =
            iop_grp1_gpiox_all & ~GPIO_KEEP_WHEN_SLEEPING;

    LL_IOP_GRP1_EnableClock(iop_grp1_gpiox_all);

#if defined(GPIOA_PINS_KEEP_WHEN_SLEEPING)
    gpio_initstruct.Pin = LL_GPIO_PIN_ALL & ~GPIOA_PINS_KEEP_WHEN_SLEEPING;
#else
    gpio_initstruct.Pin = LL_GPIO_PIN_ALL;
#endif /* defined(GPIOA_PINS_KEEP_WHEN_SLEEPING) */
#if defined(DEBUG) && defined(GPIOA_PINS_FOR_DEBUGGING)
    gpio_initstruct.Pin &= ~GPIOA_PINS_FOR_DEBUGGING;
#endif
    LL_GPIO_Init(GPIOA, &gpio_initstruct);

#if defined(GPIOB_PINS_KEEP_WHEN_SLEEPING)
    gpio_initstruct.Pin = LL_GPIO_PIN_ALL & ~GPIOB_PINS_KEEP_WHEN_SLEEPING;
#else
    gpio_initstruct.Pin = LL_GPIO_PIN_ALL;
#endif /* defined(GPIOB_PINS_KEEP_WHEN_SLEEPING) */
    LL_GPIO_Init(GPIOB, &gpio_initstruct);

#if defined(GPIOC_PINS_KEEP_WHEN_SLEEPING)
    gpio_initstruct.Pin = LL_GPIO_PIN_ALL & ~GPIOC_PINS_KEEP_WHEN_SLEEPING;
#else
    gpio_initstruct.Pin = LL_GPIO_PIN_ALL;
#endif /* defined(GPIOC_PINS_KEEP_WHEN_SLEEPING) */
    LL_GPIO_Init(GPIOC, &gpio_initstruct);

#if defined(GPIOD)
# if defined(GPIOD_PINS_KEEP_WHEN_SLEEPING)
    gpio_initstruct.Pin = LL_GPIO_PIN_ALL & ~GPIOD_PINS_KEEP_WHEN_SLEEPING;
# else
    gpio_initstruct.Pin = LL_GPIO_PIN_ALL;
# endif /* defined(GPIOD_PINS_KEEP_WHEN_SLEEPING) */
    LL_GPIO_Init(GPIOD, &gpio_initstruct);
#endif /* defined(GPIOD) */

#if defined(GPIOE)
# if defined(GPIOE_PINS_KEEP_WHEN_SLEEPING)
    gpio_initstruct.Pin = LL_GPIO_PIN_ALL & ~GPIOE_PINS_KEEP_WHEN_SLEEPING;
# else
    gpio_initstruct.Pin = LL_GPIO_PIN_ALL;
# endif /* defined(GPIOE_PINS_KEEP_WHEN_SLEEPING) */
    LL_GPIO_Init(GPIOE, &gpio_initstruct);
#endif /* defined(GPIOE) */

#if defined(GPIOH)
# if defined(GPIOH_PINS_KEEP_WHEN_SLEEPING)
    gpio_initstruct.Pin = LL_GPIO_PIN_ALL & ~GPIOH_PINS_KEEP_WHEN_SLEEPING;
# else
    gpio_initstruct.Pin = LL_GPIO_PIN_ALL;
# endif /* defined(GPIOH_PINS_KEEP_WHEN_SLEEPING) */
    LL_GPIO_Init(GPIOH, &gpio_initstruct);
#endif /* defined(GPIOH) */

    if (iop_grp1_gpiox_to_disable) {
        LL_IOP_GRP1_DisableClock(iop_grp1_gpiox_to_disable);
    }
}

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
