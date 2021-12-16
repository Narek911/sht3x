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
 * epd2in13d.c
 ************************************************************************************************/
#include "epd2in13d.h"
#include "util.h"
#include "spi.h"
#include "epdgui.h"
#include "ds18b20.h"

__IO uint32_t epd_timer = EPD_2IN13D_TIMER_MS;
static unsigned char black_image[((EPD_2IN13D_WIDTH % 8 == 0)? (EPD_2IN13D_WIDTH / 8 ): (EPD_2IN13D_WIDTH / 8 + 1)) * EPD_2IN13D_HEIGHT];
static gui_time sPaint_time;
static uint8_t dot = 0;

/* Full screen update LUT */
static const unsigned char epd_2in13d_lut_vcom_dc[] = {
    0x00, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x60, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,
};

static const unsigned char epd_2in13d_lut_ww[] = {
    0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
    0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char epd_2in13d_lut_bw[] = {
    0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x0F, 0x0F, 0x00, 0x00, 0x03,
    0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
    0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char epd_2in13d_lut_wb[] = {
    0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char epd_2in13d_lut_bb[] = {
    0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/* Partial screen update LUT */
static const unsigned char epd_2in13d_lut_vcom1[] = {
    0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    ,0x00, 0x00,
};

static const unsigned char epd_2in13d_lut_ww1[] = {
    0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char epd_2in13d_lut_bw1[] = {
    0x80, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char epd_2in13d_lut_wb1[] = {
    0x40, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char epd_2in13d_lut_bb1[] = {
    0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

#if SPI_TX_DMA
static __IO bool spi_transmit_dma_tc = false;

static void  epd_2in13d_spi_init(void)
{
	spi_dma_stop(EPD_SPI_DMA, EPD_SPI_TX_DMA_CHANNEL);

    LL_DMA_ClearFlag_TC3(EPD_SPI_DMA);
    LL_DMA_ClearFlag_TE3(EPD_SPI_DMA);

    spi_dma_defconfig(EPD_SPI_DMA, EPD_SPI_TX_DMA_CHANNEL,
                   EPD_SPI, true,
                   DMA_CCR_TCIE | DMA_CCR_TEIE);
}

#endif

/******************************************************************************
 * @brief  Software reset
 *
 * @retval None
 *
 * @papam  None
 *****************************************************************************/
static void epd_2in13d_reset(void)
{
    LL_GPIO_SetOutputPin(RST_GPIO_Port, RST_Pin);
    LL_mDelay(200);
    LL_GPIO_ResetOutputPin(RST_GPIO_Port, RST_Pin);
    LL_mDelay(2);
    LL_GPIO_SetOutputPin(RST_GPIO_Port, RST_Pin);
    LL_mDelay(200);
}

/******************************************************************************
 * @brief  Send command
 *
 * @param reg: Command register
 *
 * @retval  None
 *****************************************************************************/
static void epd_2in13d_send_command(uint8_t reg)
{
    LL_GPIO_ResetOutputPin(DC_GPIO_Port, DC_Pin);
#if SPI_TX_DMA
    spi_dma_reconfig(EPD_SPI_DMA, EPD_SPI_TX_DMA_CHANNEL,
            	     EPD_SPI, true,  (uint32_t)&reg,
					 sizeof reg);
    LL_GPIO_ResetOutputPin(SPI_CS_GPIO_Port, SPI_CS_Pin);
    spi_dma_start(EPD_SPI_DMA, EPD_SPI_TX_DMA_CHANNEL);
    while (!spi_transmit_dma_tc);
    spi_transmit_dma_tc = false;
#else
    LL_GPIO_ResetOutputPin(SPI_CS_GPIO_Port, SPI_CS_Pin);
    spi_transmit_sync(SPI1, &reg, 1);
    LL_GPIO_SetOutputPin(SPI_CS_GPIO_Port, SPI_CS_Pin);
#endif

}

/******************************************************************************
 * @brief  send data
 *
 * @retval data: Data to write
 *
 * @papam  None
 *****************************************************************************/
static void epd_2in13d_send_data(uint8_t data)
{
    LL_GPIO_SetOutputPin(DC_GPIO_Port, DC_Pin);

#if SPI_TX_DMA
    spi_dma_reconfig(EPD_SPI_DMA, EPD_SPI_TX_DMA_CHANNEL,
                     EPD_SPI, true,  (uint32_t)&data,
					 sizeof data);
    LL_GPIO_ResetOutputPin(SPI_CS_GPIO_Port, SPI_CS_Pin);
    spi_dma_start(EPD_SPI_DMA, EPD_SPI_TX_DMA_CHANNEL);
    while (!spi_transmit_dma_tc);
    spi_transmit_dma_tc = false;
#else
    LL_GPIO_ResetOutputPin(SPI_CS_GPIO_Port, SPI_CS_Pin);
    spi_transmit_sync(SPI1, &data, 1);
    LL_GPIO_SetOutputPin(SPI_CS_GPIO_Port, SPI_CS_Pin);
#endif
}

/******************************************************************************
 * @brief  Wait until the busy_pin goes LOW
 *
 * @retval None
 *
 * @papam  None
 *****************************************************************************/
static void epd_2in13d_read_busy(void)
{
    DBG("e-Paper busy\r\n");
    uint8_t busy;
    do {
        epd_2in13d_send_command(GET_STATUS);
        busy = LL_GPIO_IsInputPinSet(BUSY_GPIO_Port, BUSY_Pin);
        busy =!(busy & 0x01);
    } while(busy);
    LL_mDelay(200);
    DBG("e-Paper busy release\r\n");
}

/******************************************************************************
 * @brief  Full LUT download
 *
 * @retval None
 *
 * @papam  None
 *****************************************************************************/
static void epd_2in13d_set_full_reg(void)
{
    epd_2in13d_send_command(VCOM_AND_DATA_INTERVAL_SETTING);
    epd_2in13d_send_data(0xb7);		//WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7

    unsigned int count;
    epd_2in13d_send_command(VCOM_LUT);
    for(count=0; count<44; count++) {
        epd_2in13d_send_data(epd_2in13d_lut_vcom_dc[count]);
    }

    epd_2in13d_send_command(W2W_LUT);
    for(count=0; count<42; count++) {
        epd_2in13d_send_data(epd_2in13d_lut_ww[count]);
    }

    epd_2in13d_send_command(B2W_LUT);
    for(count=0; count<42; count++) {
        epd_2in13d_send_data(epd_2in13d_lut_bw[count]);
    }

    epd_2in13d_send_command(W2B_LUT);
    for(count=0; count<42; count++) {
        epd_2in13d_send_data(epd_2in13d_lut_wb[count]);
    }

    epd_2in13d_send_command(B2B_LUT);
    for(count=0; count<42; count++) {
        epd_2in13d_send_data(epd_2in13d_lut_bb[count]);
    }
}

/******************************************************************************
 * @brief  Partial LUT download
 *
 * @retval None
 *
 * @papam  None
 *****************************************************************************/
static void epd_2in13d_set_part_reg(void)
{
    epd_2in13d_send_command(VCM_DC_SETTING);
    epd_2in13d_send_data(0x00);
    epd_2in13d_send_command(VCOM_AND_DATA_INTERVAL_SETTING);
    epd_2in13d_send_data(0xb7);
	
    unsigned int count;
    epd_2in13d_send_command(VCOM_LUT);
    for(count=0; count<44; count++) {
        epd_2in13d_send_data(epd_2in13d_lut_vcom1[count]);
    }

    epd_2in13d_send_command(W2W_LUT);
    for(count=0; count<42; count++) {
        epd_2in13d_send_data(epd_2in13d_lut_ww1[count]);
    }

    epd_2in13d_send_command(B2W_LUT);
    for(count=0; count<42; count++) {
        epd_2in13d_send_data(epd_2in13d_lut_bw1[count]);
    }

    epd_2in13d_send_command(W2B_LUT);
    for(count=0; count<42; count++) {
        epd_2in13d_send_data(epd_2in13d_lut_wb1[count]);
    }

    epd_2in13d_send_command(B2B_LUT);
    for(count=0; count<42; count++) {
        epd_2in13d_send_data(epd_2in13d_lut_bb1[count]);
    }
}

/******************************************************************************
 * @brief  Turn On Display
 *
 * @retval None
 *
 * @papam  None
 *****************************************************************************/
static void epd_2in13d_turn_on_display(void)
{
    epd_2in13d_send_command(DISPLAY_REFRESH);
    LL_mDelay(10);    //!< The delay here is necessary, 200uS at least!!!
    epd_2in13d_read_busy();
}

/******************************************************************************
 * @brief  Initialize the e-Paper register
 *
 * @retval None
 *
 * @papam  None
 *****************************************************************************/
void epd_2in13d_init()
{
#if SPI_TX_DMA
	epd_2in13d_spi_init();

	LL_mDelay(100);
#endif
    epd_2in13d_reset();

    epd_2in13d_send_command(BOOSTER_SOFT_START);
    epd_2in13d_send_data(0xD0);     //!< A
    epd_2in13d_send_data(0xD0);     //!< B
    epd_2in13d_send_data(0xD0);     //!< C

    epd_2in13d_send_command(POWER_SETTING);
    epd_2in13d_send_data(0x03);
    epd_2in13d_send_data(0x00);
    epd_2in13d_send_data(0x2b);
    epd_2in13d_send_data(0x2b);
    epd_2in13d_send_data(0x03);

    epd_2in13d_send_command(POWER_ON);
    epd_2in13d_read_busy();

    epd_2in13d_send_command(PANEL_SETTING);
    epd_2in13d_send_data(0xbf);     //!< LUT from OTP，128x296

    epd_2in13d_send_command(PLL_CONTROL);
    epd_2in13d_send_data(0x3a);     //!< 3a 100HZ   29 150Hz 39 200HZ	31 171HZ

    epd_2in13d_send_command(RESOLUTION_SETTING);
    epd_2in13d_send_data(EPD_2IN13D_WIDTH);
    epd_2in13d_send_data((EPD_2IN13D_HEIGHT >> 8) & 0xff);
    epd_2in13d_send_data(EPD_2IN13D_HEIGHT& 0xff);

    epd_2in13d_send_command(VCM_DC_SETTING);	//vcom_DC setting
    epd_2in13d_send_data(0x28);
}

/******************************************************************************
 * @brief  Clear screen
 *
 * @retval None
 *
 * @papam  None
 *****************************************************************************/
void epd_2in13d_clear()
{
    uint16_t Width, Height;
    Width = (EPD_2IN13D_WIDTH % 8 == 0)? (EPD_2IN13D_WIDTH / 8 ): (EPD_2IN13D_WIDTH / 8 + 1);
    Height = EPD_2IN13D_HEIGHT;

    epd_2in13d_send_command(DATA_START_TRANSMISSION_1);
    for (uint16_t j = 0; j < Height; j++) {
        for (uint16_t i = 0; i < Width; i++) {
            epd_2in13d_send_data(0x00);
        }
    }

    epd_2in13d_send_command(DATA_START_TRANSMISSION_2);
    for (uint16_t j = 0; j < Height; j++) {
        for (uint16_t i = 0; i < Width; i++) {
            epd_2in13d_send_data(0xFF);
        }
    }

    epd_2in13d_set_full_reg();
    epd_2in13d_turn_on_display();
}

/******************************************************************************
 * @brief  Sends the content buffer in RAM to e-Paper and displays
 *
 * @retval None
 *
 * @papam  None
 *****************************************************************************/
void epd_2in13d_display(uint8_t *Image)
{
    uint16_t Width, Height;
    Width = (EPD_2IN13D_WIDTH % 8 == 0)? (EPD_2IN13D_WIDTH / 8 ): (EPD_2IN13D_WIDTH / 8 + 1);
    Height = EPD_2IN13D_HEIGHT;

    epd_2in13d_send_command(DATA_START_TRANSMISSION_1);
    for (uint16_t j = 0; j < Height; j++) {
        for (uint16_t i = 0; i < Width; i++) {
            epd_2in13d_send_data(0x00);
        }
    }
    LL_mDelay(10);

    epd_2in13d_send_command(DATA_START_TRANSMISSION_2);
    for (uint16_t j = 0; j < Height; j++) {
        for (uint16_t i = 0; i < Width; i++) {
            epd_2in13d_send_data(Image[i + j * Width]);
        }
    }
    LL_mDelay(10);

    epd_2in13d_set_full_reg();
    epd_2in13d_turn_on_display();
}

/******************************************************************************
 * @brief  Sends the image buffer in RAM to e-Paper and displays
 *
 * @retval None
 *
 * @papam  None
 *****************************************************************************/
void epd_2in13d_display_part(uint8_t *content)
{
    /* Set partial Windows */
    epd_2in13d_set_part_reg();
    epd_2in13d_send_command(PARTIAL_IN);
    epd_2in13d_send_command(PARTIAL_WINDOW);
    epd_2in13d_send_data(0);                           //!< x-start
    epd_2in13d_send_data(EPD_2IN13D_WIDTH - 1);        //!< x-end
    epd_2in13d_send_data(0);
    epd_2in13d_send_data(0);                           //!< y-start
    epd_2in13d_send_data(EPD_2IN13D_HEIGHT / 256);
    epd_2in13d_send_data(EPD_2IN13D_HEIGHT % 256 - 1); //!< y-end
    epd_2in13d_send_data(0x01);

    uint16_t Width;
    Width = (EPD_2IN13D_WIDTH % 8 == 0)? (EPD_2IN13D_WIDTH / 8 ): (EPD_2IN13D_WIDTH / 8 + 1);
    
    /* send data */
    epd_2in13d_send_command(DATA_START_TRANSMISSION_1);
    for (uint16_t j = 0; j < EPD_2IN13D_HEIGHT; j++) {
        for (uint16_t i = 0; i < Width; i++) {
            epd_2in13d_send_data(~content[i + j * Width]);
        }
    }

    epd_2in13d_send_command(DATA_START_TRANSMISSION_2);
    for (uint16_t j = 0; j < EPD_2IN13D_HEIGHT; j++) {
        for (uint16_t i = 0; i < Width; i++) {
            epd_2in13d_send_data(content[i + j * Width]);
        }
    }

    /* Set partial refresh */    
    epd_2in13d_turn_on_display();
}

/******************************************************************************
 * @brief  Enter sleep mode
 *
 * @retval None
 *
 * @papam  None
 *****************************************************************************/
void epd_2in13d_sleep()
{
    epd_2in13d_send_command(VCOM_AND_DATA_INTERVAL_SETTING);
    epd_2in13d_send_data(0xf7);
    epd_2in13d_send_command(POWER_OFF);
    epd_2in13d_send_command(DEEP_SLEEP);
    epd_2in13d_send_data(0xA5);
}

int epd_2in13d_prepare()
{
    printf("EPD_2IN13D_test Demo\r\n");

    printf("e-Paper Init and Clear...\r\n");
    epd_2in13d_init();
    epd_2in13d_clear();
    LL_mDelay(500);

    //Create a new image cache
    printf("Paint_NewImage\r\n");
    gui_new_content(black_image, EPD_2IN13D_WIDTH, EPD_2IN13D_HEIGHT, 270, WHITE);

#if 1   // Drawing on the image
    printf("Drawing\r\n");
    //1.Select Image
    gui_select_content(black_image);
    gui_clear(WHITE);

    // 2.Drawing on the image
    gui_display_string_en(5, 5, "locator", &Font16, BLACK, WHITE);
    gui_display_num(5, 25, 123456789, &Font16, BLACK, WHITE);
    epd_2in13d_display(black_image);
    LL_mDelay(1000);

    gui_clear(WHITE);

    epd_2in13d_display(black_image);
    LL_mDelay(2000);
#endif

    return 0;
}

void epd_2in13d_loop()
{
    char data[6];
    if (epd_timer == 0U) {
        gui_select_content(black_image);

        switch (dot) {
        case 0:
            gui_display_point(15, 55, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
            dot = 1;
            break;

        case 1:
            gui_display_point(30, 55, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
            dot = 2;
            break;

        case 2:
            gui_display_point(45, 55, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
            dot = 3;
            break;

        case 3:
            gui_display_point(60, 55, BLACK, DOT_PIXEL_4X4, DOT_STYLE_DFT);
            dot = 4;
            break;

        case 4:
            gui_clear_windows(10, 40, 65, 80, WHITE);
            dot = 0;
            break;
        }

        printf("Partial refresh\r\n");

        sPaint_time.sec += 1;
        if (sPaint_time.sec == 60) {
            sPaint_time.min = sPaint_time.min + 1;
            sPaint_time.sec = 0;
            if (sPaint_time.min == 60) {
                sPaint_time.hour =  sPaint_time.hour + 1;
                sPaint_time.min = 0;
                if (sPaint_time.hour == 24) {
                    sPaint_time.hour = 0;
                    sPaint_time.min = 0;
                    sPaint_time.sec = 0;
                }
            }
        }
        gui_clear_windows(15, 65, 15 + Font20.Width * 7, 65 + Font20.Height, WHITE);
        gui_display_time(15, 65, &sPaint_time, &Font20, WHITE, BLACK);

        sprintf(data, "%.2f", temperature);
        gui_display_string_en(5, 5, "Temp", &Font16, BLACK, WHITE);
        gui_display_string_en(80, 5, data, &Font16, BLACK, WHITE);
        gui_display_string_en(140, 5, "C", &Font16, BLACK, WHITE);

        epd_2in13d_display_part(black_image);

        epd_timer = EPD_2IN13D_TIMER_MS;
    }
}

#if SPI_TX_DMA

void epd_2in13d_transmit_complete_callback()
{
	spi_transmit_dma_tc = true;
	LL_GPIO_SetOutputPin(SPI_CS_GPIO_Port, SPI_CS_Pin);
}

void epd_2in13d_transfer_error_callback(){
}

#endif
