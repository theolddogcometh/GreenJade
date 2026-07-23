/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9068: soft Cairo path-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cairo_path_ok_u_9068(void);
 *     - Returns 1 (Cairo path soft-id continuum ok). Pure-data product
 *       tag; does not build paths or call libcairo path APIs.
 *   uint32_t __gj_cairo_path_ok_u_9068  (alias)
 *   __libcgj_batch9068_marker = "libcgj-batch9068"
 *
 * Exclusive continuum CREATE-ONLY (9061-9070: cairo soft id
 * stubs — cairo_format_argb32_id_9061, cairo_format_rgb24_id_9062,
 * cairo_format_a8_id_9063, cairo_op_over_id_9064,
 * cairo_op_source_id_9065, cairo_antialias_ok_u_9066,
 * cairo_surface_ok_u_9067, cairo_path_ok_u_9068, cairo_ready_u_9069,
 * batch_id_9070). Unique surface only; no multi-def. Distinct from
 * gj_cairo_surface_ok_u_9067. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9068_marker[] = "libcgj-batch9068";

/* Cairo path soft-ok lamp for continuum 9061-9070. */
#define B9068_CAIRO_PATH_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9068_path_ok(void)
{
	return B9068_CAIRO_PATH_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cairo_path_ok_u_9068 - soft Cairo path-ok unit flag.
 *
 * Always returns 1. Soft pure-data only; does not call move_to /
 * line_to / curve_to or copy_path. No parent wires.
 */
uint32_t
gj_cairo_path_ok_u_9068(void)
{
	(void)NULL;
	return b9068_path_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cairo_path_ok_u_9068(void)
    __attribute__((alias("gj_cairo_path_ok_u_9068")));
