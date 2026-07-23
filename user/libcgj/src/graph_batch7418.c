/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7418: open O_CLOEXEC flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_open_cloexec_u_7418(uint32_t flags);
 *     - Return 1 when O_CLOEXEC (0x80000) is set in flags; else 0.
 *   uint32_t __gj_open_cloexec_u_7418  (alias)
 *   __libcgj_batch7418_marker = "libcgj-batch7418"
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

const char __libcgj_batch7418_marker[] = "libcgj-batch7418";

/* Soft O_CLOEXEC (Linux/glibc-shaped open). */
#define B7418_O_CLOEXEC  ((uint32_t)0x00080000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7418_cloexec(uint32_t u32Flags)
{
	if ((u32Flags & B7418_O_CLOEXEC) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_open_cloexec_u_7418 - soft O_CLOEXEC presence test.
 *
 * flags: raw open oflag word
 *
 * Returns 1 when O_CLOEXEC (0x80000) is set, else 0. Pure mask.
 * No parent wires.
 */
uint32_t
gj_open_cloexec_u_7418(uint32_t flags)
{
	(void)NULL;
	return b7418_cloexec(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_open_cloexec_u_7418(uint32_t flags)
    __attribute__((alias("gj_open_cloexec_u_7418")));
