/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8579: iconv conversion buffer size ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iconv_buf_ok_u_8579(uint32_t buf_size);
 *     - Return 1 if buf_size is in [1, 65536] (min useful iconv buf
 *       through soft max conversion chunk), else 0. Pure-data bound.
 *   uint32_t __gj_iconv_buf_ok_u_8579  (alias)
 *   __libcgj_batch8579_marker = "libcgj-batch8579"
 *
 * Exclusive continuum CREATE-ONLY (8571-8580: iconv charset id stubs —
 * utf8_id, ascii_id, latin1_id, utf16le_id, utf16be_id, same_ok,
 * open_ready, close_ready, buf_ok, batch_id_8580).
 * Unique gj_iconv_buf_ok_u_8579 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8579_marker[] = "libcgj-batch8579";

#define B8579_MIN_BUF  ((uint32_t)1u)
#define B8579_MAX_BUF  ((uint32_t)65536u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8579_buf_ok(uint32_t u32Buf)
{
	return (u32Buf >= B8579_MIN_BUF && u32Buf <= B8579_MAX_BUF) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iconv_buf_ok_u_8579 - 1 if iconv conversion buffer size is soft-ok.
 *
 * buf_size: proposed conversion input/output buffer length in bytes.
 * Soft bounds only; does not call iconv. No parent wires.
 */
uint32_t
gj_iconv_buf_ok_u_8579(uint32_t u32BufSize)
{
	(void)NULL;
	return b8579_buf_ok(u32BufSize);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iconv_buf_ok_u_8579(uint32_t u32BufSize)
    __attribute__((alias("gj_iconv_buf_ok_u_8579")));
