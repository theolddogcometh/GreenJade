/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3180: MILESTONE 3180 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3180(void);
 *     - Returns the compile-time graph batch number for this TU (3180).
 *   uint32_t __gj_batch_id_3180  (alias)
 *   __libcgj_batch3180_marker = "libcgj-batch3180"
 *
 * MILESTONE 3180 for the exclusive continuum CREATE-ONLY wave
 * (batches 3171-3180: bitset32_test/set/clr_u, bitset64_test/set/clr/pop/
 * empty_p/full_p_u). Does NOT redefine gj_batch_id / prior batch_id_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3180_marker[] = "libcgj-batch3180";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3180_id(void)
{
	return 3180u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3180 - report this TU's graph batch number.
 *
 * Always returns 3180. No parent wires.
 */
uint32_t
gj_batch_id_3180(void)
{
	(void)NULL;
	return b3180_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3180(void)
    __attribute__((alias("gj_batch_id_3180")));
