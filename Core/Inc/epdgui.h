#ifndef __EPD_GUI_H__
#define __EPD_GUI_H__

#include "util.h"
#include "fonts.h"

/* Content attributes */
typedef struct {
    uint8_t *content;
    uint16_t width;
    uint16_t height;
    uint16_t width_memory;
    uint16_t height_memory;
    uint16_t color;
    uint16_t rotate;
    uint16_t mirror;
    uint16_t width_byte;
    uint16_t height_byte;
    uint16_t scale;
} content;

extern content cnt;

/* Display rotate */
#define ROTATE_0            0
#define ROTATE_90           90
#define ROTATE_180          180
#define ROTATE_270          270

/* Display Flip */
typedef enum {
    MIRROR_NONE       = 0x00,
    MIRROR_HORIZONTAL = 0x01,
    MIRROR_VERTICAL   = 0x02,
    MIRROR_ORIGIN     = 0x03,
} mirror_content_t;

#define MIRROR_IMAGE_DFT MIRROR_NONE

/* Content color */
#define WHITE          0xFF
#define BLACK          0x00
#define RED            BLACK

#define IMAGE_BACKGROUND    WHITE
#define FONT_FOREGROUND     BLACK
#define FONT_BACKGROUND     WHITE

/* 4 Gray level */
#define  GRAY1 0x03 //!< Blackest
#define  GRAY2 0x02
#define  GRAY3 0x01 //!< gray
#define  GRAY4 0x00 //!< white

/* The size of the pixel */
typedef enum {
    DOT_PIXEL_1X1  = 1,		//!< 1 x 1
    DOT_PIXEL_2X2  , 		//!< 2 X 2
    DOT_PIXEL_3X3  ,		//!< 3 X 3
    DOT_PIXEL_4X4  ,		//!< 4 X 4
    DOT_PIXEL_5X5  , 		//!< 5 X 5
    DOT_PIXEL_6X6  , 		//!< 6 X 6
    DOT_PIXEL_7X7  , 		//!< 7 X 7
    DOT_PIXEL_8X8  , 		//!< 8 X 8
} dot_pixel_t;

#define DOT_PIXEL_DFT  DOT_PIXEL_1X1

/* Point size fill style */
typedef enum {
    DOT_FILL_AROUND  = 1,		//!< dot pixel 1 x 1
    DOT_FILL_RIGHTUP  , 		//!< dot pixel 2 X 2
} dot_style;

#define DOT_STYLE_DFT  DOT_FILL_AROUND

/* Line style, solid or dashed */
typedef enum {
    LINE_STYLE_SOLID = 0,
    LINE_STYLE_DOTTED,
} line_style_t;

/* Whether the content is filled */
typedef enum {
    GUI_FILL_EMPTY = 0,
    GUI_FILL_FULL,
} gui_fill;

/* Custom structure of a time attribute*/
typedef struct {
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;
    uint8_t  min;
    uint8_t  sec;
} gui_time;

extern gui_time e_gui_time;

void gui_new_content(uint8_t *content, uint16_t width, uint16_t height, uint16_t rotate, uint16_t color);
void gui_select_content(uint8_t *content);
void gui_set_rotate(uint16_t rotate);
void gui_set_mirroring(uint8_t mirror);
void gui_set_pixel(uint16_t x_pixel, uint16_t y_pixel, uint16_t color);
void gui_set_scale(uint8_t scale);
void gui_clear(uint16_t color);
void gui_clear_windows(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
void gui_display_point(uint16_t x_pixel, uint16_t y_pixel, uint16_t color, dot_pixel_t dot_pixel, dot_style dot_display_way);
void gui_display_line(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color, dot_pixel_t line_width, line_style_t line_style);
void gui_display_rectangle(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color, dot_pixel_t line_width, gui_fill gui_fill);
void gui_display_circle(uint16_t x_center, uint16_t y_center, uint16_t radius, uint16_t color, dot_pixel_t line_width, gui_fill gui_fill);

/* String */
void gui_display_char(uint16_t x_pixel, uint16_t y_pixel, const char acsii_char, sFONT* font, uint16_t color_foreground, uint16_t color_background);
void gui_display_string_en(uint16_t x_start, uint16_t y_start, const char *p_string, sFONT* font, uint16_t color_foreground, uint16_t color_background);
void gui_display_num(uint16_t x_pixel, uint16_t y_pixel, int32_t number, sFONT* font, uint16_t color_foreground, uint16_t color_background);
void gui_display_time(uint16_t x_start, uint16_t y_start, gui_time *p_time, sFONT* font, uint16_t color_foreground, uint16_t color_background);

/* Picture */
void gui_display_bit_map(const unsigned char* content_buffer);
void gui_display_bit_map_half(const unsigned char* content_buffer, uint8_t region);
void gui_display_bit_map_one_quarter(const unsigned char* content_buffer, uint8_t region);
void gui_display_bit_map_block(const unsigned char* content_buffer, uint8_t region);
#endif





