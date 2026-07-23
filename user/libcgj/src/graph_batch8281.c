/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8281: UTF-8 ASCII single-byte class probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_utf8_is_ascii_u_8281(uint32_t b);
 *     - Return 1 if low 8 bits of b are in 0x00..0x7F (ASCII / UTF-8
 *       1-byte range), else 0. Single-byte class probe only.
 *   uint32_t __gj_utf8_is_ascii_u_8281  (alias)
 *   __libcgj_batch8281_marker = "libcgj-batch8281"
 *
 * Exclusive continuum CREATE-ONLY (8281-8290: UTF-8 / byte class probe
 * stubs — is_ascii, lead1, lead2, lead3, lead4, cont, seqlen,
 * valid_lead, overlong_guard, batch_id_8290). No full decoder.
 * Unique gj_utf8_is_ascii_u_8281 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8281_marker[] = "libcgj-batch8281";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8281_is_ascii(uint32_t u32B)
{
	if ((u32B & 0xFFu) < 0x80u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_is_ascii_u_8281 - 1 if low byte is ASCII (< 0x80).
 *
 * b: candidate octet (only low 8 bits matter)
 *
 * Returns 1 for 0x00..0x7F, else 0. Soft single-byte class probe;
 * does not walk a buffer or decode a sequence. No parent wires.
 */
uint32_t
gj_utf8_is_ascii_u_8281(uint32_t u32B)
{
	(void)NULL;
	return b8281_is_ascii(u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_utf8_is_ascii_u_8281(uint32_t u32B)
    __attribute__((alias("gj_utf8_is_ascii_u_8281")));
