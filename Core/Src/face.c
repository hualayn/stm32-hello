/**
 ******************************************************************************
 * @file    face.c
 * @brief   Smiley face composed from OLED_DrawPixel / OLED_FillRect.
 *
 *          Geometry (local 48x48 coords, origin top-left):
 *            head   : circle center (24,24) radius 21, 2-px outline
 *                     (two concentric circles, r and r-1)
 *            eyes   : open  = 4x4 blocks at (13,15) and (31,15)
 *                     closed = 4x1 lines at  (13,16) and (31,16)
 *            smile  : lower arc of a circle centered at (24,20) radius 13,
 *                     rows 26..33, |dx| <= 11, 2-px thick
 ******************************************************************************
 */
#include "face.h"
#include "oled_i2c.h"

#define FACE_CX 24U
#define FACE_CY 24U
#define FACE_R  21U

/* floor(sqrt(n)), n small (n <= 0x7FF). Classic odd-sum method:
 * 1 + 3 + 5 + ... + (2x-1) = x^2, so add odd numbers while the running
 * sum stays <= n; the count is the result. */
static uint8_t isqrt_u(uint16_t n)
{
  uint8_t x = 0U;
  uint16_t sum = 0U;
  while (sum + 2U * x + 1U <= n)
  {
    sum = (uint16_t)(sum + 2U * x + 1U);
    x++;
  }
  return x;
}

/* 2-px ring around (FACE_CX, FACE_CY): circles of radius r and r-1. */
static void face_ring(int16_t x0, int16_t y0, uint8_t r)
{
  for (uint8_t pass = 0U; pass < 2U; pass++)
  {
    uint8_t rr = (uint8_t)(r - pass);
    uint16_t r2 = (uint16_t)rr * rr;
    for (int16_t dy = -(int16_t)rr; dy <= (int16_t)rr; dy++)
    {
      uint8_t dx = isqrt_u((uint16_t)(r2 - (uint16_t)(dy * dy)));
      OLED_DrawPixel((int16_t)(x0 + FACE_CX - dx), (int16_t)(y0 + FACE_CY + dy));
      OLED_DrawPixel((int16_t)(x0 + FACE_CX + dx), (int16_t)(y0 + FACE_CY + dy));
    }
  }
}

void Face_Smiley(int16_t x0, int16_t y0, uint8_t blink)
{
  /* head */
  face_ring(x0, y0, FACE_R);

  /* eyes */
  if (blink)
  {
    OLED_FillRect(x0 + 13, y0 + 16, 4, 1);
    OLED_FillRect(x0 + 31, y0 + 16, 4, 1);
  }
  else
  {
    OLED_FillRect(x0 + 13, y0 + 15, 4, 4);
    OLED_FillRect(x0 + 31, y0 + 15, 4, 4);
  }

  /* smile: lower arc of the circle (24,20) r=13, 2-px thick */
  for (int16_t dy = 6; dy <= 13; dy++)
  {
    uint8_t dx = isqrt_u((uint16_t)(169U - (uint16_t)(dy * dy)));
    if (dx > 11U)
    {
      continue; /* narrow the mouth: keep |dx| <= 11 */
    }
    for (uint8_t t = 0U; t < 2U; t++)
    {
      OLED_DrawPixel((int16_t)(x0 + 24 - dx), (int16_t)(y0 + 20 + dy + t));
      OLED_DrawPixel((int16_t)(x0 + 24 + dx), (int16_t)(y0 + 20 + dy + t));
    }
  }
}
