/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3041: UTF-8 lead-byte predicate (uint form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_utf8_is_lead_u(unsigned char b);
 *     - Return 1 if b can start a well-formed UTF-8 sequence
 *       (ASCII or legal multi-byte lead C2..F4), else 0. Unsigned form.
 *   uint32_t __gj_utf8_is_lead_u  (alias)
 *   __libcgj_batch3041_marker = "libcgj-batch3041"
 *
 * Milestone 3050 exclusive CREATE-ONLY (3041-3050). Unique
 * gj_utf8_is_lead_u surface only; no multi-def. Distinct from
 * gj_utf8_is_lead (batch1023 int form), gj_utf8_is_lead1..4
 * (batch2252-2255), and gj_utf8_is_cont_u (batch3042). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3041_marker[] = "libcgj-batch3041";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1u if u8B is a legal UTF-8 lead:
 *   0x00..0x7F  — ASCII (1-byte)
 *   0xC2..0xDF  — 2-byte
 *   0xE0..0xEF  — 3-byte
 *   0xF0..0xF4  — 4-byte
 * Continuations (80..BF), C0/C1, and F5..FF return 0.
 */
static uint32_t
b3041_is_lead(unsigned char u8B)
{
	if (u8B < 0x80u) {
		return 1u;
	}
	if (u8B >= 0xC2u && u8B <= 0xF4u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_is_lead_u - 1 if b is a legal UTF-8 sequence lead octet.
 *
 * b: candidate octet
 *
 * Does not call libc. No parent wires.
 */
uint32_t
gj_utf8_is_lead_u(unsigned char u8B)
{
	(void)NULL;
	return b3041_is_lead(u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_utf8_is_lead_u(unsigned char u8B)
    __attribute__((alias("gj_utf8_is_lead_u")));
