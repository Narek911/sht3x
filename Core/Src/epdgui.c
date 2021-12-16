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
 * epdgui.c
 ************************************************************************************************/

#include "epdgui.h"
#include "util.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

content cnt;
#define  ARRAY_LEN 255

/******************************************************************************
 * @brief  Create Image
 *
 * @papam  content: Pointer to the content cache
 * @papam  width:   The width of the picture
 * @papam  height:  The height of the picture
 * @papam  color:   Whether the picture is inverted
 *
 * @retval None
 *****************************************************************************/
void gui_new_content(uint8_t *content, uint16_t width, uint16_t height, uint16_t rotate, uint16_t color)
{
    cnt.content = NULL;
    cnt.content = content;

    cnt.width_memory = width;
    cnt.height_memory = height;
    cnt.color = color;
    cnt.scale = 2;
		
    cnt.width_byte = (width % 8 == 0)? (width / 8 ): (width / 8 + 1);
    cnt.height_byte = height;
   
    cnt.rotate = rotate;
    cnt.mirror = MIRROR_NONE;
    
    if (rotate == ROTATE_0 || rotate == ROTATE_180) {
        cnt.width = width;
        cnt.height = height;
    } else {
        cnt.width = height;
        cnt.height = width;
    }
}

/******************************************************************************
 * @brief  Select Content
 *
 * @papam  content: Pointer to the content cache
 *
 * @retval  None
 *****************************************************************************/
void gui_select_content(uint8_t *content)
{
    cnt.content = content;
}

/******************************************************************************
 * @brief  Select Content Rotate
 *
 * @papam  rotate: 0,90,180,270 degree
 *
 * @retval None
 *****************************************************************************/
void gui_set_rotate(uint16_t rotate)
{
    if (rotate == ROTATE_0 || rotate == ROTATE_90 || rotate == ROTATE_180 || rotate == ROTATE_270) {
        DBG("Set image Rotate %d\r\n", rotate);
        cnt.rotate = rotate;
    } else {
        DBG("rotate = 0, 90, 180, 270\r\n");
    }
}

/******************************************************************************
 * @brief  Select Content Scale
 *
 * @papam  rotate: The content scale
 *
 * @retval None
 *****************************************************************************/
void gui_set_scale(uint8_t scale)
{
    if (scale == 2) {
        cnt.scale = scale;
        cnt.width_byte = (cnt.width_memory % 8 == 0)? (cnt.width_memory / 8 ): (cnt.width_memory / 8 + 1);
    } else if(scale == 4) {
        cnt.scale = scale;
        cnt.width_byte = (cnt.width_memory % 4 == 0)? (cnt.width_memory / 4 ): (cnt.width_memory / 4 + 1);
    } else if(scale == 7) {  //!< Only applicable with 5in65 e-Paper
        cnt.scale = scale;
        cnt.width_byte = (cnt.width_memory % 2 == 0)? (cnt.width_memory / 2 ): (cnt.width_memory / 2 + 1);;
    }else{
        DBG("Set Scale Input parameter error\r\n");
        DBG("Scale Only support: 2 4 7\r\n");
    }
}

/******************************************************************************
 * @brief  Select Content Mirror
 *
 * @papam  mirror: Not mirror,Horizontal mirror,Vertical mirror,Origin mirror
 *
 * @retval None
 *****************************************************************************/
void gui_set_mirroring(uint8_t mirror)
{
    if(mirror == MIRROR_NONE || mirror == MIRROR_HORIZONTAL || 
        mirror == MIRROR_VERTICAL || mirror == MIRROR_ORIGIN) {
        DBG("mirror image x:%s, y:%s\r\n",(mirror & 0x01)? "mirror":"none", ((mirror >> 1) & 0x01)? "mirror":"none");
        cnt.mirror = mirror;
    } else {
        DBG("mirror should be MIRROR_NONE, MIRROR_HORIZONTAL, \
        MIRROR_VERTICAL or MIRROR_ORIGIN\r\n");
    }    
}

