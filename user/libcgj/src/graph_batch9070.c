/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9070: cairo soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9070(void);
 *     - Returns the compile-time graph batch number for this TU (9070).
 *   uint32_t __gj_batch_id_9070  (alias)
 *   __libcgj_batch9070_marker = "libcgj-batch9070"
 *
 * Exclusive continuum CREATE-ONLY (9061-9070: cairo soft id
 * stubs — cairo_format_argb32_id_9061, cairo_format_rgb24_id_9062,
 * cairo_format_a8_id_9063, cairo_op_over_id_9064,
 * cairo_op_source_id_9065, cairo_antialias_ok_u_9066,
 * cairo_surface_ok_u_9067, cairo_path_ok_u_9068, cairo_ready_u_9069,
 * batch_id_9070). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9070_marker[] = "libcgj-batch9070";

#define B9070_BATCH_ID  9070u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9070_id(void)
{
	return B9070_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9070 - report this TU's graph batch number.
 *
 * Always returns 9070.
 */
uint32_t
gj_batch_id_9070(void)
{
	(void)NULL;
	return b9070_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9070(void)
    __attribute__((alias("gj_batch_id_9070")));
