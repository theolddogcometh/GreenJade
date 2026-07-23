/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9290: freetype soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9290(void);
 *     - Returns the compile-time graph batch number for this TU (9290).
 *   uint32_t __gj_batch_id_9290  (alias)
 *   __libcgj_batch9290_marker = "libcgj-batch9290"
 *
 * Exclusive continuum CREATE-ONLY (9281-9290: freetype soft id stubs —
 * ft_face_ok_u_9281, ft_size_ok_u_9282, ft_glyph_ok_u_9283,
 * ft_bitmap_ok_u_9284, ft_outline_ok_u_9285, ft_kerning_ok_u_9286,
 * ft_load_ok_u_9287, ft_render_ok_u_9288, freetype_ready_u_9289,
 * batch_id_9290). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9290_marker[] = "libcgj-batch9290";

#define B9290_BATCH_ID  9290u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9290_id(void)
{
	return B9290_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9290 - report this TU's graph batch number.
 *
 * Always returns 9290.
 */
uint32_t
gj_batch_id_9290(void)
{
	(void)NULL;
	return b9290_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9290(void)
    __attribute__((alias("gj_batch_id_9290")));
