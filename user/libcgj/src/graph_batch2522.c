/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2522: ELF EI_CLASS == ELFCLASS64 predicate.
 *
 * Surface (unique symbols):
 *   int gj_elf_class_is_64_u(const void *buf, size_t len);
 *     - Return 1 if buf is ELF magic, len >= 5, and EI_CLASS is
 *       ELFCLASS64 (2). Else 0. NULL / short / non-ELF / class32 → 0.
 *   int __gj_elf_class_is_64_u  (alias)
 *   __libcgj_batch2522_marker = "libcgj-batch2522"
 *
 * ELF exclusive wave (2521-2530). Distinct from gj_elf_class /
 * gj_elf_class_from_ei / gj_elf_class_is_32_u — unique surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2522_marker[] = "libcgj-batch2522";

#define B2522_ELFCLASS64  2u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2522_is_elf_magic(const unsigned char *p, size_t cb)
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

static int
b2522_class_is_64(const unsigned char *p, size_t cb)
{
	if (!b2522_is_elf_magic(p, cb) || cb < 5u) {
		return 0;
	}
	if ((unsigned)p[4] == B2522_ELFCLASS64) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_class_is_64_u - 1 if EI_CLASS is ELFCLASS64 under valid magic.
 */
int
gj_elf_class_is_64_u(const void *pBuf, size_t cb)
{
	(void)NULL;
	if (pBuf == NULL) {
		return 0;
	}
	return b2522_class_is_64((const unsigned char *)pBuf, cb);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_elf_class_is_64_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_elf_class_is_64_u")));
