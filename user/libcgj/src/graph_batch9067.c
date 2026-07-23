/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9067: soft Cairo surface-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cairo_surface_ok_u_9067(void);
 *     - Returns 1 (Cairo surface soft-id continuum ok). Pure-data
 *       product tag; does not create or status-check cairo surfaces.
 *   uint32_t __gj_cairo_surface_ok_u_9067  (alias)
 *   __libcgj_batch9067_marker = "libcgj-batch9067"
 *
 * Exclusive continuum CREATE-ONLY (9061-9070: cairo soft id
 * stubs — cairo_format_argb32_id_9061, cairo_format_rgb24_id_9062,
 * cairo_format_a8_id_9063, cairo_op_over_id_9064,
 * cairo_op_source_id_9065, cairo_antialias_ok_u_9066,
 * cairo_surface_ok_u_9067, cairo_path_ok_u_9068, cairo_ready_u_9069,
 * batch_id_9070). Unique surface only; no multi-def. Distinct from
 * gj_cairo_antialias_ok_u_9066. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9067_marker[] = "libcgj-batch9067";

/* Cairo surface soft-ok lamp for continuum 9061-9070. */
#define B9067_CAIRO_SURFACE_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9067_surf_ok(void)
{
	return B9067_CAIRO_SURFACE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cairo_surface_ok_u_9067 - soft Cairo surface-ok unit flag.
 *
 * Always returns 1. Soft pure-data only; does not create image/PDF
 * surfaces or call cairo_surface_status. No parent wires.
 */
uint32_t
gj_cairo_surface_ok_u_9067(void)
{
	(void)NULL;
	return b9067_surf_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cairo_surface_ok_u_9067(void)
    __attribute__((alias("gj_cairo_surface_ok_u_9067")));
