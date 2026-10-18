/**
  ******************************************************************************
  * @file    oled_i2c.h
  * @brief   0.96" SSD1306 OLED (128x64) driver over software (bit-bang) I2C.
  *
  *          Wiring (4-pin module):
  *            VCC  -> 3.3V
  *            GND  -> GND
  *            SCL  -> PB8
  *            SDA  -> PB9
  *
  *          The STM32F103C8T6 has no hardware I2C on PB8/PB9 (I2C1 is fixed
  *          on PB6/PB7, I2C2 does not exist on this density), so the bus is
  *          bit-banged: open-drain outputs, lines released high via the weak
  *          internal pull-ups (input pull-up mode), ~100 kHz.
  ******************************************************************************
  */
#ifndef OLED_I2C_H
#define OLED_I2C_H

#include "main.h"

/* Screen geometry (128x64 SSD1306). */
#define OLED_WIDTH  128U
#define OLED_HEIGHT 64U

/**
  * @brief Initialize I2C pins, probe the SSD1306 (0x78 then 0x3C) and
  *        run the display init sequence.
  * @retval 0 on success, -1 if no OLED ACKed its address.
  */
int OLED_Init(void);

/** Clear the internal frame buffer. */
void OLED_Clear(void);

/** Fill the frame buffer with a solid color (0 = black, 255 = white). */
void OLED_FillScreen(uint8_t color);

/** Push the frame buffer to the display (1024 bytes over I2C).
 *  The index of the first NACKed byte is stored in oled_refresh_fail
 *  (0xFFFF = address/control NACK, 0..1024 = data byte 0..1023,
 *  0x0000 = everything ACKed). */
void OLED_Refresh(void);

/** Send a single command byte to the display. @retval 0 = ACKed. */
int OLED_SendCmd(uint8_t c);

/** Debug: first NACKed byte during the last OLED_Refresh (see above). */
extern volatile uint16_t oled_refresh_fail;

/**
  * @brief Draw one 6x8 ASCII character (clipped at the screen edges).
  * @param x Start column, may be negative or > 127 (off-screen part is clipped).
  * @param y Start row 0..56 (pixel).
  * @param c Printable ASCII 0x20..0x7E.
  */
void OLED_ShowChar(int16_t x, uint8_t y, char c);

/** Draw a string starting at (x, y), 6 px advance per character. */
void OLED_ShowString(int16_t x, uint8_t y, const char *s);

/** Draw a string with inverted pixels (dark text on a light background). */
void OLED_ShowStringInv(int16_t x, uint8_t y, const char *s);

/**
 * @brief Set one frame-buffer pixel (off-screen coordinates are clipped).
 * @param x Column 0..127.
 * @param y Row 0..63 (pixel).
 */
void OLED_DrawPixel(int16_t x, int16_t y);

/**
 * @brief Fill a w x h rectangle in the frame buffer (clipped at edges).
 */
void OLED_FillRect(int16_t x, int16_t y, int16_t w, int16_t h);

#endif /* OLED_I2C_H */
