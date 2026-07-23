/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2529: ELF e_machine == EM_X86_64 predicate.
 *
 * Surface (unique symbols):
 *   int gj_elf_machine_x86_64_u(const void *buf, size_t len);
 *     - Return 1 if buffer is ELF magic, EI_DATA is LE or BE, and
 *       e_machine (u16 at offset 18) equals EM_X86_64 (62), read in
 *       the file's declared endianness. Else 0. Requires len >= 20.
 *       NULL / short / non-ELF / unknown EI_DATA → 0.
 *   int __gj_elf_machine_x86_64_u  (alias)
 *   __libcgj_batch2529_marker = "libcgj-batch2529"
 *
 * ELF exclusive wave (2521-2530). Distinct from gj_elf64_validate
 * (batch102 machine gate embedded) — unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2529_marker[] = "libcgj-batch2529";

#define B2529_ELFDATA2LSB  1u
#define B2529_ELFDATA2MSB  2u
#define B2529_EM_X86_64    62u
#define B2529_E_MACH_OFF   18u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2529_is_elf_magic(const unsigned char *p, size_t cb)
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
b2529_rd_u16(const unsigned char *p, unsigned uData)
{
	if (uData == B2529_ELFDATA2LSB) {
		return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
	}
	/* ELFDATA2MSB */
	return ((uint16_t)p[0] << 8) | (uint16_t)p[1];
}

static int
b2529_machine_x86_64(const unsigned char *p, size_t cb)
{
	unsigned uData;
	uint16_t u16Mach;

	if (p == NULL || cb < (B2529_E_MACH_OFF + 2u)) {
		return 0;
	}
	if (!b2529_is_elf_magic(p, cb)) {
		return 0;
	}
	uData = (unsigned)p[5];
	if (uData != B2529_ELFDATA2LSB && uData != B2529_ELFDATA2MSB) {
		return 0;
	}
	u16Mach = b2529_rd_u16(p + B2529_E_MACH_OFF, uData);
	if (u16Mach == B2529_EM_X86_64) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_machine_x86_64_u - 1 if e_machine is EM_X86_64 under EI_DATA.
 */
int
gj_elf_machine_x86_64_u(const void *pBuf, size_t cb)
{
	(void)NULL;
	if (pBuf == NULL) {
		return 0;
	}
	return b2529_machine_x86_64((const unsigned char *)pBuf, cb);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_elf_machine_x86_64_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_elf_machine_x86_64_u")));
