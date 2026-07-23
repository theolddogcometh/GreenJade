/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7851: iconv codeset name max stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iconv_codeset_name_max_7851(void);
 *     - Return soft max codeset name length (63; room for NUL in 64).
 *   uint32_t __gj_iconv_codeset_name_max_7851  (alias)
 *   __libcgj_batch7851_marker = "libcgj-batch7851"
 *
 * Exclusive continuum CREATE-ONLY (7851-7860: iconv codeset stubs —
 * name_max, name_len_ok, nonempty, utf8_id, ascii_id, latin1_id,
 * unknown_id, same_class_ok, pair_req_ok, continuum + batch_id_7860).
 * Unique gj_iconv_codeset_name_max_7851 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7851_marker[] = "libcgj-batch7851";

/* Soft max codeset name chars (64-byte buffer with trailing NUL). */
#define B7851_CODESET_NAME_MAX  ((uint32_t)63u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7851_name_max(void)
{
	return B7851_CODESET_NAME_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iconv_codeset_name_max_7851 - soft max codeset name length.
 *
 * Always returns 63. Pure constant tag; no parent wires. Does not call
 * iconv_open or any libc codeset API.
 */
uint32_t
gj_iconv_codeset_name_max_7851(void)
{
	(void)NULL;
	return b7851_name_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iconv_codeset_name_max_7851(void)
    __attribute__((alias("gj_iconv_codeset_name_max_7851")));
