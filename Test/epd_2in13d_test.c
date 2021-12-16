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
#include "epd_test.h"
#include "epd2in13d.h"
#include <stdlib.h>

int epd_2in13d_test()
{
    printf("EPD_2IN13D_test Demo\r\n");

    printf("e-Paper Init and Clear...\r\n");
    epd_2in13d_init();
    epd_2in13d_clear();
    LL_mDelay(500);

    //Create a new image cache
    uint8_t *BlackImage;
    /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
    uint16_t Imagesize = ((EPD_2IN13D_WIDTH % 8 == 0)? (EPD_2IN13D_WIDTH / 8 ): (EPD_2IN13D_WIDTH / 8 + 1)) * EPD_2IN13D_HEIGHT;
    if((BlackImage = (uint8_t *)malloc(Imagesize)) == NULL) {
     printf("Failed to apply for black memory...\r\n");
    }
    printf("Paint_NewImage\r\n");
    gui_new_content(BlackImage, EPD_2IN13D_WIDTH, EPD_2IN13D_HEIGHT, 270, WHITE);

    #if 1   //show image for array
    printf("show image for array\r\n");
    gui_select_content(BlackImage);
    gui_clear(WHITE);
    gui_display_bit_map(gImage_2in13d);

    epd_2in13d_display(BlackImage);
    LL_mDelay(2000);
    #endif

    #if 1   // Drawing on the image
    printf("Drawing\r\n");
    //1.Select Image
    gui_select_content(BlackImage);
    gui_clear(WHITE);

    // 2.Drawing on the image
    gui_display_string_en(5, 5, "waveshare", &Font16, BLACK, WHITE);
    gui_display_num(5, 25, 123456789, &Font16, BLACK, WHITE);
    epd_2in13d_display(BlackImage);
    LL_mDelay(1000);

    gui_clear(WHITE);
    gui_display_point(5, 10, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    gui_display_point(5, 25, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    gui_display_point(5, 40, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    gui_display_point(5, 55, BLACK, DOT_PIXEL_4X4, DOT_STYLE_DFT);
    gui_display_line(20, 10, 70, 60, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    gui_display_line(70, 10, 20, 60, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    gui_display_line(170, 15, 170, 55, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    gui_display_line(150, 35, 190, 35, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    gui_display_rectangle(20, 10, 70, 60, BLACK, DOT_PIXEL_1X1, GUI_FILL_EMPTY);
    gui_display_rectangle(85, 10, 130, 60, BLACK, DOT_PIXEL_1X1, GUI_FILL_FULL);
    gui_display_circle(170, 35, 20, BLACK, DOT_PIXEL_1X1, GUI_FILL_EMPTY);
    gui_display_circle(170, 80, 20, BLACK, DOT_PIXEL_1X1, GUI_FILL_FULL);
    epd_2in13d_display(BlackImage);
    LL_mDelay(2000);
    #endif

    printf("Partial refresh\r\n");
    gui_select_content(BlackImage);
    gui_time sPaint_time;
    sPaint_time.hour = 12;
    sPaint_time.min = 34;
    sPaint_time.sec = 56;
    uint8_t num = 20;
    for (;;) {
     sPaint_time.sec = sPaint_time.sec + 1;
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

     num = num - 1;
     if(num == 0) {
         break;
     }
     epd_2in13d_display_part(BlackImage);
     LL_mDelay(500);//Analog clock 1s
    }

    printf("Clear...\r\n");
    free(BlackImage);
    BlackImage = NULL;

    // close 5V
    printf("close 5V, Module enters 0 power consumption ...\r\n");

    return 0;
}

