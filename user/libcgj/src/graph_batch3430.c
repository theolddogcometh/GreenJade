/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3430: exclusive wave batch identity (closes
 * bitops exclusive CREATE-ONLY wave 3421-3430).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3430(void);
 *     - Returns the compile-time graph batch number for this TU (3430).
 *   uint32_t __gj_batch_id_3430  (alias)
 *   __libcgj_batch3430_marker = "libcgj-batch3430"
 *
 * Closes the bitops exclusive wave (batches 3421-3429:
 * u32_pext_soft_u, u64_pext_soft_u, u32_lzcnt_u, u64_lzcnt_u,
 * u32_tzcnt_u, u64_tzcnt_u, u32_popcnt_u, u64_popcnt_u, u32_parity_u).
 * Does NOT redefine gj_batch_id / prior batch_id_* symbols — avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3430_marker[] = "libcgj-batch3430";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3430_id(void)
{
	return 3430u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3430 - report this TU's graph batch number.
 *
 * Always returns 3430. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3430(void)
{
	(void)NULL;
	return b3430_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3430(void)
    __attribute__((alias("gj_batch_id_3430")));
