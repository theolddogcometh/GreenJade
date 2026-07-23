/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7411: open O_RDONLY access-mode stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_open_rdonly_u_7411(uint32_t flags);
 *     - Return 1 when (flags & O_ACCMODE) == O_RDONLY (0); else 0.
 *   uint32_t __gj_open_rdonly_u_7411  (alias)
 *   __libcgj_batch7411_marker = "libcgj-batch7411"
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

const char __libcgj_batch7411_marker[] = "libcgj-batch7411";

/* Soft open access-mode bits (Linux/glibc-shaped). */
#define B7411_O_RDONLY   ((uint32_t)0x0u)
#define B7411_O_ACCMODE  ((uint32_t)0x3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7411_rdonly(uint32_t u32Flags)
{
	if ((u32Flags & B7411_O_ACCMODE) == B7411_O_RDONLY) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_open_rdonly_u_7411 - soft O_RDONLY access-mode test.
 *
 * flags: raw open oflag word
 *
 * Returns 1 when (flags & O_ACCMODE) == O_RDONLY, else 0. Pure mask.
 * No parent wires.
 */
uint32_t
gj_open_rdonly_u_7411(uint32_t flags)
{
	(void)NULL;
	return b7411_rdonly(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_open_rdonly_u_7411(uint32_t flags)
    __attribute__((alias("gj_open_rdonly_u_7411")));
