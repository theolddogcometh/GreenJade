/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1143: ELF EI_DATA reader.
 *
 * Surface (unique symbols):
 *   int gj_elf_data(const void *buf, size_t len);
 *     — If buf is ELF magic and len >= 6, return EI_DATA
 *       (1 = ELFDATA2LSB, 2 = ELFDATA2MSB). Else return 0 (FAIL).
 *       NULL / short / non-ELF → 0.
 *   int __gj_elf_data  (alias)
 *   __libcgj_batch1143_marker = "libcgj-batch1143"
 *
 * Distinct from gj_elf_is_magic / gj_elf_class / gj_elf64_validate —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1143_marker[] = "libcgj-batch1143";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_data — return EI_DATA when magic is valid; else 0.
 */
int
gj_elf_data(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 6u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != 0x7fu || p[1] != (unsigned char)'E' ||
	    p[2] != (unsigned char)'L' || p[3] != (unsigned char)'F') {
		return 0;
	}
	return (int)p[5];
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_elf_data(const void *pBuf, size_t cb)
    __attribute__((alias("gj_elf_data")));
