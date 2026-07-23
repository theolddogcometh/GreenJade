/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3820: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3820(void);
 *     - Returns the compile-time graph batch number for this TU (3820).
 *   uint32_t __gj_batch_id_3820  (alias)
 *   __libcgj_batch3820_marker = "libcgj-batch3820"
 *
 * Exclusive continuum CREATE-ONLY (3811-3820): u64_is_zero_u,
 * u32_is_one_u, u64_is_one_u, u32_is_max_u, u64_is_max_u, u32_is_even_u,
 * u64_is_even_u, u32_is_odd_u, u64_is_odd_u, batch_id_3820. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3820_marker[] = "libcgj-batch3820";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3820_id(void)
{
	return 3820u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3820 - report this TU's graph batch number.
 *
 * Always returns 3820.
 */
uint32_t
gj_batch_id_3820(void)
{
	(void)NULL;
	return b3820_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3820(void)
    __attribute__((alias("gj_batch_id_3820")));
