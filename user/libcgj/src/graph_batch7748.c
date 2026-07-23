/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7748: linux_dirent64 min d_reclen stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dirent_reclen_min_u_7748(uint32_t namelen);
 *     - Soft min d_reclen for a linux_dirent64 entry with namelen
 *       name bytes (excluding NUL): align8(19 + namelen + 1).
 *       Fixed header is 19 bytes (ino8+off8+reclen2+type1).
 *   uint32_t __gj_dirent_reclen_min_u_7748  (alias)
 *   __libcgj_batch7748_marker = "libcgj-batch7748"
 *
 * Exclusive continuum CREATE-ONLY (7741-7750: getdents dirent stubs —
 * root, getdents_nr, getdents64_nr, dt_dir, dt_reg, dt_ok, reclen_align,
 * reclen_min, dt_errorish, continuum + batch_id_7750). Unique
 * gj_dirent_reclen_min_u_7748 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7748_marker[] = "libcgj-batch7748";

/* linux_dirent64 fixed prefix before d_name[] (8+8+2+1). */
#define B7748_HDR     ((uint32_t)19u)
#define B7748_ALIGN   ((uint32_t)8u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7748_align8(uint32_t u32Bytes)
{
	uint32_t u32Rem;

	if (u32Bytes == 0u) {
		return 0u;
	}
	u32Rem = u32Bytes % B7748_ALIGN;
	if (u32Rem == 0u) {
		return u32Bytes;
	}
	if (u32Bytes > (0xFFFFFFFFu - (B7748_ALIGN - u32Rem))) {
		return 0xFFFFFFF8u;
	}
	return u32Bytes + (B7748_ALIGN - u32Rem);
}

static uint32_t
b7748_reclen_min(uint32_t u32Namelen)
{
	uint32_t u32Raw;

	/* namelen + NUL + header; guard wrap on namelen. */
	if (u32Namelen > (0xFFFFFFFFu - (B7748_HDR + 1u))) {
		return 0xFFFFFFF8u;
	}
	u32Raw = B7748_HDR + u32Namelen + 1u;
	return b7748_align8(u32Raw);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dirent_reclen_min_u_7748 - soft min d_reclen for namelen.
 *
 * namelen: name length in bytes excluding the terminating NUL
 *
 * Returns align8(19 + namelen + 1). Soft layout math only; does not
 * call getdents. No parent wires.
 */
uint32_t
gj_dirent_reclen_min_u_7748(uint32_t u32Namelen)
{
	(void)NULL;
	return b7748_reclen_min(u32Namelen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dirent_reclen_min_u_7748(uint32_t u32Namelen)
    __attribute__((alias("gj_dirent_reclen_min_u_7748")));