/******************************************************************************
 * @brief  Draw Pixels
 *
 * @papam  x_pixel : At point X
 * @papam  y_pixel : At point Y
 * @papam  color   : Painted colors
 *
 * @retval None
 *****************************************************************************/
void gui_set_pixel(uint16_t x_pixel, uint16_t y_pixel, uint16_t color)
{
    if (x_pixel > cnt.width || y_pixel > cnt.height) {
        DBG("Exceeding display boundaries\r\n");
        return;
    }      

    uint16_t x, y;

    switch (cnt.rotate) {
        case 0:
            x = x_pixel;
            y = y_pixel;
            break;

        case 90:
            x = cnt.width_memory - y_pixel - 1;
            y = x_pixel;
            break;

        case 180:
            x = cnt.width_memory - x_pixel - 1;
            y = cnt.height_memory - y_pixel - 1;
            break;

        case 270:
            x = y_pixel;
            y = cnt.height_memory - x_pixel - 1;
            break;

        default:
            return;
    }
    
    switch (cnt.mirror) {
        case MIRROR_NONE:
            break;

        case MIRROR_HORIZONTAL:
            x = cnt.width_memory - x - 1;
            break;

        case MIRROR_VERTICAL:
            y = cnt.height_memory - y - 1;
            break;

        case MIRROR_ORIGIN:
            x = cnt.width_memory - x - 1;
            y = cnt.height_memory - y - 1;
            break;

        default:
            return;
    }

    if (x > cnt.width_memory || y > cnt.height_memory) {
        DBG("Exceeding display boundaries\r\n");
        return;
    }
    
    if (cnt.scale == 2) {
        uint32_t addr = x / 8 + y * cnt.width_byte;
        uint8_t r_data = cnt.content[addr];
        if (color == BLACK)
            cnt.content[addr] = r_data & ~(0x80 >> (x % 8));
        else
            cnt.content[addr] = r_data | (0x80 >> (x % 8));
    } else if (cnt.scale == 4) {
        uint32_t addr = x / 4 + y * cnt.width_byte;
        color = color % 4;                                           //!< Guaranteed color scale is 4  --- 0~3
        uint8_t r_data = cnt.content[addr];
        
        r_data = r_data & (~(0xC0 >> ((x % 4)*2)));
        cnt.content[addr] = r_data | ((color << 6) >> ((x % 4) * 2));
    } else if (cnt.scale == 7) {
		uint32_t addr = x / 2  + y * cnt.width_byte;
		uint8_t r_data = cnt.content[addr];
		r_data = r_data & (~(0xF0 >> ((x % 2)*4)));                 //!< Clear first, then set value
		cnt.content[addr] = r_data | ((color << 4) >> ((x % 2)*4));
	}
}

/******************************************************************************
 * @brief  Clear the color of the picture
 *
 * @papam  Color : Painted colors
 *
 * @retval None
 *****************************************************************************/
void gui_clear(uint16_t color)
{
	if (cnt.scale == 2 || cnt.scale == 4){
		for (uint16_t y = 0; y < cnt.height_byte; y++) {
			for (uint16_t x = 0; x < cnt.width_byte; x++ ) {    //!< 8 pixel =  1 byte
				uint32_t addr = x + y * cnt.width_byte;
				cnt.content[addr] = color;
			}
		}		
	} else if (cnt.scale == 7){
		for (uint16_t y = 0; y < cnt.height_byte; y++) {
			for (uint16_t x = 0; x < cnt.width_byte; x++ ) {
				uint32_t addr = x + y * cnt.width_byte;
				cnt.content[addr] = (color << 4) | color;
			}
		}		
	}
}

/******************************************************************************
 * @brief  Clear the color of a window
 *
 * @papam  x_start : x starting point
 * @papam  y_start : Y starting point
 * @papam  x_end   : x end point
 * @papam  y_end   : y end point
 * @papam  color  : Painted colors
 *
 * @retval None
 *****************************************************************************/
void gui_clear_windows(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    uint16_t x, y;

    for (y = y_start; y < y_end; y++) {
        for (x = x_start; x < x_end; x++) {
            gui_set_pixel(x, y, color);
        }
    }
}

