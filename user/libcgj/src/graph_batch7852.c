/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7852: iconv codeset name length ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iconv_codeset_name_len_ok_u_7852(uint32_t len);
 *     - Return 1 if len is in (0, 63], else 0.
 *   uint32_t __gj_iconv_codeset_name_len_ok_u_7852  (alias)
 *   __libcgj_batch7852_marker = "libcgj-batch7852"
 *
 * Exclusive continuum CREATE-ONLY (7851-7860: iconv codeset stubs —
 * name_max, name_len_ok, nonempty, utf8_id, ascii_id, latin1_id,
 * unknown_id, same_class_ok, pair_req_ok, continuum + batch_id_7860).
 * Unique gj_iconv_codeset_name_len_ok_u_7852 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7852_marker[] = "libcgj-batch7852";

/* Soft max codeset name chars (excluding trailing NUL). */
#define B7852_CODESET_NAME_MAX  ((uint32_t)63u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7852_name_len_ok(uint32_t u32Len)
{
	if (u32Len == 0u)
		return 0u;
	if (u32Len > B7852_CODESET_NAME_MAX)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iconv_codeset_name_len_ok_u_7852 - 1 if codeset name length fits.
 *
 * len: soft codeset name length in bytes (excluding trailing NUL)
 *
 * Returns 1 when 0 < len <= 63, else 0. Pure range test; no parent
 * wires. Does not call iconv.
 */
uint32_t
gj_iconv_codeset_name_len_ok_u_7852(uint32_t u32Len)
{
	(void)NULL;
	return b7852_name_len_ok(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iconv_codeset_name_len_ok_u_7852(uint32_t u32Len)
    __attribute__((alias("gj_iconv_codeset_name_len_ok_u_7852")));
