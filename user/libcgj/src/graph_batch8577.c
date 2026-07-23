/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8577: iconv open-ready pair gate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iconv_open_ready_u_8577(uint32_t from, uint32_t to);
 *     - Return 1 if both charset ids are non-zero (known), else 0.
 *   uint32_t __gj_iconv_open_ready_u_8577  (alias)
 *   __libcgj_batch8577_marker = "libcgj-batch8577"
 *
 * Exclusive continuum CREATE-ONLY (8571-8580: iconv charset id stubs —
 * utf8_id, ascii_id, latin1_id, utf16le_id, utf16be_id, same_ok,
 * open_ready, close_ready, buf_ok, batch_id_8580).
 * Unique gj_iconv_open_ready_u_8577 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8577_marker[] = "libcgj-batch8577";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8577_open_ready(uint32_t u32From, uint32_t u32To)
{
	if (u32From == 0u)
		return 0u;
	if (u32To == 0u)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iconv_open_ready_u_8577 - 1 if to/from charset ids are both known.
 *
 * from: soft source charset id
 * to:   soft destination charset id
 *
 * Returns 1 when both are non-zero, else 0. Soft iconv_open request
 * gate; no parent wires. Does not call iconv_open.
 */
uint32_t
gj_iconv_open_ready_u_8577(uint32_t u32From, uint32_t u32To)
{
	(void)NULL;
	return b8577_open_ready(u32From, u32To);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iconv_open_ready_u_8577(uint32_t u32From, uint32_t u32To)
    __attribute__((alias("gj_iconv_open_ready_u_8577")));
