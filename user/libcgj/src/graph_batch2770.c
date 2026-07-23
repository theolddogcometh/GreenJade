/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2770: milestone 2770 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2770(void);
 *     - Returns the compile-time graph batch number for this TU (2770).
 *   uint32_t __gj_batch_id_2770  (alias)
 *   __libcgj_batch2770_marker = "libcgj-batch2770"
 *
 * Milestone for the post-2760 str/mem/int exclusive wave (batches
 * 2761-2769: str_cspn_n_u, str_pbrk_n_u, mem_find_u8_u, mem_count_u8_u,
 * u32_min3_u, u32_max3_u, u64_min3_u, u64_max3_u, i32_sign_u). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols -
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2770_marker[] = "libcgj-batch2770";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2770_id(void)
{
	return 2770u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2770 - report this TU's graph batch number.
 *
 * Always returns 2770.
 */
uint32_t
gj_batch_id_2770(void)
{
	(void)NULL;
	return b2770_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2770(void)
    __attribute__((alias("gj_batch_id_2770")));
