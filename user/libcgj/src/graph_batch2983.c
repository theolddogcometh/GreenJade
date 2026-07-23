/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2983: Mach-O thin MH_* magic check (_u).
 *
 * Surface (unique symbols):
 *   int gj_macho_is_mh_u(const void *buf, size_t len);
 *     - Return 1 if the first 4 octets are a thin Mach-O MH magic:
 *       MH_MAGIC, MH_CIGAM, MH_MAGIC_64, or MH_CIGAM_64 (LE host u32
 *       of on-disk octets). Does NOT accept FAT_* universal magics.
 *       NULL / short → 0. Requires len >= 4.
 *   int __gj_macho_is_mh_u  (alias)
 *   __libcgj_batch2983_marker = "libcgj-batch2983"
 *
 * Exclusive continuum CREATE-ONLY (2981-2990). Distinct from
 * gj_macho_is_magic (batch1146, includes FAT) and gj_macho_is_64
 * (batch1728) — unique gj_macho_is_mh_u; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2983_marker[] = "libcgj-batch2983";

/* On-disk 4-byte sequences interpreted as LE host u32. Thin MH only. */
#define B2983_MH_MAGIC     0xfeedfaceu
#define B2983_MH_CIGAM     0xcefaedfeu
#define B2983_MH_MAGIC_64  0xfeedfacfu
#define B2983_MH_CIGAM_64  0xcffaedfeu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2983_rd_u32_le(const unsigned char *p)
{
	return (uint32_t)p[0] |
	       ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

/* 1 if u32Mag is a thin MH_* magic (not FAT). */
static int
b2983_is_mh(uint32_t u32Mag)
{
	if (u32Mag == B2983_MH_MAGIC || u32Mag == B2983_MH_CIGAM ||
	    u32Mag == B2983_MH_MAGIC_64 || u32Mag == B2983_MH_CIGAM_64) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_macho_is_mh_u - 1 if first dword is a thin Mach-O MH magic.
 *
 * FAT_MAGIC / FAT_CIGAM are intentionally rejected.
 */
int
gj_macho_is_mh_u(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	(void)NULL;
	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	return b2983_is_mh(b2983_rd_u32_le(p));
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_macho_is_mh_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_macho_is_mh_u")));
