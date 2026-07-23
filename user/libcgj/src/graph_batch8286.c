/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8286: UTF-8 continuation byte class probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_utf8_cont_u_8286(uint32_t b);
 *     - Return 1 if low 8 bits form a UTF-8 continuation octet
 *       (0x80..0xBF, top bits 10), else 0.
 *   uint32_t __gj_utf8_cont_u_8286  (alias)
 *   __libcgj_batch8286_marker = "libcgj-batch8286"
 *
 * Exclusive continuum CREATE-ONLY (8281-8290: UTF-8 / byte class probe
 * stubs — is_ascii, lead1, lead2, lead3, lead4, cont, seqlen,
 * valid_lead, overlong_guard, batch_id_8290). No full decoder.
 * Unique gj_utf8_cont_u_8286 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8286_marker[] = "libcgj-batch8286";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8286_cont(uint32_t u32B)
{
	/* 10xxxxxx */
	if ((u32B & 0xC0u) == 0x80u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_cont_u_8286 - 1 if low byte is a UTF-8 continuation octet.
 *
 * b: candidate octet (only low 8 bits matter)
 *
 * Returns 1 for 0x80..0xBF, else 0. Soft structural class only;
 * does not pair with a lead or walk a sequence. No parent wires.
 */
uint32_t
gj_utf8_cont_u_8286(uint32_t u32B)
{
	(void)NULL;
	return b8286_cont(u32B & 0xFFu);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_utf8_cont_u_8286(uint32_t u32B)
    __attribute__((alias("gj_utf8_cont_u_8286")));
