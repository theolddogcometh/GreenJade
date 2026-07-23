/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2972: UUID version-4 field predicate (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_uuid_v4_version_p_u(const unsigned char u[16]);
 *     - Return 1u if the version nibble (high 4 bits of octet 6) is 4
 *       (RFC 4122 / 9562 UUID version 4); else 0u. NULL → 0u.
 *   unsigned __gj_uuid_v4_version_p_u  (alias)
 *   __libcgj_batch2972_marker = "libcgj-batch2972"
 *
 * UUID/ELF/PE exclusive continuum wave (2971-2980). Distinct from
 * gj_uuid_version (batch163) / gj_uuid_ver_u (batch745) — unique
 * gj_uuid_v4_version_p_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2972_marker[] = "libcgj-batch2972";

/* ---- freestanding helpers ---------------------------------------------- */

/* High nibble of octet 6 is the version field. pU non-NULL. */
static unsigned
b2972_ver4_p(const unsigned char *pU)
{
	return (((unsigned)pU[6] >> 4) & 0x0fu) == 4u ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_v4_version_p_u - 1u if UUID version field is 4.
 *
 * u: pointer to 16 UUID octets (NULL → 0u)
 *
 * Only the version nibble is checked; variant bits are ignored.
 */
unsigned
gj_uuid_v4_version_p_u(const unsigned char u[16])
{
	(void)NULL;
	if (u == NULL) {
		return 0u;
	}
	return b2972_ver4_p(u);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_uuid_v4_version_p_u(const unsigned char u[16])
    __attribute__((alias("gj_uuid_v4_version_p_u")));
