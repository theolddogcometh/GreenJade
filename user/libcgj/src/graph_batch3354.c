/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3354: ELF e_phentsize reader (_u exclusive).
 *
 * Surface (unique symbols):
 *   uint16_t gj_elf_phentsize_u(const void *buf, size_t len);
 *     - If buf is ELF magic, EI_DATA is LE or BE, EI_CLASS is 32 or 64,
 *       and len covers e_phentsize, return e_phentsize (u16) in the
 *       file's declared endianness. Else 0.
 *       ELFCLASS32: e_phentsize at offset 42 (need len >= 44).
 *       ELFCLASS64: e_phentsize at offset 54 (need len >= 56).
 *       NULL / short / non-ELF / unknown class or EI_DATA → 0.
 *   uint16_t __gj_elf_phentsize_u  (alias)
 *   __libcgj_batch3354_marker = "libcgj-batch3354"
 *
 * CREATE-ONLY exclusive continuum wave (3351-3360). Distinct from
 * gj_elf64_phdr / prior elf_*_u readers — unique gj_elf_phentsize_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3354_marker[] = "libcgj-batch3354";

#define B3354_EI_CLASS            4u
#define B3354_EI_DATA             5u
#define B3354_ELFCLASS32          1u
#define B3354_ELFCLASS64          2u
#define B3354_ELFDATA2LSB         1u
#define B3354_ELFDATA2MSB         2u
#define B3354_E_PHENTSIZE_OFF32   42u
#define B3354_E_PHENTSIZE_OFF64   54u

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned
b3354_is_elf_magic(const unsigned char *p)
{
	if (p[0] != 0x7fu || p[1] != (unsigned char)'E' ||
	    p[2] != (unsigned char)'L' || p[3] != (unsigned char)'F') {
		return 0u;
	}
	return 1u;
}

static uint16_t
b3354_rd_u16(const unsigned char *p, unsigned uData)
{
	if (uData == B3354_ELFDATA2LSB) {
		return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
	}
	/* ELFDATA2MSB */
	return ((uint16_t)p[0] << 8) | (uint16_t)p[1];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_phentsize_u - e_phentsize when ELF + valid class/EI_DATA; else 0.
 *
 * buf: image start (NULL → 0)
 * len: byte length (class-dependent floor for e_phentsize)
 *
 * Note: a true e_phentsize of 0 is indistinguishable from the failure
 * return of 0.
 */
uint16_t
gj_elf_phentsize_u(const void *pBuf, size_t cb)
{
	const unsigned char *p;
	unsigned uClass;
	unsigned uData;
	unsigned uOff;

	(void)NULL;
	if (pBuf == NULL || cb < 6u) {
		return 0u;
	}
	p = (const unsigned char *)pBuf;
	if (b3354_is_elf_magic(p) == 0u) {
		return 0u;
	}
	uClass = (unsigned)p[B3354_EI_CLASS];
	uData = (unsigned)p[B3354_EI_DATA];
	if (uData != B3354_ELFDATA2LSB && uData != B3354_ELFDATA2MSB) {
		return 0u;
	}
	if (uClass == B3354_ELFCLASS32) {
		uOff = B3354_E_PHENTSIZE_OFF32;
	} else if (uClass == B3354_ELFCLASS64) {
		uOff = B3354_E_PHENTSIZE_OFF64;
	} else {
		return 0u;
	}
	if (cb < (uOff + 2u)) {
		return 0u;
	}
	return b3354_rd_u16(p + uOff, uData);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_elf_phentsize_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_elf_phentsize_u")));
