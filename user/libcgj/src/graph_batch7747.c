/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7747: linux_dirent d_reclen align stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dirent_reclen_align_u_7747(uint32_t bytes);
 *     - Round bytes up to the next multiple of 8 (soft d_reclen
 *       alignment). Zero stays zero; overflow saturates to UINT32_MAX
 *       rounded down to multiple of 8.
 *   uint32_t __gj_dirent_reclen_align_u_7747  (alias)
 *   __libcgj_batch7747_marker = "libcgj-batch7747"
 *
 * Exclusive continuum CREATE-ONLY (7741-7750: getdents dirent stubs —
 * root, getdents_nr, getdents64_nr, dt_dir, dt_reg, dt_ok, reclen_align,
 * reclen_min, dt_errorish, continuum + batch_id_7750). Unique
 * gj_dirent_reclen_align_u_7747 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7747_marker[] = "libcgj-batch7747";

/* Soft d_reclen alignment quantum (linux_dirent* pads to 8). */
#define B7747_ALIGN  ((uint32_t)8u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7747_reclen_align(uint32_t u32Bytes)
{
	uint32_t u32Rem;

	if (u32Bytes == 0u) {
		return 0u;
	}
	u32Rem = u32Bytes % B7747_ALIGN;
	if (u32Rem == 0u) {
		return u32Bytes;
	}
	/* Saturate if add would wrap. */
	if (u32Bytes > (0xFFFFFFFFu - (B7747_ALIGN - u32Rem))) {
		return 0xFFFFFFF8u;
	}
	return u32Bytes + (B7747_ALIGN - u32Rem);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dirent_reclen_align_u_7747 - soft 8-byte d_reclen alignment.
 *
 * bytes: unaligned record size before padding
 *
 * Returns ceil_align8(bytes). Pure integer; does not call getdents.
 * No parent wires.
 */
uint32_t
gj_dirent_reclen_align_u_7747(uint32_t u32Bytes)
{
	(void)NULL;
	return b7747_reclen_align(u32Bytes);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dirent_reclen_align_u_7747(uint32_t u32Bytes)
    __attribute__((alias("gj_dirent_reclen_align_u_7747")));