/******************************************************************************
 * @brief  Display Point(x_pixel, y_pixel) Fill the color
 *
 * @papam  x_pixel:         The x_pixel coordinate of the point
 * @papam  y_pixel:         The y_pixel coordinate of the point
 * @papam  color:           Painted color
 * @papam  dot_pixel:       point size
 * @papam  dot_display_way: point Style
 *
 * @retval None
 *****************************************************************************/
void gui_display_point(uint16_t x_pixel, uint16_t y_pixel, uint16_t color,
                                dot_pixel_t dot_pixel, dot_style dot_display_way)
{
    if (x_pixel > cnt.width || y_pixel > cnt.height) {
        DBG("Paint_DrawPoint Input exceeds the normal display range\r\n");
        DBG("x_pixel = %d , cnt.width = %d  \r\n ",x_pixel, cnt.width);
        DBG("y_pixel = %d , cnt.height = %d  \r\n ",y_pixel, cnt.height);
        return;
    }

    int16_t x_dir_num , y_dir_num;
    if (dot_display_way == DOT_FILL_AROUND) {
        for (x_dir_num = 0; x_dir_num < 2 * dot_pixel - 1; x_dir_num++) {
            for (y_dir_num = 0; y_dir_num < 2 * dot_pixel - 1; y_dir_num++) {
                if(x_pixel + x_dir_num - dot_pixel < 0 || y_pixel + y_dir_num - dot_pixel < 0)
                    break;
                gui_set_pixel(x_pixel + x_dir_num - dot_pixel, y_pixel + y_dir_num - dot_pixel, color);
            }
        }
    } else {
        for (x_dir_num = 0; x_dir_num <  dot_pixel; x_dir_num++) {
            for (y_dir_num = 0; y_dir_num <  dot_pixel; y_dir_num++) {
                gui_set_pixel(x_pixel + x_dir_num - 1, y_pixel + y_dir_num - 1, color);
            }
        }
    }
}

/******************************************************************************
 * @brief  Display a line of arbitrary slope
 *
 * @papam  x_start：         Starting x_pixel point coordinates
 * @papam  y_start：         Starting y_pixel point coordinates
 * @papam  x_end：              End point x_pixel coordinate
 * @papam  y_end：              End point y_pixel coordinate
 * @papam  color：              The color of the line segment
 * @papam  line_width: Line width
 * @papam  line_style: Solid and dotted lines
 *
 * @retval None
 *****************************************************************************/
void gui_display_line(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end,
                          uint16_t color, dot_pixel_t line_width, line_style_t line_style)
{
    if (x_start > cnt.width || y_start > cnt.height ||
        x_end > cnt.width || y_end > cnt.height) {
        DBG("Paint_DrawLine Input exceeds the normal display range\r\n");
        return;
    }

    uint16_t x_pixel = x_start;
    uint16_t y_pixel = y_start;
    int dx = (int)x_end - (int)x_start >= 0 ? x_end - x_start : x_start - x_end;
    int dy = (int)y_end - (int)y_start <= 0 ? y_end - y_start : y_start - y_end;

    /* Increment direction, 1 is positive, -1 is counter */
    int x_add_way = x_start < x_end ? 1 : -1;
    int y_add_way = y_start < y_end ? 1 : -1;

    /* Cumulative error */
    int esp = dx + dy;
    char dotted_len = 0;

    for (;;) {
        dotted_len++;
        /* Painted dotted line, 2 point is really virtual */
        if (line_style == LINE_STYLE_DOTTED && dotted_len % 3 == 0) {
           // DBG("LINE_DOTTED\r\n");
            gui_display_point(x_pixel, y_pixel, IMAGE_BACKGROUND, line_width, DOT_STYLE_DFT);
            dotted_len = 0;
        } else {
            gui_display_point(x_pixel, y_pixel, color, line_width, DOT_STYLE_DFT);
        }
        if (2 * esp >= dy) {
            if (x_pixel == x_end)
                break;
            esp += dy;
            x_pixel += x_add_way;
        }
        if (2 * esp <= dx) {
            if (y_pixel == y_end)
                break;
            esp += dx;
            y_pixel += y_add_way;
        }
    }
}

