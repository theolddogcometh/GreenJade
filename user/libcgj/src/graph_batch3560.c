/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3560: exclusive wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3560(void);
 *     - Returns the compile-time graph batch number for this TU (3560).
 *   uint32_t __gj_batch_id_3560  (alias)
 *   __libcgj_batch3560_marker = "libcgj-batch3560"
 *
 * Exclusive continuum CREATE-ONLY wave (3551-3560:
 * u32_rol_n_u, u64_rol_n_u, u32_ror_n_u, u64_ror_n_u,
 * u32_shl_safe_u, u64_shl_safe_u, u32_shr_safe_u, u64_shr_safe_u,
 * u32_sar_safe_u, batch_id_3560). Distinct from prior batch_id_*
 * symbols — unique surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3560_marker[] = "libcgj-batch3560";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3560_id(void)
{
	return 3560u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3560 - report this TU's graph batch number.
 *
 * Always returns 3560. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3560(void)
{
	(void)NULL;
	return b3560_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3560(void)
    __attribute__((alias("gj_batch_id_3560")));
