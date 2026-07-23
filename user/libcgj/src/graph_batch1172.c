/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1172: OpenPGP ASCII armor BEGIN prefix check.
 *
 * Surface (unique symbols):
 *   int gj_gpg_is_armor_begin(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with "-----BEGIN PGP " (15 octets)
 *       and len >= 15; else 0 (FAIL).
 *   int __gj_gpg_is_armor_begin  (alias)
 *   __libcgj_batch1172_marker = "libcgj-batch1172"
 *
 * Distinct from gj_cert_is_pem_begin (batch1168) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1172_marker[] = "libcgj-batch1172";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpg_is_armor_begin — 1 if prefix is "-----BEGIN PGP ".
 */
int
gj_gpg_is_armor_begin(const void *pBuf, size_t cb)
{
	static const unsigned char aSig[15] = {
	    (unsigned char)'-', (unsigned char)'-', (unsigned char)'-',
	    (unsigned char)'-', (unsigned char)'-', (unsigned char)'B',
	    (unsigned char)'E', (unsigned char)'G', (unsigned char)'I',
	    (unsigned char)'N', (unsigned char)' ', (unsigned char)'P',
	    (unsigned char)'G', (unsigned char)'P', (unsigned char)' '
	};
	const unsigned char *p;
	unsigned i;

	if (pBuf == NULL || cb < 15u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	for (i = 0u; i < 15u; i++) {
		if (p[i] != aSig[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_gpg_is_armor_begin(const void *pBuf, size_t cb)
    __attribute__((alias("gj_gpg_is_armor_begin")));
