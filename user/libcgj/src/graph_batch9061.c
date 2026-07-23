/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9061: soft Cairo ARGB32 format catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cairo_format_argb32_id_9061(void);
 *     - Returns 0 (soft CAIRO_FORMAT_ARGB32 catalog id). Integer
 *       product tag only; does not create surfaces or call libcairo.
 *   uint32_t __gj_cairo_format_argb32_id_9061  (alias)
 *   __libcgj_batch9061_marker = "libcgj-batch9061"
 *
 * Exclusive continuum CREATE-ONLY (9061-9070: cairo soft id
 * stubs — cairo_format_argb32_id_9061, cairo_format_rgb24_id_9062,
 * cairo_format_a8_id_9063, cairo_op_over_id_9064,
 * cairo_op_source_id_9065, cairo_antialias_ok_u_9066,
 * cairo_surface_ok_u_9067, cairo_path_ok_u_9068, cairo_ready_u_9069,
 * batch_id_9070). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9061_marker[] = "libcgj-batch9061";

/* Soft Cairo ARGB32 format catalog id (CAIRO_FORMAT_ARGB32 = 0). */
#define B9061_CAIRO_FORMAT_ARGB32_ID  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9061_fmt_id(void)
{
	return B9061_CAIRO_FORMAT_ARGB32_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cairo_format_argb32_id_9061 - soft Cairo ARGB32 format catalog id.
 *
 * Always returns 0. Soft pure-data only; does not allocate image
 * surfaces or call libcairo. No parent wires.
 */
uint32_t
gj_cairo_format_argb32_id_9061(void)
{
	(void)NULL;
	return b9061_fmt_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cairo_format_argb32_id_9061(void)
    __attribute__((alias("gj_cairo_format_argb32_id_9061")));
