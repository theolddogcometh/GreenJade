/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1723: ELF e_type == ET_EXEC probe.
 *
 * Surface (unique symbols):
 *   int gj_elf_is_et_exec(const void *buf, size_t len);
 *     — Return 1 if buffer is ELF magic, EI_DATA is LE or BE, and
 *       e_type (u16 at offset 16) equals ET_EXEC (2), read in the
 *       file's declared endianness. Else 0. Requires len >= 18.
 *       NULL / short / non-ELF / unknown EI_DATA → 0.
 *   int __gj_elf_is_et_exec  (alias)
 *   __libcgj_batch1723_marker = "libcgj-batch1723"
 *
 * Beyond-magic exclusive: type field, not identity alone. Distinct from
 * gj_elf_is_magic / gj_elf64_validate / is_et_dyn / is_et_rel.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1723_marker[] = "libcgj-batch1723";

#define B1723_ELFDATA2LSB 1u
#define B1723_ELFDATA2MSB 2u
#define B1723_ET_EXEC     2u
#define B1723_E_TYPE_OFF  16u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1723_is_elf_magic(const unsigned char *p, size_t cb)
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

static uint16_t
b1723_rd_u16(const unsigned char *p, unsigned uData)
{
	if (uData == B1723_ELFDATA2LSB) {
		return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
	}
	/* ELFDATA2MSB */
	return ((uint16_t)p[0] << 8) | (uint16_t)p[1];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_is_et_exec — 1 if e_type is ET_EXEC under EI_DATA endian.
 */
int
gj_elf_is_et_exec(const void *pBuf, size_t cb)
{
	const unsigned char *p;
	unsigned uData;
	uint16_t u16Type;

	if (pBuf == NULL || cb < (B1723_E_TYPE_OFF + 2u)) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (!b1723_is_elf_magic(p, cb)) {
		return 0;
	}
	uData = (unsigned)p[5];
	if (uData != B1723_ELFDATA2LSB && uData != B1723_ELFDATA2MSB) {
		return 0;
	}
	u16Type = b1723_rd_u16(p + B1723_E_TYPE_OFF, uData);
	if (u16Type == B1723_ET_EXEC) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_elf_is_et_exec(const void *pBuf, size_t cb)
    __attribute__((alias("gj_elf_is_et_exec")));
