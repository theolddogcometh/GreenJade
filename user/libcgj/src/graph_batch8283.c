/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8283: UTF-8 2-byte lead class probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_utf8_lead2_u_8283(uint32_t b);
 *     - Return 1 if low 8 bits form a structural 2-byte UTF-8 lead
 *       (0xC0..0xDF, top bits 110), else 0. Includes overlong C0/C1
 *       as structural class; overlong rejection is batch8289.
 *   uint32_t __gj_utf8_lead2_u_8283  (alias)
 *   __libcgj_batch8283_marker = "libcgj-batch8283"
 *
 * Exclusive continuum CREATE-ONLY (8281-8290: UTF-8 / byte class probe
 * stubs — is_ascii, lead1, lead2, lead3, lead4, cont, seqlen,
 * valid_lead, overlong_guard, batch_id_8290). No full decoder.
 * Unique gj_utf8_lead2_u_8283 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8283_marker[] = "libcgj-batch8283";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8283_lead2(uint32_t u32B)
{
	/* 110xxxxx */
	if ((u32B & 0xE0u) == 0xC0u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_lead2_u_8283 - 1 if low byte is a structural 2-byte lead.
 *
 * b: candidate octet (only low 8 bits matter)
 *
 * Returns 1 for 0xC0..0xDF, else 0. Soft structural class only;
 * does not enforce C2..DF well-formed range. No parent wires.
 */
uint32_t
gj_utf8_lead2_u_8283(uint32_t u32B)
{
	(void)NULL;
	return b8283_lead2(u32B & 0xFFu);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_utf8_lead2_u_8283(uint32_t u32B)
    __attribute__((alias("gj_utf8_lead2_u_8283")));
