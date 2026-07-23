/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8282: UTF-8 1-byte lead class probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_utf8_lead1_u_8282(uint32_t b);
 *     - Return 1 if low 8 bits form a 1-byte UTF-8 lead (0x00..0x7F),
 *       else 0. Structural class only; no decoder.
 *   uint32_t __gj_utf8_lead1_u_8282  (alias)
 *   __libcgj_batch8282_marker = "libcgj-batch8282"
 *
 * Exclusive continuum CREATE-ONLY (8281-8290: UTF-8 / byte class probe
 * stubs — is_ascii, lead1, lead2, lead3, lead4, cont, seqlen,
 * valid_lead, overlong_guard, batch_id_8290). No full decoder.
 * Unique gj_utf8_lead1_u_8282 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8282_marker[] = "libcgj-batch8282";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8282_lead1(uint32_t u32B)
{
	/* 0xxxxxxx */
	if ((u32B & 0x80u) == 0u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_lead1_u_8282 - 1 if low byte is a 1-byte UTF-8 lead.
 *
 * b: candidate octet (only low 8 bits matter)
 *
 * Returns 1 for 0x00..0x7F (bit7 clear), else 0. Soft structural
 * class probe only. No parent wires.
 */
uint32_t
gj_utf8_lead1_u_8282(uint32_t u32B)
{
	(void)NULL;
	return b8282_lead1(u32B & 0xFFu);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_utf8_lead1_u_8282(uint32_t u32B)
    __attribute__((alias("gj_utf8_lead1_u_8282")));
