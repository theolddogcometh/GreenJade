/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1141: ELF identity magic check.
 *
 * Surface (unique symbols):
 *   int gj_elf_is_magic(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with ELF magic 0x7F 'E' 'L' 'F'
 *       and len >= 4; else 0 (FAIL). NULL / short → 0.
 *   int __gj_elf_is_magic  (alias)
 *   __libcgj_batch1141_marker = "libcgj-batch1141"
 *
 * Distinct from gj_elf64_validate (batch102) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1141_marker[] = "libcgj-batch1141";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_is_magic — 1 if EI_MAG0..3 is 0x7F 'E' 'L' 'F'.
 */
int
gj_elf_is_magic(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != 0x7fu || p[1] != (unsigned char)'E' ||
	    p[2] != (unsigned char)'L' || p[3] != (unsigned char)'F') {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_elf_is_magic(const void *pBuf, size_t cb)
    __attribute__((alias("gj_elf_is_magic")));
