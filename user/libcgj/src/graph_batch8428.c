/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8428: fold 32-bit hash to 16 bits (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_fold16_u_8428(uint32_t x);
 *     - Fold x to 16 bits via hi^lo of the 32-bit word, then mask:
 *       (x ^ (x >> 16)) & 0xffff. Return value fits in 16 bits.
 *   uint32_t __gj_hash_fold16_u_8428  (alias)
 *   __libcgj_batch8428_marker = "libcgj-batch8428"
 *
 * Hash/fnv-ish integer stubs wave (8421-8430). Distinct from
 * gj_u64_hash_fold32_u (batch4168) — unique _u_8428 surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8428_marker[] = "libcgj-batch8428";

/* ---- freestanding helpers ---------------------------------------------- */

/* Fold 32→16 via xor of high and low halves, then mask. */
static uint32_t
b8428_fold16(uint32_t u32X)
{
	return (u32X ^ (u32X >> 16)) & 0xffffu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_fold16_u_8428 - fold a 32-bit hash into 16 bits.
 *
 * x: arbitrary 32-bit input (all values accepted, including 0)
 *
 * Returns a value in 0..65535. Stateless pure function.
 */
uint32_t
gj_hash_fold16_u_8428(uint32_t u32X)
{
	(void)NULL;
	return b8428_fold16(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_fold16_u_8428(uint32_t u32X)
    __attribute__((alias("gj_hash_fold16_u_8428")));