/******************************************************************************
 * @brief  Display a rectangle
 *
 * @papam  x_start：         Starting x_pixel point coordinates
 * @papam  y_start：         Starting y_pixel point coordinates
 * @papam  x_end：              End point x_pixel coordinate
 * @papam  y_end：              End point y_pixel coordinate
 * @papam  color：              The color of the line segment
 * @papam  line_width: Line width
 * @papam  gui_fill:   Whether to fill the inside of the rectangle
 *
 * @retval None
 *****************************************************************************/
void gui_display_rectangle(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end,
                            uint16_t color, dot_pixel_t line_width, gui_fill gui_fill)
{
    if (x_start > cnt.width || y_start > cnt.height ||
        x_end > cnt.width || y_end > cnt.height) {
        DBG("Input exceeds the normal display range\r\n");
        return;
    }

    if (gui_fill) {
        uint16_t y_pixel;
        for (y_pixel = y_start; y_pixel < y_end; y_pixel++) {
            gui_display_line(x_start, y_pixel, x_end, y_pixel, color , line_width, LINE_STYLE_SOLID);
        }
    } else {
        gui_display_line(x_start, y_start, x_end, y_start, color, line_width, LINE_STYLE_SOLID);
        gui_display_line(x_start, y_start, x_start, y_end, color, line_width, LINE_STYLE_SOLID);
        gui_display_line(x_end, y_end, x_end, y_start, color, line_width, LINE_STYLE_SOLID);
        gui_display_line(x_end, y_end, x_start, y_end, color, line_width, LINE_STYLE_SOLID);
    }
}

/******************************************************************************
 * @brief  Use the 8-point method to draw a circle of the
            specified size at the specified position->
 *
 * @papam  x_center  ：   Center X coordinate
 * @papam  y_center  ：   Center Y coordinate
 * @papam  radius    ：   circle Radius
 * @papam  color     ：   The color of the ：circle segment
 * @papam  line_width:  Line width
 * @papam  gui_fill:    Whether to fill the inside of the Circle
 *
 * @retval None
 *****************************************************************************/
