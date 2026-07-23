/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8429: 32-bit hash equality predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_hash_eq_u_8429(uint32_t a, uint32_t b);
 *     - Return 1 if a equals b; else 0. Integer-only hash compare
 *       helper for hash tables / set membership stubs.
 *   int __gj_hash_eq_u_8429  (alias)
 *   __libcgj_batch8429_marker = "libcgj-batch8429"
 *
 * Hash/fnv-ish integer stubs wave (8421-8430). Unique _u_8429 surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8429_marker[] = "libcgj-batch8429";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32A equals u32B. */
static int
b8429_eq(uint32_t u32A, uint32_t u32B)
{
	if (u32A == u32B) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_eq_u_8429 - equality of two 32-bit hash values.
 *
 * a, b: hash digests (or any uint32_t keys)
 *
 * Returns 1 if equal, 0 otherwise. Stateless pure function.
 */
int
gj_hash_eq_u_8429(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8429_eq(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hash_eq_u_8429(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_hash_eq_u_8429")));
