/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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
#include "spi.h"

/* USER CODE BEGIN 0 */
#include "errno.h"
#include "util.h"
/* USER CODE END 0 */

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration
  PA5   ------> SPI1_SCK
  PA7   ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* SPI1 DMA Init */

  /* SPI1_TX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_3, LL_DMA_REQUEST_1);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_3, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MDATAALIGN_BYTE);

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  SPI_InitStruct.TransferDirection = LL_SPI_HALF_DUPLEX_TX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
  /* USER CODE BEGIN SPI1_Init 2 */
#if !SPI_TX_DMA
   LL_SPI_Enable(SPI1);
#endif
  /* USER CODE END SPI1_Init 2 */

}

/* USER CODE BEGIN 1 */

void MX_SPI1_DeInit(void)
{
    LL_SPI_Disable(SPI1);
    LL_SPI_DeInit(SPI1);
    NVIC_DisableIRQ(SPI1_IRQn);
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SPI1);
}

int spi_transmit_sync(SPI_TypeDef *spi,
                        const uint8_t *data, uint32_t data_length)
{
    uint32_t timeout = SPI_TIMEOUT_TXE_MS;

    for (uint32_t i = 0; i < data_length; i++) {
        /* Wait for transmit buffer become empty */
        while (!(spi->SR & SPI_SR_TXE)) {
            /* Check Systick counter flag to decrement the time-out value */
            if (LL_SYSTICK_IsActiveCounterFlag()) {
                if (timeout-- == 0U) {
                    DBG("USART TXE Timeout\n");
                    return EWOULDBLOCK;
                }
            }
        }

        /* Send bytes over the SPI */
        LL_SPI_TransmitData8(spi, data[i]);

        timeout = SPI_TIMEOUT_TC_MS;
        /* Wait until the transmission is complete */
        while (spi->SR & SPI_SR_BSY) {
            /* Check Systick counter flag to decrement the time-out value */
            if (LL_SYSTICK_IsActiveCounterFlag()) {
                if (timeout-- == 0U) {
                    DBG("USART TXE Timeout\n");
                    return EWOULDBLOCK;
                }
            }
        }
    }

    return 0;
}

void spi_dma_start(DMA_TypeDef *dma, uint32_t dma_channel)
{
    LL_DMA_EnableChannel(dma, dma_channel);
}

void spi_dma_stop(DMA_TypeDef *dma, uint32_t dma_channel)
{
    LL_DMA_DisableChannel(dma, dma_channel);
}

void spi_dma_defconfig(DMA_TypeDef *dma, uint32_t dma_channel,
                      SPI_TypeDef *spi, bool transmit,
                      uint32_t interrupts)
{
    if (transmit) {
        LL_SPI_EnableDMAReq_TX(spi);
    } else {
        LL_SPI_EnableDMAReq_RX(spi);
    }

    if (interrupts & DMA_CCR_TCIE) {
        LL_DMA_EnableIT_TC(dma, dma_channel);
    }
    if (interrupts & DMA_CCR_TEIE) {
        LL_DMA_EnableIT_TE(dma, dma_channel);
    }

    LL_SPI_Enable(spi);
}

void spi_dma_reconfig(DMA_TypeDef *dma, uint32_t dma_channel,
                        SPI_TypeDef *spi, bool transmit,
                        uint32_t buffer, uint32_t buffer_length)
{
    const uint32_t src_addr = transmit
            ? buffer
            : LL_SPI_DMA_GetRegAddr(spi);
    const uint32_t dst_addr = transmit
            ? LL_SPI_DMA_GetRegAddr(spi)
            : buffer;

    spi_dma_stop(EPD_SPI_DMA, EPD_SPI_TX_DMA_CHANNEL);

    LL_DMA_SetDataLength(dma, dma_channel, buffer_length);

    LL_DMA_ConfigAddresses(dma, dma_channel, src_addr, dst_addr,
                           LL_DMA_GetDataTransferDirection(dma, dma_channel));

}


/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
