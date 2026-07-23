/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1721: ELF EI_CLASS exclusive (1/2 only).
 *
 * Surface (unique symbols):
 *   int gj_elf_class_from_ei(const void *buf, size_t len);
 *     — If buf is ELF magic and len >= 5, return EI_CLASS only when it
 *       is ELFCLASS32 (1) or ELFCLASS64 (2). Any other class byte, NULL,
 *       short, or non-ELF → 0. Stricter than gj_elf_class (batch1142),
 *       which echoes the raw EI_CLASS octet.
 *   int __gj_elf_class_from_ei  (alias)
 *   __libcgj_batch1721_marker = "libcgj-batch1721"
 *
 * Distinct from gj_elf_class / gj_elf_is_magic / gj_elf64_validate —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1721_marker[] = "libcgj-batch1721";

#define B1721_ELFCLASS32 1
#define B1721_ELFCLASS64 2

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1721_is_elf_magic(const unsigned char *p, size_t cb)
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
 * gj_elf_class_from_ei — EI_CLASS when it is 1 or 2; else 0.
 */
int
gj_elf_class_from_ei(const void *pBuf, size_t cb)
{
	const unsigned char *p;
	int nClass;

	if (pBuf == NULL || cb < 5u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (!b1721_is_elf_magic(p, cb)) {
		return 0;
	}
	nClass = (int)p[4];
	if (nClass != B1721_ELFCLASS32 && nClass != B1721_ELFCLASS64) {
		return 0;
	}
	return nClass;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_elf_class_from_ei(const void *pBuf, size_t cb)
    __attribute__((alias("gj_elf_class_from_ei")));
