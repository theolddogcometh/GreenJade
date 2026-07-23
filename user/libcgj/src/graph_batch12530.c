/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12530: font soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12530(void);
 *     - Returns the compile-time graph batch number for this TU (12530).
 *   uint32_t __gj_batch_id_12530  (alias)
 *   __libcgj_batch12530_marker = "libcgj-batch12530"
 *
 * Exclusive continuum CREATE-ONLY (12521-12530: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_12521, freetype_ok_u_12522,
 * harfbuzz_ok_u_12523, cairo_ok_u_12524, pango_ok_u_12525,
 * fribidi_ok_u_12526, graphite_ok_u_12527, font_match_ok_u_12528,
 * font_soft_ready_u_12529, batch_id_12530).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12530_marker[] = "libcgj-batch12530";

#define B12530_BATCH_ID  12530u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12530_id(void)
{
	return B12530_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12530 - report this TU's graph batch number.
 *
 * Always returns 12530.
 */
uint32_t
gj_batch_id_12530(void)
{
	(void)NULL;
	return b12530_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12530(void)
    __attribute__((alias("gj_batch_id_12530")));
