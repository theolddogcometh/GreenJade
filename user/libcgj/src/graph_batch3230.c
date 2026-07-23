/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3230: MILESTONE 3230 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3230(void);
 *     - Returns the compile-time graph batch number for this TU (3230).
 *   uint32_t __gj_batch_id_3230  (alias)
 *   __libcgj_batch3230_marker = "libcgj-batch3230"
 *
 * MILESTONE 3230 for the exclusive continuum CREATE-ONLY wave
 * (batches 3221-3229: u32_shl_u, u64_shl_u, u32_shr_u, u64_shr_u,
 * u32_sar_u, u64_sar_u, u32_shl_sat_u, u64_shl_sat_u,
 * u32_extract_bits_u). Does NOT redefine gj_batch_id / prior
 * batch_id_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3230_marker[] = "libcgj-batch3230";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3230_id(void)
{
return 3230u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3230 - report this TU's graph batch number.
 *
 * Always returns 3230.
 */
uint32_t
gj_batch_id_3230(void)
{
(void)NULL;
return b3230_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3230(void)
    __attribute__((alias("gj_batch_id_3230")));
