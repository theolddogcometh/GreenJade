/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2572: freestanding UUID version extract (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_uuid_version_u(const unsigned char u[16]);
 *     - Return the 4-bit version field (high nibble of octet 6) as
 *       0..15. u == NULL returns 0.
 *   unsigned __gj_uuid_version_u  (alias)
 *   __libcgj_batch2572_marker = "libcgj-batch2572"
 *
 * UUID exclusive wave (2571-2580). Distinct from gj_uuid_version
 * (batch163, NULL -> -1), gj_uuid_ver_u (batch745), and
 * gj_uuid_version_nibble (batch2097) — unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2572_marker[] = "libcgj-batch2572";

/* ---- freestanding helpers ---------------------------------------------- */

/* High nibble of octet 6. pU must be non-NULL. */
static unsigned
b2572_version(const unsigned char *pU)
{
	return (unsigned)((pU[6] >> 4) & 0x0fu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_version_u - high nibble of UUID octet 6 (version field).
 *
 * u: 16-octet UUID (NULL -> 0)
 *
 * Returns 0..15 on success, 0 if u is NULL. Does not call libc.
 */
unsigned
gj_uuid_version_u(const unsigned char pU[16])
{
	(void)NULL;
	if (pU == NULL) {
		return 0u;
	}
	return b2572_version(pU);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_uuid_version_u(const unsigned char pU[16])
    __attribute__((alias("gj_uuid_version_u")));
