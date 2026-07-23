/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7857: iconv codeset class unknown id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iconv_cs_class_unknown_id_7857(void);
 *     - Return soft codeset class id for unknown/unsupported (0).
 *   uint32_t __gj_iconv_cs_class_unknown_id_7857  (alias)
 *   __libcgj_batch7857_marker = "libcgj-batch7857"
 *
 * Exclusive continuum CREATE-ONLY (7851-7860: iconv codeset stubs —
 * name_max, name_len_ok, nonempty, utf8_id, ascii_id, latin1_id,
 * unknown_id, same_class_ok, pair_req_ok, continuum + batch_id_7860).
 * Unique gj_iconv_cs_class_unknown_id_7857 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7857_marker[] = "libcgj-batch7857";

/* Soft class id for unknown / unsupported codeset names. */
#define B7857_CS_UNKNOWN  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7857_unknown_id(void)
{
	return B7857_CS_UNKNOWN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iconv_cs_class_unknown_id_7857 - soft unknown codeset class id.
 *
 * Always returns 0. Pure constant tag; no parent wires. Does not call
 * iconv or classify string names.
 */
uint32_t
gj_iconv_cs_class_unknown_id_7857(void)
{
	(void)NULL;
	return b7857_unknown_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iconv_cs_class_unknown_id_7857(void)
    __attribute__((alias("gj_iconv_cs_class_unknown_id_7857")));
