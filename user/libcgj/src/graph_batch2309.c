/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2309: freestanding byte-buffer length reset
 * (post-2300 buffer exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_buf_reset(size_t *plen);
 *     - Set *plen = 0 (clear used length). Does not touch storage.
 *       NULL plen -> no-op.
 *   void __gj_buf_reset  (alias)
 *   __libcgj_batch2309_marker = "libcgj-batch2309"
 *
 * Post-2300 buffer exclusive wave (2301-2310). Distinct from
 * gj_sb_init (batch351, string-builder watermark) - unique
 * gj_buf_reset surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2309_marker[] = "libcgj-batch2309";

/* ---- freestanding helpers ---------------------------------------------- */

/* Zero the used-length watermark. */
static void
b2309_reset(size_t *pcbLen)
{
	if (pcbLen == NULL) {
		return;
	}
	*pcbLen = 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_reset - clear the used-length watermark of a byte buffer.
 *
 * plen: in/out used length (NULL -> no-op)
 *
 * Sets *plen = 0. Does not zero or free storage. Does not call libc.
 */
void
gj_buf_reset(size_t *pcbLen)
{
	(void)NULL;
	b2309_reset(pcbLen);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_buf_reset(size_t *pcbLen)
    __attribute__((alias("gj_buf_reset")));
