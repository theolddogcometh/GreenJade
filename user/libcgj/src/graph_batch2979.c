/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2979: PE/DOS MZ magic check (_u exclusive).
 *
 * Surface (unique symbols):
 *   unsigned gj_pe_is_mz_u(const void *buf, size_t len);
 *     - Return 1u if buf begins with DOS "MZ" (0x4D 0x5A) and len >= 2;
 *       else 0u. NULL / short → 0u.
 *   unsigned __gj_pe_is_mz_u  (alias)
 *   __libcgj_batch2979_marker = "libcgj-batch2979"
 *
 * UUID/ELF/PE exclusive continuum wave (2971-2980). Distinct from
 * gj_pe_is_mz (batch1144) / gj_pe_is_pe — unique gj_pe_is_mz_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2979_marker[] = "libcgj-batch2979";

/* ---- freestanding helpers ---------------------------------------------- */

/* IMAGE_DOS_HEADER.e_magic == "MZ". p non-NULL, cb >= 2. */
static unsigned
b2979_is_mz(const unsigned char *p)
{
	if (p[0] != (unsigned char)'M' || p[1] != (unsigned char)'Z') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_is_mz_u - 1u if first two octets are 'M' 'Z'.
 *
 * buf: image start (NULL → 0u)
 * len: byte length (need >= 2)
 */
unsigned
gj_pe_is_mz_u(const void *pBuf, size_t cb)
{
	(void)NULL;
	if (pBuf == NULL || cb < 2u) {
		return 0u;
	}
	return b2979_is_mz((const unsigned char *)pBuf);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_pe_is_mz_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_pe_is_mz_u")));
