/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1722: ELF EI_DATA exclusive (endian 1/2).
 *
 * Surface (unique symbols):
 *   int gj_elf_data_from_ei(const void *buf, size_t len);
 *     — If buf is ELF magic and len >= 6, return EI_DATA only when it
 *       is ELFDATA2LSB (1) or ELFDATA2MSB (2). Any other data byte, NULL,
 *       short, or non-ELF → 0. Stricter than gj_elf_data (batch1143),
 *       which echoes the raw EI_DATA octet.
 *   int __gj_elf_data_from_ei  (alias)
 *   __libcgj_batch1722_marker = "libcgj-batch1722"
 *
 * Distinct from gj_elf_data / gj_elf_class / gj_elf_class_from_ei —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1722_marker[] = "libcgj-batch1722";

#define B1722_ELFDATA2LSB 1
#define B1722_ELFDATA2MSB 2

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1722_is_elf_magic(const unsigned char *p, size_t cb)
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
 * gj_elf_data_from_ei — EI_DATA when it is 1 (LE) or 2 (BE); else 0.
 */
int
gj_elf_data_from_ei(const void *pBuf, size_t cb)
{
	const unsigned char *p;
	int nData;

	if (pBuf == NULL || cb < 6u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (!b1722_is_elf_magic(p, cb)) {
		return 0;
	}
	nData = (int)p[5];
	if (nData != B1722_ELFDATA2LSB && nData != B1722_ELFDATA2MSB) {
		return 0;
	}
	return nData;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_elf_data_from_ei(const void *pBuf, size_t cb)
    __attribute__((alias("gj_elf_data_from_ei")));
