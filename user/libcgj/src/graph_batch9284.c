/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9284: FreeType bitmap ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ft_bitmap_ok_u_9284(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       FreeType FT_Bitmap validity probe.
 *   uint32_t __gj_ft_bitmap_ok_u_9284  (alias)
 *   __libcgj_batch9284_marker = "libcgj-batch9284"
 *
 * Exclusive continuum CREATE-ONLY (9281-9290: freetype soft id stubs —
 * ft_face_ok_u_9281, ft_size_ok_u_9282, ft_glyph_ok_u_9283,
 * ft_bitmap_ok_u_9284, ft_outline_ok_u_9285, ft_kerning_ok_u_9286,
 * ft_load_ok_u_9287, ft_render_ok_u_9288, freetype_ready_u_9289,
 * batch_id_9290). Unique surface only; no multi-def. No parent wires.
 * No __int128. No FreeType implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9284_marker[] = "libcgj-batch9284";

/* Soft FreeType bitmap-ok lamp: always off (not a real bitmap probe). */
#define B9284_FT_BITMAP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9284_ft_bitmap_ok(void)
{
	return B9284_FT_BITMAP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ft_bitmap_ok_u_9284 - FreeType bitmap ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect glyph bitmaps
 * or call FreeType/libc. No parent wires.
 */
uint32_t
gj_ft_bitmap_ok_u_9284(void)
{
	(void)NULL;
	return b9284_ft_bitmap_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ft_bitmap_ok_u_9284(void)
    __attribute__((alias("gj_ft_bitmap_ok_u_9284")));
