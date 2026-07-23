/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8572: iconv charset ASCII id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iconv_ascii_id_8572(void);
 *     - Return soft charset id for ASCII (2).
 *   uint32_t __gj_iconv_ascii_id_8572  (alias)
 *   __libcgj_batch8572_marker = "libcgj-batch8572"
 *
 * Exclusive continuum CREATE-ONLY (8571-8580: iconv charset id stubs —
 * utf8_id, ascii_id, latin1_id, utf16le_id, utf16be_id, same_ok,
 * open_ready, close_ready, buf_ok, batch_id_8580).
 * Unique gj_iconv_ascii_id_8572 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8572_marker[] = "libcgj-batch8572";

/* Soft charset id matching local iconv ASCII codeset. */
#define B8572_CS_ASCII  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8572_ascii_id(void)
{
	return B8572_CS_ASCII;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iconv_ascii_id_8572 - soft ASCII charset id.
 *
 * Always returns 2. Pure constant tag; no parent wires. Does not call
 * iconv or classify string names.
 */
uint32_t
gj_iconv_ascii_id_8572(void)
{
	(void)NULL;
	return b8572_ascii_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iconv_ascii_id_8572(void)
    __attribute__((alias("gj_iconv_ascii_id_8572")));
