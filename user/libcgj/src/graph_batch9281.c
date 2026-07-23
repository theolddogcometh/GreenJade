/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9281: FreeType face ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ft_face_ok_u_9281(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       FreeType FT_Face validity probe.
 *   uint32_t __gj_ft_face_ok_u_9281  (alias)
 *   __libcgj_batch9281_marker = "libcgj-batch9281"
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

const char __libcgj_batch9281_marker[] = "libcgj-batch9281";

/* Soft FreeType face-ok lamp: always off (not a real FT_Face probe). */
#define B9281_FT_FACE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9281_ft_face_ok(void)
{
	return B9281_FT_FACE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ft_face_ok_u_9281 - FreeType face ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open font faces or
 * call FreeType/libc. No parent wires.
 */
uint32_t
gj_ft_face_ok_u_9281(void)
{
	(void)NULL;
	return b9281_ft_face_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ft_face_ok_u_9281(void)
    __attribute__((alias("gj_ft_face_ok_u_9281")));
