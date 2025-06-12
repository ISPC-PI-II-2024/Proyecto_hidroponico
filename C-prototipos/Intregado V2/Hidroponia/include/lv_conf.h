/**
 * @file lv_conf.h
 * Configuration file for v8 of the LittlevGL graphics library
 */

 #ifndef LV_CONF_H
 #define LV_CONF_H
 
 /*====================
    COLOR SETTINGS
  *====================*/
 
 /* Color depth:
  *  - 1: 1 byte per pixel
  *  - 8: RGB332
  *  - 16: RGB565
  *  - 32: ARGB8888
  */
 #define LV_COLOR_DEPTH     16
 
 /* Enable anti-aliasing (slightly smoother edges) */
 #define LV_ANTIALIAS       1
 
 /*====================
    DISPLAY SETTINGS
  *====================*/
 
 /* Default display refresh period in milliseconds */
 #define LV_DISP_DEF_REFR_PERIOD    30
 
 /* Default screen horizontal and vertical resolution */
 #define LV_HOR_RES_MAX    240
 #define LV_VER_RES_MAX    320
 
 /*====================
    INPUT DEVICE SETTINGS
  *====================*/
 
 /* Enable input device support (touch, encoder, buttons) */
 #define LV_USE_INDEV      1
 
 /*====================
    FONT USAGE
  *====================*/
 
 #define LV_FONT_MONTSERRAT_12     1
 #define LV_FONT_MONTSERRAT_14     1
 #define LV_FONT_DEFAULT           &lv_font_montserrat_14
 
 /*====================
    MISC SETTINGS
  *====================*/
 
 #define LV_USE_LOG      1
 #define LV_LOG_LEVEL    LV_LOG_LEVEL_WARN
 
 /*====================
    FEATURE ENABLE/DISABLE
  *====================*/
 
 #define LV_USE_ASSERT_NULL        1
 #define LV_USE_ASSERT_MEM         1
 #define LV_USE_ASSERT_STR         1
 
 /*====================
    COMPILER SETTINGS
  *====================*/
 
 /* For ESP32 usually nothing extra is needed */
 #define LV_ATTRIBUTE_FAST_MEM
 #define LV_ATTRIBUTE_LARGE_CONST
 
 /*====================
    DRAWING CONFIG
  *====================*/
 
 /* Enable support for drawing on CPU (without GPU acceleration) */
 #define LV_DRAW_COMPLEX           1
 
 #endif /*LV_CONF_H*/
 