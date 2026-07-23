/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3320: MILESTONE 3320 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3320(void);
 *     — Returns the compile-time graph batch number for this TU (3320).
 *   uint32_t __gj_batch_id_3320  (alias)
 *   __libcgj_batch3320_marker = "libcgj-batch3320"
 *
 * Milestone for the exclusive number-theory CREATE-ONLY wave (batches
 * 3311–3319: u64_pow_u, u32_powmod_u, u64_powmod_u2, u32_egcd_u,
 * u32_crt_u, u32_jacobi_u, u32_legendre_u, u32_miller_rabin_u,
 * u32_random_prime_u). Does NOT redefine gj_batch_id / prior batch_id_*
 * or milestone_* symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3320_marker[] = "libcgj-batch3320";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3320_id(void)
{
	return 3320u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3320 — report this TU's graph batch number.
 *
 * Always returns 3320. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_3320(void)
{
	(void)NULL;
	return b3320_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3320(void)
    __attribute__((alias("gj_batch_id_3320")));
