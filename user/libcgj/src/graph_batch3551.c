/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3551: uint32_t rotate left by n mod 32 (rol).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_rol_n_u(uint32_t x, unsigned n);
 *     - Rotate x left by (n % 32) bits. n == 0 or multiple of 32
 *       returns x unchanged (no shift UB). Pure integer; no builtins.
 *   uint32_t __gj_u32_rol_n_u  (alias)
 *   __libcgj_batch3551_marker = "libcgj-batch3551"
 *
 * Exclusive continuum CREATE-ONLY (3551-3560): u32_rol_n_u, u64_rol_n_u,
 * u32_ror_n_u, u64_ror_n_u, u32_shl_safe_u, u64_shl_safe_u,
 * u32_shr_safe_u, u64_shr_safe_u, u32_sar_safe_u, batch_id_3560.
 * Distinct from gj_u32_rotl_n_u (batch2403) — unique rol surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3551_marker[] = "libcgj-batch3551";

/* ---- freestanding helpers ---------------------------------------------- */

/* Rotate u32X left by (uN % 32). Zero mod returns u32X. */
static uint32_t
b3551_rol(uint32_t u32X, unsigned uN)
{
	unsigned uR;

	uR = uN % 32u;
	if (uR == 0u) {
		return u32X;
	}
	return (u32X << uR) | (u32X >> (32u - uR));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_rol_n_u - rotate a 32-bit word left by n mod 32.
 *
 * x: value to rotate
 * n: rotate count; reduced mod 32 (0 and multiples of 32 are no-ops)
 *
 * Returns the rotated value. Does not call libc. No parent wires.
 */
uint32_t
gj_u32_rol_n_u(uint32_t u32X, unsigned uN)
{
	(void)NULL;
	return b3551_rol(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_rol_n_u(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_rol_n_u")));