void gui_display_circle(uint16_t x_center, uint16_t y_center, uint16_t radius,
        uint16_t color, dot_pixel_t line_width, gui_fill gui_fill)
{
    if (x_center > cnt.width || y_center >= cnt.height) {
        DBG("Paint_DrawCircle Input exceeds the normal display range\r\n");
        return;
    }

    /* Draw a circle from(0, R) as a starting point */
    int16_t x_current, y_current;
    x_current = 0;
    y_current = radius;

    /* Cumulative error,judge the next point of the logo */
    int16_t esp = 3 - (radius << 1 );

    int16_t s_count_y;
    if (gui_fill == GUI_FILL_FULL) {
        while (x_current <= y_current ) { //!< Realistic circles
            for (s_count_y = x_current; s_count_y <= y_current; s_count_y ++ ) {
                gui_display_point(x_center + x_current, y_center + s_count_y, color, DOT_PIXEL_DFT, DOT_STYLE_DFT);  //!< 1
                gui_display_point(x_center - x_current, y_center + s_count_y, color, DOT_PIXEL_DFT, DOT_STYLE_DFT);  //!< 2
                gui_display_point(x_center - s_count_y, y_center + x_current, color, DOT_PIXEL_DFT, DOT_STYLE_DFT);  //!< 3
                gui_display_point(x_center - s_count_y, y_center - x_current, color, DOT_PIXEL_DFT, DOT_STYLE_DFT);  //!< 4
                gui_display_point(x_center - x_current, y_center - s_count_y, color, DOT_PIXEL_DFT, DOT_STYLE_DFT);  //!< 5
                gui_display_point(x_center + x_current, y_center - s_count_y, color, DOT_PIXEL_DFT, DOT_STYLE_DFT);  //!< 6
                gui_display_point(x_center + s_count_y, y_center - x_current, color, DOT_PIXEL_DFT, DOT_STYLE_DFT);  //!< 7
                gui_display_point(x_center + s_count_y, y_center + x_current, color, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            }
            if (esp < 0 )
                esp += 4 * x_current + 6;
            else {
                esp += 10 + 4 * (x_current - y_current );
                y_current --;
            }
            x_current ++;
        }
    } else { //Draw a hollow circle
        while (x_current <= y_current ) {
            gui_display_point(x_center + x_current, y_center + y_current, color, line_width, DOT_STYLE_DFT);  //!< 1
            gui_display_point(x_center - x_current, y_center + y_current, color, line_width, DOT_STYLE_DFT);  //!< 2
            gui_display_point(x_center - y_current, y_center + x_current, color, line_width, DOT_STYLE_DFT);  //!< 3
            gui_display_point(x_center - y_current, y_center - x_current, color, line_width, DOT_STYLE_DFT);  //!< 4
            gui_display_point(x_center - x_current, y_center - y_current, color, line_width, DOT_STYLE_DFT);  //!< 5
            gui_display_point(x_center + x_current, y_center - y_current, color, line_width, DOT_STYLE_DFT);  //!< 6
            gui_display_point(x_center + y_current, y_center - x_current, color, line_width, DOT_STYLE_DFT);  //!< 7
            gui_display_point(x_center + y_current, y_center + x_current, color, line_width, DOT_STYLE_DFT);  //!< 0

            if (esp < 0 )
                esp += 4 * x_current + 6;
            else {
                esp += 10 + 4 * (x_current - y_current );
                y_current --;
            }
            x_current ++;
        }
    }
}

/******************************************************************************
 * @brief  Show English characters
 *
 * @papam  x_pixel：                                 X coordinate
 * @papam  y_pixel：                                 Y coordinate
 * @papam  acsii_char：                         To display the English characters
 * @papam  font:                A structure pointer that displays a character size
 * @papam  color_foreground:    Select the foreground color
 * @papam  color_background:    Select the background color
 *
 * @retval None
 *****************************************************************************/
void gui_display_char(uint16_t x_pixel, uint16_t y_pixel, const char acsii_char, sFONT* font,
                                        uint16_t color_foreground, uint16_t color_background)
{
    uint16_t page, column;

    if (x_pixel > cnt.width || y_pixel > cnt.height) {
        DBG("Paint_DrawChar Input exceeds the normal display range\r\n");
        return;
    }

    uint32_t char_offset = (acsii_char - ' ') * font->Height * (font->Width / 8 + (font->Width % 8 ? 1 : 0));
    const unsigned char *ptr = &font->table[char_offset];

    for (page = 0; page < font->Height; page++ ) {
        for (column = 0; column < font->Width; column++ ) {
            /* To determine whether the font background color and screen background color is consistent */
            if (FONT_BACKGROUND == color_background) { //!< this process is to speed up the scan
                if (*ptr & (0x80 >> (column % 8)))
                    gui_set_pixel(x_pixel + column, y_pixel + page, color_foreground);
            } else {
                if (*ptr & (0x80 >> (column % 8))) {
                    gui_set_pixel(x_pixel + column, y_pixel + page, color_foreground);
                } else {
                    gui_set_pixel(x_pixel + column, y_pixel + page, color_background);
                }
            }
            /* One pixel is 8 bits */
            if (column % 8 == 7)
                ptr++;
        } //!< Write a line
        if (font->Width % 8 != 0)
            ptr++;
    }//!< Write all
}

/******************************************************************************
 * @brief  Display the string
 *
 * @papam  x_start:          X coordinate
 * @papam  x_start:          Y coordinate
 * @papam  p_string:         The first address of the English string to be displayed
 * @papam  font:             A structure pointer that displays a character size
 * @papam  color_foreground: Select the foreground color
 * @papam  color_background: Select the background color
 *
 * @retval None
 *****************************************************************************/
void gui_display_string_en(uint16_t x_start, uint16_t y_start, const char *p_string, sFONT* font,
                            uint16_t color_foreground, uint16_t color_background)
{
    uint16_t x_pixel = x_start;
    uint16_t y_pixel = y_start;

    if (x_start > cnt.width || y_start > cnt.height) {
        DBG("Paint_DrawString_EN Input exceeds the normal display range\r\n");
        return;
    }

    while (* p_string != '\0') {
        /* if X direction filled, reposition to(x_start, y_pixel), y_pixel is Y direction plus the Height of the character */
        if ((x_pixel + font->Width ) > cnt.width ) {
            x_pixel = x_start;
            y_pixel += font->Height;
        }

        /* If the Y direction is full, reposition to (x_start, y_start) */
        if ((y_pixel + font->Height ) > cnt.height ) {
            x_pixel = x_start;
            y_pixel = y_start;
        }
        gui_display_char(x_pixel, y_pixel, *p_string, font, color_background, color_foreground);

        /* The next character of the address */
        p_string++;

        /* The next word of the abscissa increases the font of the broadband */
        x_pixel += font->Width;
    }
}

/******************************************************************************
 * @brief  Display number
 *
 * @papam  x_pixel:          X coordinate
 * @papam  y_pixel:          Y coordinate
 * @papam  number:           The number displayed
 * @papam  font：                                  A structure pointer that displays a character size
 * @papam  color_foreground: Select the foreground color
 * @papam  color_background: Select the background color
 *
 * @retval None
 *****************************************************************************/
void gui_display_num(uint16_t x_pixel, uint16_t y_pixel, int32_t number, sFONT* font,
                                uint16_t color_foreground, uint16_t color_background)
{

    int16_t num_bit = 0, str_bit = 0;
    uint8_t str_array[ARRAY_LEN] = {0}, num_array[ARRAY_LEN] = {0};
    uint8_t *p_str = str_array;

    if (x_pixel > cnt.width || y_pixel > cnt.height) {
        DBG("Paint_DisNum Input exceeds the normal display range\r\n");
        return;
    }

    /* Converts a number to a string */
    while (number) {
        num_array[num_bit] = number % 10 + '0';
        num_bit++;
        number /= 10;
    }

    /* The string is inverted */
    while (num_bit > 0) {
        str_array[str_bit] = num_array[num_bit - 1];
        str_bit++;
        num_bit--;
    }

    /* Display */
    gui_display_string_en(x_pixel, y_pixel, (const char*)p_str, font, color_background, color_foreground);
}

/******************************************************************************
 * @brief  Display time
 *
 * @papam  x_start:          X coordinate
 * @papam  y_start:          Y coordinate
 * @papam  p_time:           Time-related structures
 * @papam  font：                                 A structure pointer that displays a character size
 * @papam  color_foreground: Select the foreground color
 * @papam  color_background: Select the background color
 *
 * @retval None
 *****************************************************************************/
void gui_display_time(uint16_t x_start, uint16_t y_start, gui_time *p_time, sFONT* font,
                                uint16_t color_foreground, uint16_t color_background)
{
    uint8_t value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    uint16_t dx = font->Width;

    /* Write data into the cache */
    gui_display_char(x_start                           , y_start, value[p_time->hour / 10], font, color_background, color_foreground);
    gui_display_char(x_start + dx                      , y_start, value[p_time->hour % 10], font, color_background, color_foreground);
    gui_display_char(x_start + dx  + dx / 4 + dx / 2   , y_start, ':'                    , font, color_background, color_foreground);
    gui_display_char(x_start + dx * 2 + dx / 2         , y_start, value[p_time->min / 10] , font, color_background, color_foreground);
    gui_display_char(x_start + dx * 3 + dx / 2         , y_start, value[p_time->min % 10] , font, color_background, color_foreground);
    gui_display_char(x_start + dx * 4 + dx / 2 - dx / 4, y_start, ':'                    , font, color_background, color_foreground);
    gui_display_char(x_start + dx * 5                  , y_start, value[p_time->sec / 10] , font, color_background, color_foreground);
    gui_display_char(x_start + dx * 6                  , y_start, value[p_time->sec % 10] , font, color_background, color_foreground);
}

/******************************************************************************
 * @brief  Display monochrome bitmap
 * @Note   Use a computer to convert the image into a corresponding array,
           and then embed the array directly into Imagedata.cpp as a .c file.
 *
 * @papam  content_buffer ：A picture data converted to a bitmap
 *
 * @retval None
 *****************************************************************************/
void gui_display_bit_map(const unsigned char* content_buffer)
{
    uint16_t x, y;
    uint32_t addr = 0;

    for (y = 0; y < cnt.height_byte; y++) {
        for (x = 0; x < cnt.width_byte; x++) {
            addr = x + y * cnt.width_byte;
            cnt.content[addr] = (unsigned char)content_buffer[addr];
        }
    }
}

/******************************************************************************
 * @brief  Display half of monochrome bitmap
 *
 * @papam  content_buffer ：A picture data converted to a bitmap
 * @param  Region : 1 Upper half
                    2 Lower half
 *
 * @retval None
 *****************************************************************************/
void gui_display_bit_map_half(const unsigned char* content_buffer, uint8_t region)
{
    uint16_t x, y;
    uint32_t addr = 0;

    if (region == 1){
        for (y = 0; y < cnt.height_byte; y++) {
            for (x = 0; x < cnt.width_byte; x++) {
                addr = x + y * cnt.width_byte;
                cnt.content[addr] = (unsigned char)content_buffer[addr];
            }
        }
    } else {
        for (y = 0; y < cnt.height_byte; y++) {
            for (x = 0; x < cnt.width_byte; x++) {
                addr = x + y * cnt.width_byte ;
                cnt.content[addr] = \
                (unsigned char)content_buffer[addr + (cnt.height_byte) * cnt.width_byte];
            }
        }
    }
}

/******************************************************************************
 * @brief  Display half of monochrome bitmap
 *
 * @papam  content_buffer ：A picture data converted to a bitmap
 * @param  Region : 1 Upper half
                    2 Lower half
 *
 * @retval None
 *****************************************************************************/
void gui_display_bit_map_one_quarter(const unsigned char* content_buffer, uint8_t region)
{
    uint16_t x, y;
    uint32_t addr = 0;

    if (region == 1) {
        for (y = 0; y < cnt.height_byte; y++) {
            for (x = 0; x < cnt.width_byte; x++) {
                addr = x + y * cnt.width_byte;
                cnt.content[addr] = (unsigned char)content_buffer[addr];
            }
        }
    } else if (region == 2) {
        for (y = 0; y < cnt.height_byte; y++) {
            for (x = 0; x < cnt.width_byte; x++) {
                addr = x + y * cnt.width_byte ;
                cnt.content[addr] = \
                (unsigned char)content_buffer[addr + (cnt.height_byte) * cnt.width_byte];
            }
        }
    } else if (region == 3) {
        for (y = 0; y < cnt.height_byte; y++) {
            for (x = 0; x < cnt.width_byte; x++) {
                addr = x + y * cnt.width_byte ;
                cnt.content[addr] = \
                (unsigned char)content_buffer[addr + (cnt.height_byte) * cnt.width_byte * 2];
            }
        }
    } else if (region == 4) {
        for (y = 0; y < cnt.height_byte; y++) {
            for (x = 0; x < cnt.width_byte; x++) {
                addr = x + y * cnt.width_byte ;
                cnt.content[addr] = \
                (unsigned char)content_buffer[addr + (cnt.height_byte) * cnt.width_byte * 3];
            }
        }
    }
}

/******************************************************************************
 * @brief  Display Block of monochrome bitmap
 *
 * @papam  content_buffer ：A picture data converted to a bitmap
 * @param  Region : 1 Upper half
                    2 Lower half
 *
 * @retval None
 *****************************************************************************/
void gui_display_bit_map_block(const unsigned char* content_buffer, uint8_t region)
{
    uint16_t x, y;
    uint32_t addr = 0;

    for (y = 0; y < cnt.height_byte; y++) {
        for (x = 0; x < cnt.width_byte; x++) {
            addr = x + y * cnt.width_byte ;
            cnt.content[addr] = \
            (unsigned char)content_buffer[addr + (cnt.height_byte) * cnt.width_byte * (region - 1)];
        }
    }
}

