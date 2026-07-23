/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3042: UTF-8 continuation-byte predicate (uint).
 *
 * Surface (unique symbols):
 *   uint32_t gj_utf8_is_cont_u(unsigned char b);
 *     - Return 1 if b is a UTF-8 continuation byte (10xxxxxx),
 *       else 0. Unsigned form.
 *   uint32_t __gj_utf8_is_cont_u  (alias)
 *   __libcgj_batch3042_marker = "libcgj-batch3042"
 *
 * Milestone 3050 exclusive CREATE-ONLY (3041-3050). Unique
 * gj_utf8_is_cont_u surface only; no multi-def. Distinct from
 * gj_utf8_is_cont (batch1022 int form), gj_utf8_is_cont_byte
 * (batch2251), and gj_utf8_is_lead_u (batch3041). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3042_marker[] = "libcgj-batch3042";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if (u8B & 0xC0) == 0x80 (continuation form 10xxxxxx). */
static uint32_t
b3042_is_cont(unsigned char u8B)
{
	if ((u8B & 0xC0u) == 0x80u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_is_cont_u - 1 if b matches the UTF-8 continuation form.
 *
 * b: candidate octet
 *
 * Does not call libc. No parent wires.
 */
uint32_t
gj_utf8_is_cont_u(unsigned char u8B)
{
	(void)NULL;
	return b3042_is_cont(u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_utf8_is_cont_u(unsigned char u8B)
    __attribute__((alias("gj_utf8_is_cont_u")));
