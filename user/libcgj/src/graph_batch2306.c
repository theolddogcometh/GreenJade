/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2306: freestanding byte-buffer push one u8
 * (post-2300 buffer exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_buf_push_u8(uint8_t *p, size_t cap, size_t *plen, uint8_t v);
 *     - Append v at p[*plen] and advance *plen by 1 when room remains.
 *       Returns 1 on success, 0 on bad args or full buffer.
 *       NULL p, NULL plen, *plen > cap, or *plen == cap -> 0.
 *   int __gj_buf_push_u8  (alias)
 *   __libcgj_batch2306_marker = "libcgj-batch2306"
 *
 * Post-2300 buffer exclusive wave (2301-2310). Distinct from
 * gj_buf_append (batch431, multi-byte) - unique gj_buf_push_u8 surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2306_marker[] = "libcgj-batch2306";

/* ---- freestanding helpers ---------------------------------------------- */

/* Push one byte at the used end; 1 on success. */
static int
b2306_push(uint8_t *pBuf, size_t cbCap, size_t *pcbLen, uint8_t u8V)
{
	size_t cbUsed;

	if (pBuf == NULL || pcbLen == NULL) {
		return 0;
	}
	cbUsed = *pcbLen;
	if (cbUsed >= cbCap) {
		return 0;
	}
	pBuf[cbUsed] = u8V;
	*pcbLen = cbUsed + 1u;
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_push_u8 - append one byte onto a capacity-bounded buffer.
 *
 * p:    destination base (NULL -> fail)
 * cap:  total capacity of p in bytes
 * plen: in/out used length (NULL -> fail); *plen must be <= cap
 * v:    byte to append
 *
 * Returns 1 on success (byte stored, *plen advanced), else 0.
 * Does not call libc.
 */
int
gj_buf_push_u8(uint8_t *pBuf, size_t cbCap, size_t *pcbLen, uint8_t u8V)
{
	(void)NULL;
	return b2306_push(pBuf, cbCap, pcbLen, u8V);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_buf_push_u8(uint8_t *pBuf, size_t cbCap, size_t *pcbLen,
                     uint8_t u8V)
    __attribute__((alias("gj_buf_push_u8")));
