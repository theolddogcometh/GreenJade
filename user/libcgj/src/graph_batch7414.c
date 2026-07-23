/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7414: open O_CREAT flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_open_creat_u_7414(uint32_t flags);
 *     - Return 1 when O_CREAT (0x40) is set in flags; else 0.
 *   uint32_t __gj_open_creat_u_7414  (alias)
 *   __libcgj_batch7414_marker = "libcgj-batch7414"
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

const char __libcgj_batch7414_marker[] = "libcgj-batch7414";

/* Soft O_CREAT (Linux/glibc-shaped open). */
#define B7414_O_CREAT  ((uint32_t)0x00000040u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7414_creat(uint32_t u32Flags)
{
	if ((u32Flags & B7414_O_CREAT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_open_creat_u_7414 - soft O_CREAT presence test.
 *
 * flags: raw open oflag word
 *
 * Returns 1 when O_CREAT (0x40) is set, else 0. Pure mask.
 * No parent wires.
 */
uint32_t
gj_open_creat_u_7414(uint32_t flags)
{
	(void)NULL;
	return b7414_creat(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_open_creat_u_7414(uint32_t flags)
    __attribute__((alias("gj_open_creat_u_7414")));
