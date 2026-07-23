/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2977: ELF e_machine reader (_u exclusive).
 *
 * Surface (unique symbols):
 *   uint16_t gj_elf_machine_u(const void *buf, size_t len);
 *     - If buf is ELF magic, EI_DATA is LE or BE, and len >= 20, return
 *       e_machine (u16 at offset 18) in the file's declared endianness.
 *       Else return 0. NULL / short / non-ELF / unknown EI_DATA → 0.
 *   uint16_t __gj_elf_machine_u  (alias)
 *   __libcgj_batch2977_marker = "libcgj-batch2977"
 *
 * UUID/ELF/PE exclusive continuum wave (2971-2980). Distinct from
 * gj_elf64_validate (batch102) machine gate — unique gj_elf_machine_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2977_marker[] = "libcgj-batch2977";

#define B2977_EI_DATA         5u
#define B2977_ELFDATA2LSB     1u
#define B2977_ELFDATA2MSB     2u
#define B2977_E_MACHINE_OFF   18u

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned
b2977_is_elf_magic(const unsigned char *p)
{
	if (p[0] != 0x7fu || p[1] != (unsigned char)'E' ||
	    p[2] != (unsigned char)'L' || p[3] != (unsigned char)'F') {
		return 0u;
	}
	return 1u;
}

static uint16_t
b2977_rd_u16(const unsigned char *p, unsigned uData)
{
	if (uData == B2977_ELFDATA2LSB) {
		return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
	}
	/* ELFDATA2MSB */
	return ((uint16_t)p[0] << 8) | (uint16_t)p[1];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_machine_u - e_machine when ELF + valid EI_DATA; else 0.
 *
 * buf: image start (NULL → 0)
 * len: byte length (need >= 20 for e_machine)
 *
 * e_machine is at offset 18 (after e_ident[16] + e_type). Endianness
 * follows EI_DATA. Note: a true EM_NONE (0) is indistinguishable from
 * the failure return of 0.
 */
uint16_t
gj_elf_machine_u(const void *pBuf, size_t cb)
{
	const unsigned char *p;
	unsigned uData;

	(void)NULL;
	if (pBuf == NULL || cb < (B2977_E_MACHINE_OFF + 2u)) {
		return 0u;
	}
	p = (const unsigned char *)pBuf;
	if (b2977_is_elf_magic(p) == 0u) {
		return 0u;
	}
	uData = (unsigned)p[B2977_EI_DATA];
	if (uData != B2977_ELFDATA2LSB && uData != B2977_ELFDATA2MSB) {
		return 0u;
	}
	return b2977_rd_u16(p + B2977_E_MACHINE_OFF, uData);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_elf_machine_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_elf_machine_u")));
