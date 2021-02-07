/*
** Jo Sega Saturn Engine
** Copyright (c) 2012-2020, Johannes Fetz (johannesfetz@gmail.com)
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the Johannes Fetz nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL Johannes Fetz BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/** @file colors.h
 *  @author Johannes Fetz
 *
 *  @brief Jo Engine Standard colors
 *  @bug No known bugs.
 */

#ifndef __JO_COLORS_H__
# define __JO_COLORS_H__

/*
** MACROS
*/

/*
** for jo_set_printf_color_index
*/
#define JO_COLOR_INDEX_White    (0)
#define JO_COLOR_INDEX_Black    (1)
#define JO_COLOR_INDEX_Red      (2)
#define JO_COLOR_INDEX_Green    (3)
#define JO_COLOR_INDEX_Yellow   (4)
#define JO_COLOR_INDEX_Blue     (5)
#define JO_COLOR_INDEX_Purple   (6)

/*
** jo_color constants
*/
# define JO_COLOR_White			(65535)
# define JO_COLOR_Black			(32768)
# define JO_COLOR_Red			(32799)
# define JO_COLOR_Green			(33760)
# define JO_COLOR_Yellow		(33791)
# define JO_COLOR_Blue			(64512)
# define JO_COLOR_Purple		(64543)
# define JO_COLOR_Cyan			(65504)
# define JO_COLOR_Gray		    (58136)
# define JO_COLOR_DarkRed		(32776)
# define JO_COLOR_DarkGreen		(33024)
# define JO_COLOR_DarkYellow	(33032)
# define JO_COLOR_DarkBlue		(40960)
# define JO_COLOR_DarkPurple	(40968)
# define JO_COLOR_DarkCyan		(41216)
# define JO_COLOR_DarkGray		(41224)
# define JO_COLOR_MediumRed		(32784)
# define JO_COLOR_MediumGreen	(33280)
# define JO_COLOR_MediumYellow	(33296)
# define JO_COLOR_MediumBlue	(49152)
# define JO_COLOR_MediumPurple	(49168)
# define JO_COLOR_MediumCyan	(49664)
# define JO_COLOR_MediumWhite	(49680)
# define JO_COLOR_SemiRed		(32792)
# define JO_COLOR_SemiGreen		(33536)
# define JO_COLOR_SemiYellow	(33560)
# define JO_COLOR_SemiBlue		(57344)
# define JO_COLOR_SemiPurple	(57368)
# define JO_COLOR_SemiCyan		(58112)

/** @brief Transparent color */
# define JO_COLOR_Transparent	(0)

/** @brief Internal, you should not use this macro. */
# define JO_COLOR_SATURN(C)         (JO_DIV_BY_8(C) & 0x1f)
/** @brief Internal, you should not use this macro. */
# define JO_COLOR_SATURN_RGB(R,G,B)	((jo_color)(0x8000 | ((B) << 10) | ((G) << 5) | (R)))
/** @brief Internal, you should not use this macro. */
# define JO_COLOR_SATURN_GET_R(C)   ((C) & 0x1f)
# define JO_COLOR_SATURN_GET_G(C)   (((C) >> 5) & 0x1f)
# define JO_COLOR_SATURN_GET_B(C)   (((C) >> 10) & 0x1f)

/** @brief Get jo_color type from RGB additive color model
 *  @param R Red [0, 255]
 *  @param G Green [0, 255]
 *  @param B Blue [0, 255]
 *  @return jo_color
*/
# define JO_COLOR_RGB(R,G,B)	    JO_COLOR_SATURN_RGB(JO_COLOR_SATURN(R), JO_COLOR_SATURN(G), JO_COLOR_SATURN(B))

/** @brief Set color filter A for scroll screen
 *  @param screens Scroll screens (You can pass multiple value using pipe(|). Example: JO_SPRITE_SCREEN|JO_NBG0_SCREEN)
 *  @param r Red offset value (signed 9 bits)
 *  @param g Green offset value (signed 9 bits)
 *  @param b Blue offset value (signed 9 bits)
 */
static  __jo_force_inline void      jo_set_screen_color_filter_a(jo_scroll_screen screens, short r, short g, short b)
{
    slColOffsetA(r, g, b);
    slColOffsetAUse(screens);
}

/** @brief Set color filter A for scroll screen
 *  @param screens Scroll screens (You can pass multiple value using pipe(|). Example: JO_SPRITE_SCREEN|JO_NBG0_SCREEN)
 *  @param r Red offset value (signed 9 bits)
 *  @param g Green offset value (signed 9 bits)
 *  @param b Blue offset value (signed 9 bits)
 */
static  __jo_force_inline void      jo_set_screen_color_filter_b(jo_scroll_screen screens, short r, short g, short b)
{
    slColOffsetB(r, g, b);
    slColOffsetBUse(screens);
}

/** @brief Disable color filter for scroll screen
 *  @param screens Scroll screens (You can pass multiple value using pipe(|). Example: JO_SPRITE_SCREEN|JO_NBG0_SCREEN)
 */
static  __jo_force_inline void      jo_disable_screen_color_filter(jo_scroll_screen screens)
{
    slColOffsetOff(screens);
}

/** @brief Disable color filter for all scroll screen
 */
static  __jo_force_inline void      jo_disable_all_screen_color_filter(void)
{
    jo_disable_screen_color_filter(JO_ALL_SCROLL_SCREEN);
}

#endif /* !__JO_COLORS_H__ */

/*
** END OF FILE
*/
