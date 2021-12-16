#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdint.h>
#include "main.h"

#define BOARD_NAME                          "E-PAPER"

#define HCLK_MAX                            (32000000U)
#define HSI16_MAX                           (16000000U)

#define HAL_DRIVER                          (0U)
#define LL_DRIVER                           (1U)

#define GPIO_KEEP_WHEN_SLEEPING             ( \
    LL_IOP_GRP1_PERIPH_GPIOA                | \
    LL_IOP_GRP1_PERIPH_GPIOB                )

#define GPIOA_PINS_KEEP_WHEN_SLEEPING       ( \
    EXT_INTERRUPT_Pin                       )

#define GPIOA_PINS_FOR_DEBUGGING            ( \
    LL_GPIO_PIN_13                          | \
    LL_GPIO_PIN_14                          )


#define ACTIVE_STM_DRIVER                   LL_DRIVER

/* SPI Public Macros */
#define EPD_SPI                             SPI1
#define EPD_SPI_DMA                         DMA1
#define EPD_SPI_TX_DMA_CHANNEL              LL_DMA_CHANNEL_3

/* One Wire Public Macros */
#define DS_ONE_WIRE_PIN                     LL_GPIO_PIN_9
#define DS_ONE_WIRE_PORT                    GPIOA
#define DS_ONE_WIRE_ODR_BIT                 GPIO_ODR_OD9
#define DS_ONE_WIRE_IDR_BIT                 GPIO_IDR_ID9

/* I2C Public Macros */
#if (ACTIVE_STM_DRIVER == HAL_DRIVER)
#define SHT3X_I2C                           &hi2c1
#define SHT3X_I2C_HANDLER                   I2C_HandleTypeDef
#elif (ACTIVE_STM_DRIVER == LL_DRIVER)
#define SHT3X_I2C                           I2C1
#define SHT3X_I2C_HANDLER                   I2C_TypeDef
#endif

#endif /* __BOARD_H__ */
