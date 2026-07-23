/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2943: uint32 modular exponentiation (_u surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pow_mod_u32_u(uint32_t base, uint32_t exp, uint32_t mod);
 *     — (base^exp) mod mod via square-and-multiply.
 *       mod==0 → 0. mod==1 → 0. exp==0 → 1 when mod > 1.
 *   uint32_t __gj_pow_mod_u32_u  (alias)
 *   __libcgj_batch2943_marker = "libcgj-batch2943"
 *
 * CREATE-ONLY exclusive continuum wave (2941–2950). Distinct from
 * gj_pow_mod_u32 (batch842) and gj_u64_powmod_u (batch2505) — unique
 * gj_pow_mod_u32_u name; no parent wires, no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2943_marker[] = "libcgj-batch2943";

/* ---- freestanding helpers ---------------------------------------------- */

/* Local binary (a*b) mod m; no external mulmod wire. */
static uint32_t
b2943_mulmod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	uint32_t u32R;

	u32A %= u32M;
	u32B %= u32M;
	u32R = 0u;
	while (u32B != 0u) {
		if ((u32B & 1u) != 0u) {
			uint32_t u32S = u32M - u32R;

			u32R = (u32A >= u32S) ? (u32A - u32S) : (u32R + u32A);
		}
		{
			uint32_t u32S = u32M - u32A;

			u32A = (u32A >= u32S) ? (u32A - u32S) : (u32A + u32A);
		}
		u32B >>= 1;
	}
	return u32R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pow_mod_u32_u — (base^exp) mod mod via square-and-multiply.
 *
 * Special cases:
 *   mod == 0  → 0 (invalid modulus)
 *   mod == 1  → 0
 *   exp == 0  → 1 when mod > 1
 */
uint32_t
gj_pow_mod_u32_u(uint32_t u32Base, uint32_t u32Exp, uint32_t u32Mod)
{
	uint32_t u32Result;

	(void)NULL;
	if (u32Mod == 0u || u32Mod == 1u) {
		return 0u;
	}

	u32Result = 1u;
	u32Base %= u32Mod;
	while (u32Exp != 0u) {
		if ((u32Exp & 1u) != 0u) {
			u32Result = b2943_mulmod(u32Result, u32Base, u32Mod);
		}
		u32Base = b2943_mulmod(u32Base, u32Base, u32Mod);
		u32Exp >>= 1;
	}
	return u32Result;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pow_mod_u32_u(uint32_t u32Base, uint32_t u32Exp, uint32_t u32Mod)
    __attribute__((alias("gj_pow_mod_u32_u")));
