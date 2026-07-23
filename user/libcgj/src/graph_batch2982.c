/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2982: bare COFF object header sniff (_u).
 *
 * Surface (unique symbols):
 *   int gj_coff_is_coff_u(const void *buf, size_t len);
 *     - Return 1 if buf looks like a standalone COFF object:
 *       IMAGE_FILE_HEADER (20 octets) with a known Machine, a sane
 *       NumberOfSections (1..96), and SizeOfOptionalHeader == 0.
 *       Else 0. NULL / short → 0.
 *   int __gj_coff_is_coff_u  (alias)
 *   __libcgj_batch2982_marker = "libcgj-batch2982"
 *
 * Exclusive continuum CREATE-ONLY (2981-2990). Distinct from PE
 * surfaces (gj_pe_is_pe / gj_pe_is_pe_u) — bare COFF, not MZ/PE.
 * Unique name; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2982_marker[] = "libcgj-batch2982";

/* IMAGE_FILE_HEADER size. */
#define B2982_COFF_HDR  20u
/* Practical upper bound on NumberOfSections for a single object. */
#define B2982_SEC_MAX   96u

/* Common COFF Machine values (LE host). */
#define B2982_I386      0x014cu
#define B2982_AMD64     0x8664u
#define B2982_ARMNT     0x01c4u
#define B2982_ARM64     0xaa64u
#define B2982_IA64      0x0200u
#define B2982_EBC       0x0ebcu
#define B2982_R4000     0x0166u

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b2982_rd_u16_le(const unsigned char *p)
{
	return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

/* 1 if u16Mach is a known COFF Machine code. */
static int
b2982_known_machine(uint16_t u16Mach)
{
	if (u16Mach == B2982_I386 || u16Mach == B2982_AMD64 ||
	    u16Mach == B2982_ARMNT || u16Mach == B2982_ARM64 ||
	    u16Mach == B2982_IA64 || u16Mach == B2982_EBC ||
	    u16Mach == B2982_R4000) {
		return 1;
	}
	return 0;
}

/*
 * Bare COFF object sniff: Machine known, sections in 1..96,
 * SizeOfOptionalHeader == 0 (no PE optional header).
 */
static int
b2982_is_coff(const unsigned char *p, size_t cb)
{
	uint16_t u16Mach;
	uint16_t u16Secs;
	uint16_t u16Opt;

	if (cb < B2982_COFF_HDR) {
		return 0;
	}
	u16Mach = b2982_rd_u16_le(p + 0);
	u16Secs = b2982_rd_u16_le(p + 2);
	u16Opt = b2982_rd_u16_le(p + 16);
	if (b2982_known_machine(u16Mach) == 0) {
		return 0;
	}
	if (u16Secs == 0u || u16Secs > B2982_SEC_MAX) {
		return 0;
	}
	if (u16Opt != 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_coff_is_coff_u - 1 if buf is a plausible bare COFF object header.
 *
 * Does not accept MZ/PE images (those begin with "MZ", not a Machine).
 */
int
gj_coff_is_coff_u(const void *pBuf, size_t cb)
{
	(void)NULL;
	if (pBuf == NULL) {
		return 0;
	}
	return b2982_is_coff((const unsigned char *)pBuf, cb);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_coff_is_coff_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_coff_is_coff_u")));
