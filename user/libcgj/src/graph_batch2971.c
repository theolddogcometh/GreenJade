/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2971: UUID nil predicate (_u exclusive).
 *
 * Surface (unique symbols):
 *   unsigned gj_uuid_nil_p_u(const unsigned char u[16]);
 *     - Return 1u if all 16 octets are zero; else 0u. NULL → 0u.
 *   unsigned __gj_uuid_nil_p_u  (alias)
 *   __libcgj_batch2971_marker = "libcgj-batch2971"
 *
 * UUID/ELF/PE exclusive continuum wave (2971-2980). Distinct from
 * gj_uuid_nil_p (batch629) / gj_uuid_is_nil (batch467) — unique
 * gj_uuid_nil_p_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2971_marker[] = "libcgj-batch2971";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if all 16 octets are zero. pU non-NULL. */
static unsigned
b2971_nil_p(const unsigned char *pU)
{
	size_t i;

	for (i = 0u; i < 16u; i++) {
		if (pU[i] != 0u) {
			return 0u;
		}
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_nil_p_u - 1u if uuid is the nil UUID (all zero octets).
 *
 * u: pointer to 16 UUID octets (NULL → 0u)
 *
 * Returns 1u when every octet is 0; else 0u.
 */
unsigned
gj_uuid_nil_p_u(const unsigned char u[16])
{
	(void)NULL;
	if (u == NULL) {
		return 0u;
	}
	return b2971_nil_p(u);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_uuid_nil_p_u(const unsigned char u[16])
    __attribute__((alias("gj_uuid_nil_p_u")));
