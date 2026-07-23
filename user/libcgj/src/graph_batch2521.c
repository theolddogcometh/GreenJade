/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2521: ELF identity magic soft-ok (unsigned wave).
 *
 * Surface (unique symbols):
 *   int gj_elf_magic_ok_u(const void *buf, size_t len);
 *     - Return 1 if buf begins with ELF magic 0x7F 'E' 'L' 'F'
 *       and len >= 4; else 0. NULL / short / non-ELF → 0.
 *   int __gj_elf_magic_ok_u  (alias)
 *   __libcgj_batch2521_marker = "libcgj-batch2521"
 *
 * ELF exclusive wave (2521-2530). Distinct from gj_elf_is_magic
 * (batch1141) / gj_elf64_validate — unique gj_elf_magic_ok_u surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2521_marker[] = "libcgj-batch2521";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2521_magic_ok(const unsigned char *p, size_t cb)
{
	if (p == NULL || cb < 4u) {
		return 0;
	}
	if (p[0] != 0x7fu || p[1] != (unsigned char)'E' ||
	    p[2] != (unsigned char)'L' || p[3] != (unsigned char)'F') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_magic_ok_u - 1 if EI_MAG0..3 is 0x7F 'E' 'L' 'F'.
 *
 * buf: candidate image bytes; len: available octet count
 */
int
gj_elf_magic_ok_u(const void *pBuf, size_t cb)
{
	(void)NULL;
	if (pBuf == NULL) {
		return 0;
	}
	return b2521_magic_ok((const unsigned char *)pBuf, cb);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_elf_magic_ok_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_elf_magic_ok_u")));
