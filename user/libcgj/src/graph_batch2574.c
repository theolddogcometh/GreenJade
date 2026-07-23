/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2574: freestanding UUID version setter (_u).
 *
 * Surface (unique symbols):
 *   void gj_uuid_set_version_u(unsigned char u[16], unsigned uVer);
 *     - In-place: set the 4-bit version field (high nibble of octet 6)
 *       to (uVer & 0x0f). Low nibble of octet 6 is preserved.
 *       NULL u -> no-op. uVer is masked to 4 bits.
 *   void __gj_uuid_set_version_u  (alias)
 *   __libcgj_batch2574_marker = "libcgj-batch2574"
 *
 * UUID exclusive wave (2571-2580). Distinct from gj_uuid_set_v4_bits
 * (batch628) and gj_uuid_v7_set_timestamp (batch163) — generic version
 * stamp only; unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2574_marker[] = "libcgj-batch2574";

/* ---- freestanding helpers ---------------------------------------------- */

/* Stamp version high nibble on octet 6. pU must be non-NULL. */
static void
b2574_set_version(unsigned char *pU, unsigned uVer)
{
	unsigned char u8Lo;

	u8Lo = (unsigned char)(pU[6] & 0x0fu);
	pU[6] = (unsigned char)(u8Lo | ((unsigned char)(uVer & 0x0fu) << 4));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_set_version_u - set UUID version nibble in place.
 *
 * u:    16-octet UUID (NULL -> no-op)
 * uVer: version value; only low 4 bits are used (0..15)
 *
 * Preserves the low nibble of octet 6 (clock_seq / rand_a high).
 * Does not call libc.
 */
void
gj_uuid_set_version_u(unsigned char pU[16], unsigned uVer)
{
	(void)NULL;
	if (pU == NULL) {
		return;
	}
	b2574_set_version(pU, uVer);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_uuid_set_version_u(unsigned char pU[16], unsigned uVer)
    __attribute__((alias("gj_uuid_set_version_u")));
