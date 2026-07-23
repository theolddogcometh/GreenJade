/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1146: Mach-O / universal (FAT) magic check.
 *
 * Surface (unique symbols):
 *   int gj_macho_is_magic(const void *buf, size_t len);
 *     — Return 1 (OK) if the first 4 octets match a known Mach-O or
 *       FAT magic (host LE load of u32 compared against standard
 *       MH_* / FAT_* constants and their byte-swapped forms); else 0.
 *       NULL / short → 0.
 *   int __gj_macho_is_magic  (alias)
 *   __libcgj_batch1146_marker = "libcgj-batch1146"
 *
 * Magics accepted (as little-endian host u32 values of on-disk octets):
 *   MH_MAGIC 0xfeedface, MH_CIGAM 0xcefaedfe,
 *   MH_MAGIC_64 0xfeedfacf, MH_CIGAM_64 0xcffaedfe,
 *   FAT_MAGIC 0xcafebabe, FAT_CIGAM 0xbebafeca.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1146_marker[] = "libcgj-batch1146";

/* On-disk 4-byte sequences interpreted as LE host u32. */
#define B1146_MH_MAGIC     0xfeedfaceu
#define B1146_MH_CIGAM     0xcefaedfeu
#define B1146_MH_MAGIC_64  0xfeedfacfu
#define B1146_MH_CIGAM_64  0xcffaedfeu
#define B1146_FAT_MAGIC    0xcafebabeu
#define B1146_FAT_CIGAM    0xbebafecau

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_macho_is_magic — 1 if first dword is a known Mach-O/FAT magic.
 */
int
gj_macho_is_magic(const void *pBuf, size_t cb)
{
	const unsigned char *p;
	uint32_t u32Mag;

	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	/* Load on-disk octets as LE host word (matches standard macros). */
	u32Mag = (uint32_t)p[0] |
	         ((uint32_t)p[1] << 8) |
	         ((uint32_t)p[2] << 16) |
	         ((uint32_t)p[3] << 24);
	if (u32Mag == B1146_MH_MAGIC || u32Mag == B1146_MH_CIGAM ||
	    u32Mag == B1146_MH_MAGIC_64 || u32Mag == B1146_MH_CIGAM_64 ||
	    u32Mag == B1146_FAT_MAGIC || u32Mag == B1146_FAT_CIGAM) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_macho_is_magic(const void *pBuf, size_t cb)
    __attribute__((alias("gj_macho_is_magic")));
