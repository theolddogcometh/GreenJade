/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8285: UTF-8 4-byte lead class probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_utf8_lead4_u_8285(uint32_t b);
 *     - Return 1 if low 8 bits form a structural 4-byte UTF-8 lead
 *       (0xF0..0xF7, top bits 11110), else 0. Includes F5..F7 as
 *       structural class; valid-lead tightening is batch8288.
 *   uint32_t __gj_utf8_lead4_u_8285  (alias)
 *   __libcgj_batch8285_marker = "libcgj-batch8285"
 *
 * Exclusive continuum CREATE-ONLY (8281-8290: UTF-8 / byte class probe
 * stubs — is_ascii, lead1, lead2, lead3, lead4, cont, seqlen,
 * valid_lead, overlong_guard, batch_id_8290). No full decoder.
 * Unique gj_utf8_lead4_u_8285 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8285_marker[] = "libcgj-batch8285";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8285_lead4(uint32_t u32B)
{
	/* 11110xxx */
	if ((u32B & 0xF8u) == 0xF0u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_lead4_u_8285 - 1 if low byte is a structural 4-byte lead.
 *
 * b: candidate octet (only low 8 bits matter)
 *
 * Returns 1 for 0xF0..0xF7, else 0. Soft structural class only;
 * does not cap at F4 or check continuations. No parent wires.
 */
uint32_t
gj_utf8_lead4_u_8285(uint32_t u32B)
{
	(void)NULL;
	return b8285_lead4(u32B & 0xFFu);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_utf8_lead4_u_8285(uint32_t u32B)
    __attribute__((alias("gj_utf8_lead4_u_8285")));
