#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <stdbool.h>
#include "board.h"

/* version information */
#define FIRMWARE_VERSION_MAJOR                (0x00U)
#define FIRMWARE_VERSION_MINOR                (0x01U)

/* ds18b20 high and low alarm trigger registers */
#define DS_TH_REGISTER_VALUE       (0x64) //!< 100C
#define DS_TL_REGISTER_VALUE       (0x9E) //!< -30C

/* modules loop timers */
#define MAIN_TIMER_MS                         (5000U)
#define DS18B20_TIMER_MS                      (1000U)
#define SHT3X_TIMER_MS                        (1000U)
#define EPD_2IN13D_TIMER_MS                   (200U)

/* use wakeup (general switch) - wkup1 or standard external interrupt */
#define WKUP_ENABLED                          (1)

/* wakeup pin1 - use wkup1, otherwise will use standard external interrupt */
#define WKUP_WKUP1_ENABLED                    (0)

/* printf debugging, disable semihosting when enabling one of these */
#define PRINTF_DEBUG_ENABLED                  (0)
#define PRINTF_NOP_ENABLED                    (0)

/* makros to activate SPI transmission over dma controller */
#define SPI_TX_DMA                            (1)

#endif /* __SETTINGS_H__ */
