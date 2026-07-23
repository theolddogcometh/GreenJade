/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2578: freestanding UUID load from raw bytes (_u).
 *
 * Surface (unique symbols):
 *   void gj_uuid_from_bytes_u(unsigned char u[16],
 *                             const unsigned char bytes[16]);
 *     - Load 16 raw octets from bytes into u (network / sequential
 *       layout). Either NULL -> no-op. Same pointer -> no-op.
 *   void __gj_uuid_from_bytes_u  (alias)
 *   __libcgj_batch2578_marker = "libcgj-batch2578"
 *
 * UUID exclusive wave (2571-2580). Distinct from gj_uuid_copy_u
 * (batch2577) by API role (bytes -> uuid); unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2578_marker[] = "libcgj-batch2578";

/* ---- freestanding helpers ---------------------------------------------- */

/* Load 16 raw octets into UUID. pU and pBytes must be non-NULL. */
static void
b2578_from_bytes(unsigned char *pU, const unsigned char *pBytes)
{
	unsigned i;

	for (i = 0u; i < 16u; i++) {
		pU[i] = pBytes[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_from_bytes_u - construct a UUID value from a 16-byte buffer.
 *
 * u:     destination 16-octet UUID (NULL -> no-op)
 * bytes: source raw octets, sequential/network order (NULL -> no-op)
 *
 * Does not call libc. No parsing — pure octet transfer.
 */
void
gj_uuid_from_bytes_u(unsigned char pU[16], const unsigned char pBytes[16])
{
	(void)NULL;
	if (pU == NULL || pBytes == NULL) {
		return;
	}
	if (pU == pBytes) {
		return;
	}
	b2578_from_bytes(pU, pBytes);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_uuid_from_bytes_u(unsigned char pU[16],
    const unsigned char pBytes[16])
    __attribute__((alias("gj_uuid_from_bytes_u")));
