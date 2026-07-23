/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3420: exclusive wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3420(void);
 *     - Returns the compile-time graph batch number for this TU (3420).
 *   uint32_t __gj_batch_id_3420  (alias)
 *   __libcgj_batch3420_marker = "libcgj-batch3420"
 *
 * Exclusive continuum CREATE-ONLY (3411-3420): u64_blsmsk_u, u32_tzmsk_u,
 * u64_tzmsk_u, u32_andn_u, u64_andn_u, u32_bextr_u, u64_bextr_u,
 * u32_pdep_soft_u, u64_pdep_soft_u, batch_id_3420. Distinct from
 * gj_batch_id_3400 (batch3400) and other batch_id_* symbols — unique
 * surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3420_marker[] = "libcgj-batch3420";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3420_id(void)
{
	return 3420u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3420 - report this TU's graph batch number.
 *
 * Always returns 3420. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3420(void)
{
	(void)NULL;
	return b3420_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3420(void)
    __attribute__((alias("gj_batch_id_3420")));
