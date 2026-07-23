/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10430: font soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10430(void);
 *     - Returns the compile-time graph batch number for this TU (10430).
 *   uint32_t __gj_batch_id_10430  (alias)
 *   __libcgj_batch10430_marker = "libcgj-batch10430"
 *
 * Exclusive continuum CREATE-ONLY (10421-10430: font soft
 * id stubs — fontconfig_ok_u_10421, freetype_ok_u_10422,
 * harfbuzz_ok_u_10423, cairo_ok_u_10424, pango_ok_u_10425,
 * fribidi_ok_u_10426, graphite_ok_u_10427, font_match_ok_u_10428,
 * font_soft_ready_u_10429, batch_id_10430). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10430_marker[] = "libcgj-batch10430";

#define B10430_BATCH_ID  10430u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10430_id(void)
{
	return B10430_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10430 - report this TU's graph batch number.
 *
 * Always returns 10430.
 */
uint32_t
gj_batch_id_10430(void)
{
	(void)NULL;
	return b10430_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10430(void)
    __attribute__((alias("gj_batch_id_10430")));
