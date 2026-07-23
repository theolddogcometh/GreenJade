/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3355: ELF e_entry reader (_u exclusive).
 *
 * Surface (unique symbols):
 *   uint64_t gj_elf_entry_u(const void *buf, size_t len);
 *     - If buf is ELF magic, EI_DATA is LE or BE, EI_CLASS is 32 or 64,
 *       and len covers e_entry, return e_entry in the file's declared
 *       endianness (zero-extended from u32 for ELFCLASS32). Else 0.
 *       e_entry starts at offset 24 for both classes.
 *       ELFCLASS32: u32, need len >= 28.
 *       ELFCLASS64: u64, need len >= 32.
 *       NULL / short / non-ELF / unknown class or EI_DATA → 0.
 *   uint64_t __gj_elf_entry_u  (alias)
 *   __libcgj_batch3355_marker = "libcgj-batch3355"
 *
 * CREATE-ONLY exclusive continuum wave (3351-3360). Distinct from
 * gj_elf64_validate / prior elf_*_u readers — unique gj_elf_entry_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3355_marker[] = "libcgj-batch3355";

#define B3355_EI_CLASS         4u
#define B3355_EI_DATA          5u
#define B3355_ELFCLASS32       1u
#define B3355_ELFCLASS64       2u
#define B3355_ELFDATA2LSB      1u
#define B3355_ELFDATA2MSB      2u
#define B3355_E_ENTRY_OFF      24u

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned
b3355_is_elf_magic(const unsigned char *p)
{
	if (p[0] != 0x7fu || p[1] != (unsigned char)'E' ||
	    p[2] != (unsigned char)'L' || p[3] != (unsigned char)'F') {
		return 0u;
	}
	return 1u;
}

static uint32_t
b3355_rd_u32(const unsigned char *p, unsigned uData)
{
	if (uData == B3355_ELFDATA2LSB) {
		return (uint32_t)p[0] |
		       ((uint32_t)p[1] << 8) |
		       ((uint32_t)p[2] << 16) |
		       ((uint32_t)p[3] << 24);
	}
	/* ELFDATA2MSB */
	return ((uint32_t)p[0] << 24) |
	       ((uint32_t)p[1] << 16) |
	       ((uint32_t)p[2] << 8) |
	       (uint32_t)p[3];
}

static uint64_t
b3355_rd_u64(const unsigned char *p, unsigned uData)
{
	uint32_t uLo;
	uint32_t uHi;

	if (uData == B3355_ELFDATA2LSB) {
		uLo = b3355_rd_u32(p, B3355_ELFDATA2LSB);
		uHi = b3355_rd_u32(p + 4u, B3355_ELFDATA2LSB);
		return (uint64_t)uLo | ((uint64_t)uHi << 32);
	}
	/* ELFDATA2MSB: high word first */
	uHi = b3355_rd_u32(p, B3355_ELFDATA2MSB);
	uLo = b3355_rd_u32(p + 4u, B3355_ELFDATA2MSB);
	return (uint64_t)uLo | ((uint64_t)uHi << 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_entry_u - e_entry when ELF + valid class/EI_DATA; else 0.
 *
 * buf: image start (NULL → 0)
 * len: byte length (class-dependent floor for e_entry)
 *
 * Note: a true e_entry of 0 is indistinguishable from the failure
 * return of 0.
 */
uint64_t
gj_elf_entry_u(const void *pBuf, size_t cb)
{
	const unsigned char *p;
	unsigned uClass;
	unsigned uData;

	(void)NULL;
	if (pBuf == NULL || cb < 6u) {
		return 0u;
	}
	p = (const unsigned char *)pBuf;
	if (b3355_is_elf_magic(p) == 0u) {
		return 0u;
	}
	uClass = (unsigned)p[B3355_EI_CLASS];
	uData = (unsigned)p[B3355_EI_DATA];
	if (uData != B3355_ELFDATA2LSB && uData != B3355_ELFDATA2MSB) {
		return 0u;
	}
	if (uClass == B3355_ELFCLASS32) {
		if (cb < (B3355_E_ENTRY_OFF + 4u)) {
			return 0u;
		}
		return (uint64_t)b3355_rd_u32(p + B3355_E_ENTRY_OFF, uData);
	}
	if (uClass == B3355_ELFCLASS64) {
		if (cb < (B3355_E_ENTRY_OFF + 8u)) {
			return 0u;
		}
		return b3355_rd_u64(p + B3355_E_ENTRY_OFF, uData);
	}
	return 0u;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_elf_entry_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_elf_entry_u")));
