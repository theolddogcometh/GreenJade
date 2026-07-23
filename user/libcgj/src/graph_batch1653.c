/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1653: Mach-O / FAT magic bytes (lightweight).
 *
 * Surface (unique symbols):
 *   int gj_magic_is_macho_bytes(const void *buf, size_t len);
 *     — Return 1 (OK) if the first 4 octets match a known Mach-O or
 *       FAT magic (host LE load of u32 vs MH_* / FAT_* and byte-swapped
 *       forms); else 0. NULL / short → 0.
 *   int __gj_magic_is_macho_bytes  (alias)
 *   __libcgj_batch1653_marker = "libcgj-batch1653"
 *
 * Magics accepted (LE host u32 of on-disk octets):
 *   MH_MAGIC 0xfeedface, MH_CIGAM 0xcefaedfe,
 *   MH_MAGIC_64 0xfeedfacf, MH_CIGAM_64 0xcffaedfe,
 *   FAT_MAGIC 0xcafebabe, FAT_CIGAM 0xbebafeca.
 *
 * Distinct from gj_macho_is_magic (batch1146) — avoid multi-def.
 * Exclusive file-magic wave (1651–1659).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1653_marker[] = "libcgj-batch1653";

#define B1653_MH_MAGIC     0xfeedfaceu
#define B1653_MH_CIGAM     0xcefaedfeu
#define B1653_MH_MAGIC_64  0xfeedfacfu
#define B1653_MH_CIGAM_64  0xcffaedfeu
#define B1653_FAT_MAGIC    0xcafebabeu
#define B1653_FAT_CIGAM    0xbebafecau

/* ---- freestanding helpers ---------------------------------------------- */

/* Load 4 on-disk octets as little-endian host u32. */
static uint32_t
b1653_load_le32(const unsigned char *p)
{
	return (uint32_t)p[0] |
	       ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

/* True if u32Mag is a known Mach-O or FAT magic. */
static int
b1653_is_known(uint32_t u32Mag)
{
	if (u32Mag == B1653_MH_MAGIC || u32Mag == B1653_MH_CIGAM ||
	    u32Mag == B1653_MH_MAGIC_64 || u32Mag == B1653_MH_CIGAM_64 ||
	    u32Mag == B1653_FAT_MAGIC || u32Mag == B1653_FAT_CIGAM) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_magic_is_macho_bytes — 1 if first dword is a known Mach-O/FAT magic.
 */
int
gj_magic_is_macho_bytes(const void *pBuf, size_t cb)
{
	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	return b1653_is_known(b1653_load_le32((const unsigned char *)pBuf));
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_magic_is_macho_bytes(const void *pBuf, size_t cb)
    __attribute__((alias("gj_magic_is_macho_bytes")));
