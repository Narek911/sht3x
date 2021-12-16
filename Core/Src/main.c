/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "epd2in13d.h"
#include "util.h"
#include "fonts.h"
#include "ds18b20.h"
#include <stdlib.h>
#include "epd_test.h"
#include "sht3x.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
__IO uint32_t main_timer = MAIN_TIMER_MS;

typedef enum {
    ACTIVE_SENSOR_DS18B20,
    ACTIVE_SENSOR_SHT3X,
    ACTIVE_SENSOR_FAIL
} active_sensor_type;

static char *active_sensor_name[3] = {
    "Active sensor is ds18b20",
    "Active sensor is sht3x",
    "Active sensor failed"
};

static active_sensor_type active_sensor = ACTIVE_SENSOR_FAIL;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
static sht3x_handle_t handle[] = {
        {
                .i2c_handle = SHT3X_I2C,
                .device_address = SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW
        }
};

static ds18b20_cfg_t ds18b20_init_cfg[] = {
        {
                .mode = SKIP_ROM,
                .resolution = RESOLUTION_12BIT,
                .temp_high_trigger = DS_TH_REGISTER_VALUE,
                .temp_low_trigger = DS_TL_REGISTER_VALUE,
                .GPIOx = DS_ONE_WIRE_PORT,
                .odr_bit = DS_ONE_WIRE_ODR_BIT,
                .idr_bit = DS_ONE_WIRE_IDR_BIT,
        }
};
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
 void main_loop()
 {
     if (main_timer == 0U) {
         LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

         if (active_sensor == ACTIVE_SENSOR_FAIL) {
             LL_GPIO_SetPinMode(GPIOA, DS_ONE_WIRE_PIN, LL_GPIO_MODE_OUTPUT);
             LL_mDelay(100);
             if (ds18b20_init(ds18b20_init_cfg)) {
                 active_sensor = ACTIVE_SENSOR_DS18B20;
                 DBG("ds18b20 successfully initialized\n");
             } else {
                 if (LL_I2C_IsEnabled(handle->i2c_handle)) {
                     LL_I2C_Disable(handle->i2c_handle);
                 }
                 LL_GPIO_SetPinMode(GPIOA, DS_ONE_WIRE_PIN, LL_GPIO_MODE_ALTERNATE);
                 LL_mDelay(100);
             }

             if (sht3x_init(handle)) {
                 active_sensor = ACTIVE_SENSOR_SHT3X;
                 DBG("SHT3X successfully initialized\n");
             }

             DBG("%s\n", active_sensor_name[active_sensor]);
         }

         if (!is_ds18b20_active() && active_sensor == ACTIVE_SENSOR_DS18B20) {
             if (LL_I2C_IsEnabled(handle->i2c_handle)) {
                 LL_I2C_Disable(handle->i2c_handle);
             }
             active_sensor = ACTIVE_SENSOR_FAIL;
             LL_GPIO_SetPinMode(GPIOA, DS_ONE_WIRE_PIN, LL_GPIO_MODE_ALTERNATE);
             DBG("%s\n", active_sensor_name[active_sensor]);
         }

         if (!is_sht3x_active() && active_sensor == ACTIVE_SENSOR_SHT3X) {
             active_sensor = ACTIVE_SENSOR_FAIL;
             LL_GPIO_SetPinMode(GPIOA, DS_ONE_WIRE_PIN, LL_GPIO_MODE_OUTPUT);
             DBG("%s\n", active_sensor_name[active_sensor]);
         }

         LL_mDelay(100);

         main_timer = MAIN_TIMER_MS;
     }
 }
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#if defined(DEBUG) && defined(USING_SEMIHOSTING)
extern void initialise_monitor_handles(void);
#endif
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* System interrupt init*/

  /** PVD Configuration
  */
  LL_PWR_SetPVDLevel(LL_PWR_PVDLEVEL_6);

  /* Configure EXTI line 16 */
  LL_EXTI_InitTypeDef exti_initstruct;
  exti_initstruct.Line_0_31 = LL_EXTI_LINE_16;
  exti_initstruct.LineCommand = ENABLE;

  exti_initstruct.Mode = LL_EXTI_MODE_IT;

  exti_initstruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;

  LL_EXTI_Init( &exti_initstruct);
  /** Enable the PVD Output
  */
  LL_PWR_EnablePVD();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
# if defined(USING_SEMIHOSTING)
  initialise_monitor_handles();
# endif
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_RTC_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  LL_SYSTICK_EnableIT();

  LL_GPIO_SetOutputPin(VDD_GPIO_Port, VDD_Pin);
  LL_GPIO_ResetOutputPin(DC_GPIO_Port, DC_Pin);
  LL_GPIO_SetOutputPin(SPI_CS_GPIO_Port, SPI_CS_Pin);
  LL_GPIO_SetOutputPin(RST_GPIO_Port, RST_Pin);
  LL_mDelay(500);

  /* Change i2c clock pin mode to output */
  LL_GPIO_SetPinMode(GPIOA, DS_ONE_WIRE_PIN, LL_GPIO_MODE_OUTPUT);
  LL_mDelay(10);
  if (ds18b20_init(ds18b20_init_cfg)) {
      active_sensor = ACTIVE_SENSOR_DS18B20;
      DBG("ds18b20 successfully initialized\n");
  } else {
      if (LL_I2C_IsEnabled(handle->i2c_handle)) {
          LL_I2C_Disable(handle->i2c_handle);
      }
      LL_GPIO_SetPinMode(GPIOA, DS_ONE_WIRE_PIN, LL_GPIO_MODE_ALTERNATE);
      DBG("ds18b20 initialization failed configuring clock pin to alternate mode\n");
      LL_mDelay(10);
  }

  if (sht3x_init(handle)) {
      active_sensor = ACTIVE_SENSOR_SHT3X;
      DBG("SHT3X successfully initialized\n");
  }
 // epd_2in13d_prepare();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      sht3x_loop();
      main_loop();
      ds18b20_loop();
    //  epd_2in13d_loop();
#if 0

    LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin);
    LL_mDelay(1000);
    DBG("e-Paper init failed\n");
    LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin);
    LL_mDelay(1000);
#endif
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_0)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_LSI_Enable();

   /* Wait till LSI is ready */
  while(LL_RCC_LSI_IsReady() != 1)
  {

  }
  LL_PWR_EnableBkUpAccess();
  if(LL_RCC_GetRTCClockSource() != LL_RCC_RTC_CLKSOURCE_LSI)
  {
    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);
  }
  LL_RCC_EnableRTC();
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {

  }

  LL_Init1msTick(16000000);

  LL_SetSystemCoreClock(16000000);
  LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_PCLK1);
}

/* USER CODE BEGIN 4 */

/* prepare things before entering into a low power mode */
void deinit(enum power_save_t power_save_mode)
{
    (void)power_save_mode;

//    rs485_deinit();
//    gprs_deinit();
//    adc_deinit();

//    MX_DMA_DeInit();

    MX_GPIO_DeInit();
}

/* prepare things after exiting a low power mode */
void reinit(enum power_save_t power_save_mode)
{
    (void)power_save_mode;

    MX_GPIO_ReInit();

//    if (!LL_AHB1_GRP1_IsEnabledClock(LL_AHB1_GRP1_PERIPH_DMA1)) {
//        MX_DMA_Init();
//    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
