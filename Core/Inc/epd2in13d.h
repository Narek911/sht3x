#ifndef __EPD_2IN13D_H__
#define __EPD_2IN13D_H__

#include "util.h"

// Display resolution
#define EPD_2IN13D_WIDTH                            104
#define EPD_2IN13D_HEIGHT                           212

#define PANEL_SETTING                               0x00
#define POWER_SETTING                               0x01
#define POWER_OFF                                   0x02
#define POWER_OFF_SEQUENCE_SETTING                  0x03
#define POWER_ON                                    0x04
#define POWER_ON_MEASURE                            0x05
#define BOOSTER_SOFT_START                          0x06
#define DEEP_SLEEP                                  0x07
#define DATA_START_TRANSMISSION_1                   0x10
#define DATA_STOP                                   0x11
#define DISPLAY_REFRESH                             0x12
#define DATA_START_TRANSMISSION_2                   0x13
#define AUTO_SEQUENCE                               0x17
#define VCOM_LUT                                    0x20
#define W2W_LUT                                     0x21
#define B2W_LUT                                     0x22
#define W2B_LUT                                     0x23
#define B2B_LUT                                     0x24
#define LUT_OPTION                                  0x2A
#define PLL_CONTROL                                 0x30
#define TEMPERATURE_SENSOR_CALIBRATION              0x40
#define TEMPERATURE_SENSOR_SELECTION                0x41
#define TEMPERATURE_SENSOR_WRITE                    0x42
#define TEMPERATURE_SENSOR_READ                     0x43
#define PANEL_GLASS_CHECK                           0x44
#define VCOM_AND_DATA_INTERVAL_SETTING              0x50
#define LOW_POWER_DETECTION                         0x51
#define TCON_SETTING                                0x60
#define RESOLUTION_SETTING                          0x61
#define GATE_SOURCE_START_SETTING                   0x65
#define REVISION                                    0x70
#define GET_STATUS                                  0x71
#define AUTO_MEASURE_VCOM                           0x80
#define READ_VCOM_VALUE                             0x81
#define VCM_DC_SETTING                              0x82
#define PARTIAL_WINDOW                              0x90
#define PARTIAL_IN                                  0x91
#define PARTIAL_OUT                                 0x92
#define PROGRAM_MODE                                0xA0
#define ACTIVE_PROGRAM                              0xA1
#define READ_OTP_DATA                               0xA2
#define CASCADE_SETTING                             0xE0
#define POWER_SAVING                                0xE3
#define LVD_VOLTAGE_SELECT                          0xE4
#define FORCE_TEMPERATURE                           0xE5

extern __IO uint32_t epd_timer;

void epd_2in13d_init(void);
void epd_2in13d_clear(void);
void epd_2in13d_display(uint8_t *content);
void epd_2in13d_display_part(uint8_t *content);
void epd_2in13d_sleep(void);
void epd_2in13d_loop(void);
int epd_2in13d_prepare(void);
void epd_2in13d_transmit_complete_callback(void);
void epd_2in13d_transfer_error_callback(void);

#endif /* __EPD_2IN13D_H__ */
