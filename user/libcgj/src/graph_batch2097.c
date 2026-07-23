/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2097: freestanding UUID version nibble extract.
 *
 * Surface (unique symbols):
 *   int gj_uuid_version_nibble(const unsigned char uuid[16]);
 *     - Return the 4-bit version field (high nibble of octet 6) as
 *       0..15. uuid == NULL returns -1.
 *   int __gj_uuid_version_nibble  (alias)
 *   __libcgj_batch2097_marker = "libcgj-batch2097"
 *
 * Post-2000 id exclusive wave (2091-2100). Distinct from
 * gj_uuid_version (batch163) - unique gj_uuid_version_nibble name;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2097_marker[] = "libcgj-batch2097";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2097_ver_nibble(const unsigned char *pU)
{
	return (int)((pU[6] >> 4) & 0x0fu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_version_nibble - high nibble of UUID octet 6 (version field).
 *
 * uuid: 16-octet UUID (NULL -> -1)
 *
 * Returns 0..15 on success, -1 if uuid is NULL. Does not call libc.
 */
int
gj_uuid_version_nibble(const unsigned char pUuid[16])
{
	(void)NULL;
	if (pUuid == NULL) {
		return -1;
	}
	return b2097_ver_nibble(pUuid);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uuid_version_nibble(const unsigned char pUuid[16])
    __attribute__((alias("gj_uuid_version_nibble")));
