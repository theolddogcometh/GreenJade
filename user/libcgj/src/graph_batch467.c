/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch467: freestanding UUID nil helpers.
 *
 * Surface (unique symbols):
 *   int  gj_uuid_is_nil(const uint8_t u[16]);
 *     — Return 1 if all 16 bytes are zero (nil UUID). Return 0 for
 *       non-nil or NULL.
 *   void gj_uuid_nil(uint8_t u[16]);
 *     — Write the nil UUID (all zero) into u. No-op if u is NULL.
 *   int  __gj_uuid_is_nil  (alias)
 *   void __gj_uuid_nil     (alias)
 *   __libcgj_batch467_marker = "libcgj-batch467"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch467_marker[] = "libcgj-batch467";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_is_nil — 1 iff u[0..15] are all zero.
 *
 * Edge cases:
 *   u == NULL → 0
 *   all-zero  → 1
 *   any nonzero byte → 0
 */
int
gj_uuid_is_nil(const uint8_t u[16])
{
	size_t i;

	if (u == NULL) {
		return 0;
	}
	for (i = 0u; i < 16u; i++) {
		if (u[i] != 0u) {
			return 0;
		}
	}
	return 1;
}

/*
 * gj_uuid_nil — store the nil UUID (16 zero bytes) at u.
 *
 * Edge cases:
 *   u == NULL → no-op
 */
void
gj_uuid_nil(uint8_t u[16])
{
	size_t i;

	if (u == NULL) {
		return;
	}
	for (i = 0u; i < 16u; i++) {
		u[i] = 0u;
	}
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_uuid_is_nil(const uint8_t u[16])
    __attribute__((alias("gj_uuid_is_nil")));

void __gj_uuid_nil(uint8_t u[16])
    __attribute__((alias("gj_uuid_nil")));
