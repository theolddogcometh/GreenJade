/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2528: ELF e_type == ET_REL predicate.
 *
 * Surface (unique symbols):
 *   int gj_elf_type_rel_u(const void *buf, size_t len);
 *     - Return 1 if buffer is ELF magic, EI_DATA is LE or BE, and
 *       e_type (u16 at offset 16) equals ET_REL (1), read in the
 *       file's declared endianness. Else 0. Requires len >= 18.
 *       NULL / short / non-ELF / unknown EI_DATA → 0.
 *   int __gj_elf_type_rel_u  (alias)
 *   __libcgj_batch2528_marker = "libcgj-batch2528"
 *
 * ELF exclusive wave (2521-2530). Distinct from gj_elf_is_et_rel
 * (batch1725) / type_exec / type_dyn — unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2528_marker[] = "libcgj-batch2528";

#define B2528_ELFDATA2LSB  1u
#define B2528_ELFDATA2MSB  2u
#define B2528_ET_REL       1u
#define B2528_E_TYPE_OFF   16u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2528_is_elf_magic(const unsigned char *p, size_t cb)
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
b2528_rd_u16(const unsigned char *p, unsigned uData)
{
	if (uData == B2528_ELFDATA2LSB) {
		return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
	}
	/* ELFDATA2MSB */
	return ((uint16_t)p[0] << 8) | (uint16_t)p[1];
}

static int
b2528_type_rel(const unsigned char *p, size_t cb)
{
	unsigned uData;
	uint16_t u16Type;

	if (p == NULL || cb < (B2528_E_TYPE_OFF + 2u)) {
		return 0;
	}
	if (!b2528_is_elf_magic(p, cb)) {
		return 0;
	}
	uData = (unsigned)p[5];
	if (uData != B2528_ELFDATA2LSB && uData != B2528_ELFDATA2MSB) {
		return 0;
	}
	u16Type = b2528_rd_u16(p + B2528_E_TYPE_OFF, uData);
	if (u16Type == B2528_ET_REL) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_type_rel_u - 1 if e_type is ET_REL under EI_DATA endian.
 */
int
gj_elf_type_rel_u(const void *pBuf, size_t cb)
{
	(void)NULL;
	if (pBuf == NULL) {
		return 0;
	}
	return b2528_type_rel((const unsigned char *)pBuf, cb);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_elf_type_rel_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_elf_type_rel_u")));
