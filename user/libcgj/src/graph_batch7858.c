/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7858: iconv same codeset class convert stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iconv_cs_same_class_ok_u_7858(uint32_t from, uint32_t to);
 *     - Return 1 if from == to and both are non-zero class ids, else 0.
 *   uint32_t __gj_iconv_cs_same_class_ok_u_7858  (alias)
 *   __libcgj_batch7858_marker = "libcgj-batch7858"
 *
 * Exclusive continuum CREATE-ONLY (7851-7860: iconv codeset stubs —
 * name_max, name_len_ok, nonempty, utf8_id, ascii_id, latin1_id,
 * unknown_id, same_class_ok, pair_req_ok, continuum + batch_id_7860).
 * Unique gj_iconv_cs_same_class_ok_u_7858 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7858_marker[] = "libcgj-batch7858";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7858_same_class_ok(uint32_t u32From, uint32_t u32To)
{
	if (u32From == 0u)
		return 0u;
	if (u32To == 0u)
		return 0u;
	if (u32From != u32To)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iconv_cs_same_class_ok_u_7858 - 1 if both class ids match and known.
 *
 * from: soft source codeset class id
 * to:   soft destination codeset class id
 *
 * Returns 1 when from == to and both are non-zero, else 0. Pure
 * predicate for identity-convert eligibility; no parent wires.
 */
uint32_t
gj_iconv_cs_same_class_ok_u_7858(uint32_t u32From, uint32_t u32To)
{
	(void)NULL;
	return b7858_same_class_ok(u32From, u32To);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iconv_cs_same_class_ok_u_7858(uint32_t u32From, uint32_t u32To)
    __attribute__((alias("gj_iconv_cs_same_class_ok_u_7858")));
