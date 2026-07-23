/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8571: iconv charset UTF-8 id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iconv_utf8_id_8571(void);
 *     - Return soft charset id for UTF-8 (1).
 *   uint32_t __gj_iconv_utf8_id_8571  (alias)
 *   __libcgj_batch8571_marker = "libcgj-batch8571"
 *
 * Exclusive continuum CREATE-ONLY (8571-8580: iconv charset id stubs —
 * utf8_id, ascii_id, latin1_id, utf16le_id, utf16be_id, same_ok,
 * open_ready, close_ready, buf_ok, batch_id_8580).
 * Unique gj_iconv_utf8_id_8571 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8571_marker[] = "libcgj-batch8571";

/* Soft charset id matching local iconv UTF-8 codeset. */
#define B8571_CS_UTF8  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8571_utf8_id(void)
{
	return B8571_CS_UTF8;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iconv_utf8_id_8571 - soft UTF-8 charset id.
 *
 * Always returns 1. Pure constant tag; no parent wires. Does not call
 * iconv or classify string names.
 */
uint32_t
gj_iconv_utf8_id_8571(void)
{
	(void)NULL;
	return b8571_utf8_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iconv_utf8_id_8571(void)
    __attribute__((alias("gj_iconv_utf8_id_8571")));
