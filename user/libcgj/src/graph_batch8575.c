/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8575: iconv charset UTF-16BE id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iconv_utf16be_id_8575(void);
 *     - Return soft charset id for UTF-16BE (5).
 *   uint32_t __gj_iconv_utf16be_id_8575  (alias)
 *   __libcgj_batch8575_marker = "libcgj-batch8575"
 *
 * Exclusive continuum CREATE-ONLY (8571-8580: iconv charset id stubs —
 * utf8_id, ascii_id, latin1_id, utf16le_id, utf16be_id, same_ok,
 * open_ready, close_ready, buf_ok, batch_id_8580).
 * Unique gj_iconv_utf16be_id_8575 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8575_marker[] = "libcgj-batch8575";

/* Soft charset id matching local iconv UTF-16BE codeset. */
#define B8575_CS_UTF16BE  ((uint32_t)5u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8575_utf16be_id(void)
{
	return B8575_CS_UTF16BE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iconv_utf16be_id_8575 - soft UTF-16BE charset id.
 *
 * Always returns 5. Pure constant tag; no parent wires. Does not call
 * iconv or classify string names.
 */
uint32_t
gj_iconv_utf16be_id_8575(void)
{
	(void)NULL;
	return b8575_utf16be_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iconv_utf16be_id_8575(void)
    __attribute__((alias("gj_iconv_utf16be_id_8575")));
