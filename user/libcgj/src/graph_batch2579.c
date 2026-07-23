/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2579: freestanding UUID export to raw bytes (_u).
 *
 * Surface (unique symbols):
 *   void gj_uuid_to_bytes_u(unsigned char bytes[16],
 *                           const unsigned char u[16]);
 *     - Export 16 octets from u into bytes (network / sequential
 *       layout). Either NULL -> no-op. Same pointer -> no-op.
 *   void __gj_uuid_to_bytes_u  (alias)
 *   __libcgj_batch2579_marker = "libcgj-batch2579"
 *
 * UUID exclusive wave (2571-2580). Inverse of gj_uuid_from_bytes_u
 * (batch2578); distinct from gj_uuid_copy_u (batch2577) by API role
 * (uuid -> bytes); unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2579_marker[] = "libcgj-batch2579";

/* ---- freestanding helpers ---------------------------------------------- */

/* Export 16 UUID octets to raw buffer. pBytes and pU must be non-NULL. */
static void
b2579_to_bytes(unsigned char *pBytes, const unsigned char *pU)
{
	unsigned i;

	for (i = 0u; i < 16u; i++) {
		pBytes[i] = pU[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_to_bytes_u - export a UUID value into a 16-byte buffer.
 *
 * bytes: destination raw octets, sequential/network order (NULL -> no-op)
 * u:     source 16-octet UUID (NULL -> no-op)
 *
 * Does not call libc. No formatting — pure octet transfer.
 */
void
gj_uuid_to_bytes_u(unsigned char pBytes[16], const unsigned char pU[16])
{
	(void)NULL;
	if (pBytes == NULL || pU == NULL) {
		return;
	}
	if (pBytes == pU) {
		return;
	}
	b2579_to_bytes(pBytes, pU);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_uuid_to_bytes_u(unsigned char pBytes[16],
    const unsigned char pU[16])
    __attribute__((alias("gj_uuid_to_bytes_u")));
