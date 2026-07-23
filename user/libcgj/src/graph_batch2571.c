/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2571: freestanding UUID nil predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_uuid_is_nil_u(const unsigned char u[16]);
 *     - Return 1 if all 16 octets are zero (nil UUID). Return 0 for
 *       non-nil or NULL.
 *   int __gj_uuid_is_nil_u  (alias)
 *   __libcgj_batch2571_marker = "libcgj-batch2571"
 *
 * UUID exclusive wave (2571-2580). Distinct from gj_uuid_is_nil
 * (batch467) and gj_uuid_nil_p (batch629) — unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2571_marker[] = "libcgj-batch2571";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff all 16 octets are zero. pU must be non-NULL. */
static int
b2571_is_nil(const unsigned char *pU)
{
	unsigned i;

	for (i = 0u; i < 16u; i++) {
		if (pU[i] != 0u) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_is_nil_u - 1 iff u[0..15] are all zero.
 *
 * u: 16-octet UUID (NULL -> 0)
 *
 * Returns 1 for the nil UUID, else 0. Does not call libc.
 */
int
gj_uuid_is_nil_u(const unsigned char pU[16])
{
	(void)NULL;
	if (pU == NULL) {
		return 0;
	}
	return b2571_is_nil(pU);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uuid_is_nil_u(const unsigned char pU[16])
    __attribute__((alias("gj_uuid_is_nil_u")));
