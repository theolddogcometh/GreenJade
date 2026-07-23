/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8574: iconv charset UTF-16LE id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iconv_utf16le_id_8574(void);
 *     - Return soft charset id for UTF-16LE (4).
 *   uint32_t __gj_iconv_utf16le_id_8574  (alias)
 *   __libcgj_batch8574_marker = "libcgj-batch8574"
 *
 * Exclusive continuum CREATE-ONLY (8571-8580: iconv charset id stubs —
 * utf8_id, ascii_id, latin1_id, utf16le_id, utf16be_id, same_ok,
 * open_ready, close_ready, buf_ok, batch_id_8580).
 * Unique gj_iconv_utf16le_id_8574 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8574_marker[] = "libcgj-batch8574";

/* Soft charset id matching local iconv UTF-16LE codeset. */
#define B8574_CS_UTF16LE  ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8574_utf16le_id(void)
{
	return B8574_CS_UTF16LE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iconv_utf16le_id_8574 - soft UTF-16LE charset id.
 *
 * Always returns 4. Pure constant tag; no parent wires. Does not call
 * iconv or classify string names.
 */
uint32_t
gj_iconv_utf16le_id_8574(void)
{
	(void)NULL;
	return b8574_utf16le_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iconv_utf16le_id_8574(void)
    __attribute__((alias("gj_iconv_utf16le_id_8574")));
