/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8288: UTF-8 valid-lead soft range probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_utf8_valid_lead_u_8288(uint32_t b);
 *     - Return 1 if low 8 bits are a soft "valid lead" for starting a
 *       well-formed UTF-8 unit: 0x00..0x7F, 0xC2..0xDF, 0xE0..0xEF,
 *       or 0xF0..0xF4. Rejects continuation, C0/C1, F5..FF.
 *   uint32_t __gj_utf8_valid_lead_u_8288  (alias)
 *   __libcgj_batch8288_marker = "libcgj-batch8288"
 *
 * Exclusive continuum CREATE-ONLY (8281-8290: UTF-8 / byte class probe
 * stubs — is_ascii, lead1, lead2, lead3, lead4, cont, seqlen,
 * valid_lead, overlong_guard, batch_id_8290). No full decoder.
 * Unique gj_utf8_valid_lead_u_8288 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8288_marker[] = "libcgj-batch8288";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8288_valid_lead(uint32_t u32B)
{
	uint32_t u8;

	u8 = u32B & 0xFFu;
	if (u8 < 0x80u)
		return 1u;
	/* Strict 2-byte leads: C2..DF (drop overlong C0/C1). */
	if (u8 >= 0xC2u && u8 <= 0xDFu)
		return 1u;
	if (u8 >= 0xE0u && u8 <= 0xEFu)
		return 1u;
	/* Unicode scalar max U+10FFFF ⇒ F0..F4 only. */
	if (u8 >= 0xF0u && u8 <= 0xF4u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_valid_lead_u_8288 - 1 if low byte may start a well-formed unit.
 *
 * b: candidate lead octet (only low 8 bits matter)
 *
 * Returns 1 for soft valid-lead ranges (ASCII, C2..DF, E0..EF,
 * F0..F4), else 0. Does not read continuations or check overlong
 * second-byte constraints. No parent wires.
 */
uint32_t
gj_utf8_valid_lead_u_8288(uint32_t u32B)
{
	(void)NULL;
	return b8288_valid_lead(u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_utf8_valid_lead_u_8288(uint32_t u32B)
    __attribute__((alias("gj_utf8_valid_lead_u_8288")));
