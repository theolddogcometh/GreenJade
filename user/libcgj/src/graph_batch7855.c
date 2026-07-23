/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7855: iconv codeset class ASCII id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iconv_cs_class_ascii_id_7855(void);
 *     - Return soft codeset class id for ASCII (2).
 *   uint32_t __gj_iconv_cs_class_ascii_id_7855  (alias)
 *   __libcgj_batch7855_marker = "libcgj-batch7855"
 *
 * Exclusive continuum CREATE-ONLY (7851-7860: iconv codeset stubs —
 * name_max, name_len_ok, nonempty, utf8_id, ascii_id, latin1_id,
 * unknown_id, same_class_ok, pair_req_ok, continuum + batch_id_7860).
 * Unique gj_iconv_cs_class_ascii_id_7855 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7855_marker[] = "libcgj-batch7855";

/* Soft class id matching local iconv ASCII codeset class. */
#define B7855_CS_ASCII  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7855_ascii_id(void)
{
	return B7855_CS_ASCII;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iconv_cs_class_ascii_id_7855 - soft ASCII codeset class id.
 *
 * Always returns 2. Pure constant tag; no parent wires. Does not call
 * iconv or classify string names.
 */
uint32_t
gj_iconv_cs_class_ascii_id_7855(void)
{
	(void)NULL;
	return b7855_ascii_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iconv_cs_class_ascii_id_7855(void)
    __attribute__((alias("gj_iconv_cs_class_ascii_id_7855")));
