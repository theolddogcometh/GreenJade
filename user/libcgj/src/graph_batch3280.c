/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3280: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3280(void);
 *     - Returns the compile-time graph batch number for this TU (3280).
 *   uint32_t __gj_batch_id_3280  (alias)
 *   __libcgj_batch3280_marker = "libcgj-batch3280"
 *
 * Exclusive continuum CREATE-ONLY (3271-3280): ascii_isupper_u,
 * ascii_islower_u, ascii_iscntrl_u, ascii_ispunct_u, ascii_isgraph_u,
 * ascii_isblank_u, ascii_tohex_u, ascii_fromhex_u, ascii_casecmp_u,
 * batch_id_3280. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3280_marker[] = "libcgj-batch3280";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3280_id(void)
{
	return 3280u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3280 - report this TU's graph batch number.
 *
 * Always returns 3280.
 */
uint32_t
gj_batch_id_3280(void)
{
	(void)NULL;
	return b3280_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3280(void)
    __attribute__((alias("gj_batch_id_3280")));
