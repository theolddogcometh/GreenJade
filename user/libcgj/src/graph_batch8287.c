/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8287: UTF-8 sequence-length from lead probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_utf8_seqlen_u_8287(uint32_t b);
 *     - Return expected UTF-8 sequence length from a lead octet class:
 *       1 (0x00..0x7F), 2 (0xC0..0xDF), 3 (0xE0..0xEF), 4 (0xF0..0xF7),
 *       or 0 if not a structural lead (continuation / illegal prefix).
 *   uint32_t __gj_utf8_seqlen_u_8287  (alias)
 *   __libcgj_batch8287_marker = "libcgj-batch8287"
 *
 * Exclusive continuum CREATE-ONLY (8281-8290: UTF-8 / byte class probe
 * stubs — is_ascii, lead1, lead2, lead3, lead4, cont, seqlen,
 * valid_lead, overlong_guard, batch_id_8290). No full decoder.
 * Unique gj_utf8_seqlen_u_8287 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8287_marker[] = "libcgj-batch8287";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8287_seqlen(uint32_t u32B)
{
	uint32_t u8;

	u8 = u32B & 0xFFu;
	if ((u8 & 0x80u) == 0u)
		return 1u;
	if ((u8 & 0xE0u) == 0xC0u)
		return 2u;
	if ((u8 & 0xF0u) == 0xE0u)
		return 3u;
	if ((u8 & 0xF8u) == 0xF0u)
		return 4u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_seqlen_u_8287 - structural sequence length from a lead octet.
 *
 * b: candidate lead octet (only low 8 bits matter)
 *
 * Returns 1..4 for structural lead classes, or 0 for continuation /
 * illegal prefixes (0x80..0xBF, 0xF8..0xFF). Soft class probe only;
 * does not validate continuations or scalar range. No parent wires.
 */
uint32_t
gj_utf8_seqlen_u_8287(uint32_t u32B)
{
	(void)NULL;
	return b8287_seqlen(u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_utf8_seqlen_u_8287(uint32_t u32B)
    __attribute__((alias("gj_utf8_seqlen_u_8287")));
