/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7859: iconv codeset pair request ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iconv_cs_pair_req_ok_u_7859(uint32_t from, uint32_t to);
 *     - Return 1 if both class ids are non-zero (known), else 0.
 *   uint32_t __gj_iconv_cs_pair_req_ok_u_7859  (alias)
 *   __libcgj_batch7859_marker = "libcgj-batch7859"
 *
 * Exclusive continuum CREATE-ONLY (7851-7860: iconv codeset stubs —
 * name_max, name_len_ok, nonempty, utf8_id, ascii_id, latin1_id,
 * unknown_id, same_class_ok, pair_req_ok, continuum + batch_id_7860).
 * Unique gj_iconv_cs_pair_req_ok_u_7859 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7859_marker[] = "libcgj-batch7859";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7859_pair_req_ok(uint32_t u32From, uint32_t u32To)
{
	if (u32From == 0u)
		return 0u;
	if (u32To == 0u)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iconv_cs_pair_req_ok_u_7859 - 1 if to/from class ids are both known.
 *
 * from: soft source codeset class id
 * to:   soft destination codeset class id
 *
 * Returns 1 when both are non-zero, else 0. Soft open-request gate;
 * no parent wires. Does not call iconv_open.
 */
uint32_t
gj_iconv_cs_pair_req_ok_u_7859(uint32_t u32From, uint32_t u32To)
{
	(void)NULL;
	return b7859_pair_req_ok(u32From, u32To);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iconv_cs_pair_req_ok_u_7859(uint32_t u32From, uint32_t u32To)
    __attribute__((alias("gj_iconv_cs_pair_req_ok_u_7859")));
