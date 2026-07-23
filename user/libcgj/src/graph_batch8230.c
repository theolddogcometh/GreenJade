/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8230: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8230(void);
 *     - Returns the compile-time graph batch number for this TU (8230).
 *   uint32_t __gj_batch_id_8230  (alias)
 *   __libcgj_batch8230_marker = "libcgj-batch8230"
 *
 * Exclusive continuum CREATE-ONLY (8221-8230: string length / bound
 * helpers — strlen_cap, strnlen, strlen_empty, strlen_nonzero,
 * str_bound_ok, str_need_nul, str_fits, str_overflow, str_underflow,
 * batch_id_8230). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8230_marker[] = "libcgj-batch8230";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8230_id(void)
{
	return 8230u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8230 - report this TU's graph batch number.
 *
 * Always returns 8230. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8230(void)
{
	(void)NULL;
	return b8230_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8230(void)
    __attribute__((alias("gj_batch_id_8230")));
