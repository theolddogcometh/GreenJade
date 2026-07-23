/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8573: iconv charset Latin-1 id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iconv_latin1_id_8573(void);
 *     - Return soft charset id for ISO-8859-1 / Latin-1 (3).
 *   uint32_t __gj_iconv_latin1_id_8573  (alias)
 *   __libcgj_batch8573_marker = "libcgj-batch8573"
 *
 * Exclusive continuum CREATE-ONLY (8571-8580: iconv charset id stubs —
 * utf8_id, ascii_id, latin1_id, utf16le_id, utf16be_id, same_ok,
 * open_ready, close_ready, buf_ok, batch_id_8580).
 * Unique gj_iconv_latin1_id_8573 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8573_marker[] = "libcgj-batch8573";

/* Soft charset id matching local iconv Latin-1 codeset. */
#define B8573_CS_LATIN1  ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8573_latin1_id(void)
{
	return B8573_CS_LATIN1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iconv_latin1_id_8573 - soft Latin-1 charset id.
 *
 * Always returns 3. Pure constant tag; no parent wires. Does not call
 * iconv or classify string names.
 */
uint32_t
gj_iconv_latin1_id_8573(void)
{
	(void)NULL;
	return b8573_latin1_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iconv_latin1_id_8573(void)
    __attribute__((alias("gj_iconv_latin1_id_8573")));
