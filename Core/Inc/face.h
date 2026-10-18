/**
 ******************************************************************************
 * @file    face.h
 * @brief   Emoji-style faces drawn from driver primitives (no bitmap data).
 *
 *          A face is a 48x48 composition of circles / arcs / rectangles, so
 *          every new expression (blink, surprise, sad) is a parameter change
 *          instead of a hand-drawn bitmap.
 ******************************************************************************
 */
#ifndef FACE_H
#define FACE_H

#include <stdint.h>

/**
 * @brief Draw a 48x48 smiley face, top-left corner at (x0, y0).
 * @param x0 Column of the top-left corner (off-screen part is clipped).
 * @param y0 Row of the top-left corner.
 * @param blink 0 = open eyes (4x4 blocks), 1 = closed eyes (4x1 lines).
 */
void Face_Smiley(int16_t x0, int16_t y0, uint8_t blink);

#endif /* FACE_H */
