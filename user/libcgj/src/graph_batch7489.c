/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7489: mknodat file-type validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mknodat_type_ok_u_7489(uint32_t mode);
 *     - Return 1 if (mode & S_IFMT) is a known S_IF* type, else 0.
 *   uint32_t __gj_mknodat_type_ok_u_7489  (alias)
 *   __libcgj_batch7489_marker = "libcgj-batch7489"
 *
 * Exclusive continuum CREATE-ONLY (7481-7490: mknodat type stubs —
 * s_ififo_id, s_ifchr_id, s_ifdir_id, s_ifblk_id, s_ifreg_id,
 * s_iflnk_id, s_ifsock_id, s_ifmt_id, type_ok, batch_id_7490).
 * Unique gj_mknodat_type_ok_u_7489 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7489_marker[] = "libcgj-batch7489";

/*
 * Soft catalog type bits (mode & S_IFMT):
 *   IFIFO=0x1000 IFCHR=0x2000 IFDIR=0x4000 IFBLK=0x6000
 *   IFREG=0x8000 IFLNK=0xA000 IFSOCK=0xC000; mask S_IFMT=0xF000
 */
#define B7489_S_IFMT   ((uint32_t)0xF000u)
#define B7489_S_IFIFO  ((uint32_t)0x1000u)
#define B7489_S_IFCHR  ((uint32_t)0x2000u)
#define B7489_S_IFDIR  ((uint32_t)0x4000u)
#define B7489_S_IFBLK  ((uint32_t)0x6000u)
#define B7489_S_IFREG  ((uint32_t)0x8000u)
#define B7489_S_IFLNK  ((uint32_t)0xA000u)
#define B7489_S_IFSOCK ((uint32_t)0xC000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7489_mknodat_type_ok(uint32_t u32Mode)
{
	uint32_t u32Type;

	u32Type = u32Mode & B7489_S_IFMT;
	if (u32Type == B7489_S_IFIFO ||
	    u32Type == B7489_S_IFCHR ||
	    u32Type == B7489_S_IFDIR ||
	    u32Type == B7489_S_IFBLK ||
	    u32Type == B7489_S_IFREG ||
	    u32Type == B7489_S_IFLNK ||
	    u32Type == B7489_S_IFSOCK)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mknodat_type_ok_u_7489 - 1 if mode carries a known S_IF* type.
 *
 * mode: mknodat(2) / stat mode argument (type bits in high nibble)
 *
 * Returns 1 when (mode & 0xF000) is one of
 * {IFIFO,IFCHR,IFDIR,IFBLK,IFREG,IFLNK,IFSOCK}, else 0. Soft catalog
 * check; does not call mknodat. No parent wires.
 */
uint32_t
gj_mknodat_type_ok_u_7489(uint32_t u32Mode)
{
	(void)NULL;
	return b7489_mknodat_type_ok(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mknodat_type_ok_u_7489(uint32_t u32Mode)
    __attribute__((alias("gj_mknodat_type_ok_u_7489")));
