/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch431: freestanding bounded byte-buffer append.
 *
 * Surface (unique symbols):
 *   size_t gj_buf_append(uint8_t *dst, size_t dcap, size_t *dlen,
 *                        const uint8_t *src, size_t slen);
 *     — Append slen bytes from src onto dst[*dlen..). On success, advances
 *       *dlen by the number of bytes written and returns that count (slen).
 *       If there is not enough remaining capacity (dcap - *dlen < slen), or
 *       any pointer argument required for a non-empty write is NULL, returns
 *       0 and leaves *dlen unchanged. slen == 0 is a successful no-op and
 *       returns 0 only when a required pointer is missing; with valid dlen
 *       it returns 0 (zero bytes written) as a successful empty append when
 *       dst/src may be NULL.
 *   size_t __gj_buf_append  (alias)
 *   __libcgj_batch431_marker = "libcgj-batch431"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch431_marker[] = "libcgj-batch431";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_append — append src[0..slen) to the used region of a capacity-bounded
 * destination buffer tracked by *dlen.
 *
 * dst:  destination base (may be NULL only when slen == 0)
 * dcap: total capacity of dst in bytes
 * dlen: in/out used length; must be non-NULL; *dlen must be <= dcap
 * src:  source bytes (may be NULL only when slen == 0)
 * slen: number of source bytes to append
 *
 * Returns bytes written (slen on success), or 0 on failure / empty write.
 */
size_t
gj_buf_append(uint8_t *pDst, size_t cbDcap, size_t *pcbDlen,
              const uint8_t *pSrc, size_t cbSlen)
{
	size_t cbUsed;
	size_t cbRem;
	size_t i;

	if (pcbDlen == NULL) {
		return 0u;
	}

	cbUsed = *pcbDlen;
	if (cbUsed > cbDcap) {
		return 0u;
	}

	if (cbSlen == 0u) {
		return 0u;
	}

	if (pDst == NULL || pSrc == NULL) {
		return 0u;
	}

	cbRem = cbDcap - cbUsed;
	if (cbRem < cbSlen) {
		return 0u;
	}

	for (i = 0u; i < cbSlen; i++) {
		pDst[cbUsed + i] = pSrc[i];
	}

	*pcbDlen = cbUsed + cbSlen;
	return cbSlen;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_buf_append(uint8_t *pDst, size_t cbDcap, size_t *pcbDlen,
                       const uint8_t *pSrc, size_t cbSlen)
    __attribute__((alias("gj_buf_append")));
