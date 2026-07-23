/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4260: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4260(void);
 *     - Returns the compile-time graph batch number for this TU (4260).
 *   uint32_t __gj_batch_id_4260  (alias)
 *   __libcgj_batch4260_marker = "libcgj-batch4260"
 *
 * Exclusive continuum CREATE-ONLY (4251-4260): u32_pop_parity_u,
 * u64_pop_parity_u, u32_hamming_u, u64_hamming_u, u32_bit_deposit_low_u,
 * u32_bit_extract_low_u, u32_isolate_msb_u, u32_isolate_lsb_u,
 * u32_clear_lsb_u, batch_id_4260. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def. No
 * parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4260_marker[] = "libcgj-batch4260";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4260_id(void)
{
	return 4260u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4260 - report this TU's graph batch number.
 *
 * Always returns 4260. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_4260(void)
{
	(void)NULL;
	return b4260_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4260(void)
    __attribute__((alias("gj_batch_id_4260")));
