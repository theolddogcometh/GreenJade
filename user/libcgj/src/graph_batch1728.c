/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1728: Mach-O 64-bit magic exclusive.
 *
 * Surface (unique symbols):
 *   int gj_macho_is_64(const void *buf, size_t len);
 *     — Return 1 if the first 4 octets are MH_MAGIC_64 (0xfeedfacf)
 *       or MH_CIGAM_64 (0xcffaedfe) as on-disk LE host u32. Else 0.
 *       Does NOT accept 32-bit MH_* or FAT_* magics (stricter than
 *       gj_macho_is_magic). NULL / short → 0. Requires len >= 4.
 *   int __gj_macho_is_64  (alias)
 *   __libcgj_batch1728_marker = "libcgj-batch1728"
 *
 * Distinct from gj_macho_is_magic / gj_macho_is_fat — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1728_marker[] = "libcgj-batch1728";

/* On-disk 4-byte sequences interpreted as LE host u32. */
#define B1728_MH_MAGIC_64 0xfeedfacfu
#define B1728_MH_CIGAM_64 0xcffaedfeu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1728_rd_u32_le(const unsigned char *p)
{
	return (uint32_t)p[0] |
	       ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_macho_is_64 — 1 if first dword is MH_MAGIC_64 or MH_CIGAM_64.
 */
int
gj_macho_is_64(const void *pBuf, size_t cb)
{
	const unsigned char *p;
	uint32_t u32Mag;

	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	u32Mag = b1728_rd_u32_le(p);
	if (u32Mag == B1728_MH_MAGIC_64 || u32Mag == B1728_MH_CIGAM_64) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_macho_is_64(const void *pBuf, size_t cb)
    __attribute__((alias("gj_macho_is_64")));
