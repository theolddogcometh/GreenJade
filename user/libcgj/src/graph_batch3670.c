/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3670: batch identity for exclusive sketch
 * primitives continuum wave (3661-3670).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3670(void);
 *     - Returns the compile-time graph batch number for this TU (3670).
 *   uint32_t __gj_batch_id_3670  (alias)
 *   __libcgj_batch3670_marker = "libcgj-batch3670"
 *
 * Exclusive continuum CREATE-ONLY (3661-3670:
 * bloom_hash1_u, bloom_hash2_u, bloom_test_bit_u, bloom_set_bit_u,
 * hll_register_u, hll_estimate_u, cms_index_u, cms_inc_u,
 * topk_candidate_u, batch_id_3670). Distinct from prior batch_id_*
 * symbols — unique surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3670_marker[] = "libcgj-batch3670";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3670_id(void)
{
	return 3670u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3670 - report this TU's graph batch number.
 *
 * Always returns 3670. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3670(void)
{
	(void)NULL;
	return b3670_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3670(void)
    __attribute__((alias("gj_batch_id_3670")));
