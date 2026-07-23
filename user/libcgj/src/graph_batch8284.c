/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8284: UTF-8 3-byte lead class probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_utf8_lead3_u_8284(uint32_t b);
 *     - Return 1 if low 8 bits form a structural 3-byte UTF-8 lead
 *       (0xE0..0xEF, top bits 1110), else 0.
 *   uint32_t __gj_utf8_lead3_u_8284  (alias)
 *   __libcgj_batch8284_marker = "libcgj-batch8284"
 *
 * Exclusive continuum CREATE-ONLY (8281-8290: UTF-8 / byte class probe
 * stubs — is_ascii, lead1, lead2, lead3, lead4, cont, seqlen,
 * valid_lead, overlong_guard, batch_id_8290). No full decoder.
 * Unique gj_utf8_lead3_u_8284 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8284_marker[] = "libcgj-batch8284";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8284_lead3(uint32_t u32B)
{
	/* 1110xxxx */
	if ((u32B & 0xF0u) == 0xE0u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_lead3_u_8284 - 1 if low byte is a structural 3-byte lead.
 *
 * b: candidate octet (only low 8 bits matter)
 *
 * Returns 1 for 0xE0..0xEF, else 0. Soft structural class only;
 * does not inspect following continuation bytes. No parent wires.
 */
uint32_t
gj_utf8_lead3_u_8284(uint32_t u32B)
{
	(void)NULL;
	return b8284_lead3(u32B & 0xFFu);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_utf8_lead3_u_8284(uint32_t u32B)
    __attribute__((alias("gj_utf8_lead3_u_8284")));
