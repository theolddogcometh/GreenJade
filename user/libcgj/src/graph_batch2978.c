/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2978: ELF e_type reader (_u exclusive).
 *
 * Surface (unique symbols):
 *   uint16_t gj_elf_type_u(const void *buf, size_t len);
 *     - If buf is ELF magic, EI_DATA is LE or BE, and len >= 18, return
 *       e_type (u16 at offset 16) in the file's declared endianness.
 *       Else return 0. NULL / short / non-ELF / unknown EI_DATA → 0.
 *   uint16_t __gj_elf_type_u  (alias)
 *   __libcgj_batch2978_marker = "libcgj-batch2978"
 *
 * UUID/ELF/PE exclusive continuum wave (2971-2980). Distinct from
 * gj_elf_is_et_exec / is_et_dyn / is_et_rel — unique gj_elf_type_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2978_marker[] = "libcgj-batch2978";

#define B2978_EI_DATA      5u
#define B2978_ELFDATA2LSB  1u
#define B2978_ELFDATA2MSB  2u
#define B2978_E_TYPE_OFF   16u

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned
b2978_is_elf_magic(const unsigned char *p)
{
	if (p[0] != 0x7fu || p[1] != (unsigned char)'E' ||
	    p[2] != (unsigned char)'L' || p[3] != (unsigned char)'F') {
		return 0u;
	}
	return 1u;
}

static uint16_t
b2978_rd_u16(const unsigned char *p, unsigned uData)
{
	if (uData == B2978_ELFDATA2LSB) {
		return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
	}
	/* ELFDATA2MSB */
	return ((uint16_t)p[0] << 8) | (uint16_t)p[1];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_type_u - e_type when ELF + valid EI_DATA; else 0.
 *
 * buf: image start (NULL → 0)
 * len: byte length (need >= 18 for e_type)
 *
 * e_type is at offset 16 (immediately after e_ident). Endianness
 * follows EI_DATA. Note: a true ET_NONE (0) is indistinguishable from
 * the failure return of 0.
 */
uint16_t
gj_elf_type_u(const void *pBuf, size_t cb)
{
	const unsigned char *p;
	unsigned uData;

	(void)NULL;
	if (pBuf == NULL || cb < (B2978_E_TYPE_OFF + 2u)) {
		return 0u;
	}
	p = (const unsigned char *)pBuf;
	if (b2978_is_elf_magic(p) == 0u) {
		return 0u;
	}
	uData = (unsigned)p[B2978_EI_DATA];
	if (uData != B2978_ELFDATA2LSB && uData != B2978_ELFDATA2MSB) {
		return 0u;
	}
	return b2978_rd_u16(p + B2978_E_TYPE_OFF, uData);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_elf_type_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_elf_type_u")));
