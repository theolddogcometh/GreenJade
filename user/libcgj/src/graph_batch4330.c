/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4330: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4330(void);
 *     - Returns the compile-time graph batch number for this TU (4330).
 *   uint32_t __gj_batch_id_4330  (alias)
 *   __libcgj_batch4330_marker = "libcgj-batch4330"
 *
 * Exclusive continuum CREATE-ONLY (4321-4330: str_is_empty_u2,
 * str_eq_u2, str_neq_u2, str_startswith_u, str_endswith_u, str_ncmp_u2,
 * str_chr_idx_u, str_count_char_u2, str_hash_mix_u, batch_id_4330).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4330_marker[] = "libcgj-batch4330";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4330_id(void)
{
	return 4330u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4330 - report this TU's graph batch number.
 *
 * Always returns 4330.
 */
uint32_t
gj_batch_id_4330(void)
{
	(void)NULL;
	return b4330_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4330(void)
    __attribute__((alias("gj_batch_id_4330")));
