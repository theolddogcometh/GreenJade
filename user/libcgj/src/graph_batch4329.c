/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4329: freestanding 32-bit rolling C-string hash.
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_hash_mix_u(const char *s);
 *     - Simple 32-bit rolling mix over the octets of s before NUL.
 *       Seed 0; step is h = h * 31 + c (unsigned char). s == NULL → 0.
 *   uint32_t __gj_str_hash_mix_u  (alias)
 *   __libcgj_batch4329_marker = "libcgj-batch4329"
 *
 * Exclusive continuum CREATE-ONLY (4321-4330 string utils). Distinct
 * from gj_buf_hash_djb2_u / gj_str_hash_djb2_u family — unique
 * gj_str_hash_mix_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4329_marker[] = "libcgj-batch4329";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4329_hash_mix(const char *sz)
{
	const unsigned char *p;
	uint32_t u32H;

	if (sz == NULL) {
		return 0u;
	}

	p = (const unsigned char *)(const void *)sz;
	u32H = 0u;
	while (*p != (unsigned char)'\0') {
		/* simple rolling mix: h = h * 31 + c */
		u32H = (u32H * 31u) + (uint32_t)(*p);
		p++;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_hash_mix_u - simple 32-bit rolling hash of a C string.
 *
 * s: NUL-terminated string, or NULL
 *
 * Returns 0 when s is NULL; otherwise the rolling mix over payload
 * octets. Does not call libc. Self-contained; no parent wires.
 */
uint32_t
gj_str_hash_mix_u(const char *s)
{
	(void)NULL;
	return b4329_hash_mix(s);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_hash_mix_u(const char *s)
    __attribute__((alias("gj_str_hash_mix_u")));
