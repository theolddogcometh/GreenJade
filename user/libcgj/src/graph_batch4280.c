/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4280: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4280(void);
 *     - Returns the compile-time graph batch number for this TU (4280).
 *   uint32_t __gj_batch_id_4280  (alias)
 *   __libcgj_batch4280_marker = "libcgj-batch4280"
 *
 * Exclusive continuum CREATE-ONLY (4271-4280): u32_add_overflow_u,
 * u32_sub_overflow_u, u32_mul_overflow_u, u64_add_overflow_u,
 * u64_sub_overflow_u, u64_mul_overflow_u, u32_add_sat_flag_u,
 * u32_sub_sat_flag_u, size_add_ok_u, batch_id_4280.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4280_marker[] = "libcgj-batch4280";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4280_id(void)
{
	return 4280u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4280 - report this TU's graph batch number.
 *
 * Always returns 4280.
 */
uint32_t
gj_batch_id_4280(void)
{
	(void)NULL;
	return b4280_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4280(void)
    __attribute__((alias("gj_batch_id_4280")));
