/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7854: iconv codeset class UTF-8 id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iconv_cs_class_utf8_id_7854(void);
 *     - Return soft codeset class id for UTF-8 (1).
 *   uint32_t __gj_iconv_cs_class_utf8_id_7854  (alias)
 *   __libcgj_batch7854_marker = "libcgj-batch7854"
 *
 * Exclusive continuum CREATE-ONLY (7851-7860: iconv codeset stubs —
 * name_max, name_len_ok, nonempty, utf8_id, ascii_id, latin1_id,
 * unknown_id, same_class_ok, pair_req_ok, continuum + batch_id_7860).
 * Unique gj_iconv_cs_class_utf8_id_7854 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7854_marker[] = "libcgj-batch7854";

/* Soft class id matching local iconv UTF-8 codeset class. */
#define B7854_CS_UTF8  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7854_utf8_id(void)
{
	return B7854_CS_UTF8;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iconv_cs_class_utf8_id_7854 - soft UTF-8 codeset class id.
 *
 * Always returns 1. Pure constant tag; no parent wires. Does not call
 * iconv or classify string names.
 */
uint32_t
gj_iconv_cs_class_utf8_id_7854(void)
{
	(void)NULL;
	return b7854_utf8_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iconv_cs_class_utf8_id_7854(void)
    __attribute__((alias("gj_iconv_cs_class_utf8_id_7854")));
