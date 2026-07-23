/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7417: open O_APPEND flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_open_append_u_7417(uint32_t flags);
 *     - Return 1 when O_APPEND (0x400) is set in flags; else 0.
 *   uint32_t __gj_open_append_u_7417  (alias)
 *   __libcgj_batch7417_marker = "libcgj-batch7417"
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

const char __libcgj_batch7417_marker[] = "libcgj-batch7417";

/* Soft O_APPEND (Linux/glibc-shaped open). */
#define B7417_O_APPEND  ((uint32_t)0x00000400u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7417_append(uint32_t u32Flags)
{
	if ((u32Flags & B7417_O_APPEND) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_open_append_u_7417 - soft O_APPEND presence test.
 *
 * flags: raw open oflag word
 *
 * Returns 1 when O_APPEND (0x400) is set, else 0. Pure mask.
 * No parent wires.
 */
uint32_t
gj_open_append_u_7417(uint32_t flags)
{
	(void)NULL;
	return b7417_append(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_open_append_u_7417(uint32_t flags)
    __attribute__((alias("gj_open_append_u_7417")));
