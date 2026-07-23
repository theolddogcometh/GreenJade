/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7413: open O_RDWR access-mode stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_open_rdwr_u_7413(uint32_t flags);
 *     - Return 1 when (flags & O_ACCMODE) == O_RDWR (2); else 0.
 *   uint32_t __gj_open_rdwr_u_7413  (alias)
 *   __libcgj_batch7413_marker = "libcgj-batch7413"
 *
 * Exclusive continuum CREATE-ONLY (7411-7420: open flag O_* stubs —
 * rdonly, wronly, rdwr, creat, excl, trunc, append, cloexec, pack,
 * flags_ready). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7413_marker[] = "libcgj-batch7413";

/* Soft open access-mode bits (Linux/glibc-shaped). */
#define B7413_O_RDWR     ((uint32_t)0x2u)
#define B7413_O_ACCMODE  ((uint32_t)0x3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7413_rdwr(uint32_t u32Flags)
{
	if ((u32Flags & B7413_O_ACCMODE) == B7413_O_RDWR) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_open_rdwr_u_7413 - soft O_RDWR access-mode test.
 *
 * flags: raw open oflag word
 *
 * Returns 1 when (flags & O_ACCMODE) == O_RDWR, else 0. Pure mask.
 * No parent wires.
 */
uint32_t
gj_open_rdwr_u_7413(uint32_t flags)
{
	(void)NULL;
	return b7413_rdwr(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_open_rdwr_u_7413(uint32_t flags)
    __attribute__((alias("gj_open_rdwr_u_7413")));
