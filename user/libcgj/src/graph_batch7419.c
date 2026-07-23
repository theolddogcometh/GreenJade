/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7419: open flags pack soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_open_pack_u_7419(uint32_t want_creat, uint32_t want_excl,
 *                                uint32_t want_trunc, uint32_t want_append);
 *     - Soft pack: non-zero want_creat → O_CREAT, want_excl → O_EXCL,
 *       want_trunc → O_TRUNC, want_append → O_APPEND; OR selected bits.
 *   uint32_t __gj_open_pack_u_7419  (alias)
 *   __libcgj_batch7419_marker = "libcgj-batch7419"
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

const char __libcgj_batch7419_marker[] = "libcgj-batch7419";

#define B7419_O_CREAT   ((uint32_t)0x00000040u)
#define B7419_O_EXCL    ((uint32_t)0x00000080u)
#define B7419_O_TRUNC   ((uint32_t)0x00000200u)
#define B7419_O_APPEND  ((uint32_t)0x00000400u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7419_pack(uint32_t u32Creat, uint32_t u32Excl, uint32_t u32Trunc,
    uint32_t u32Append)
{
	uint32_t u32Out = 0u;

	if (u32Creat != 0u) {
		u32Out |= B7419_O_CREAT;
	}
	if (u32Excl != 0u) {
		u32Out |= B7419_O_EXCL;
	}
	if (u32Trunc != 0u) {
		u32Out |= B7419_O_TRUNC;
	}
	if (u32Append != 0u) {
		u32Out |= B7419_O_APPEND;
	}
	return u32Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_open_pack_u_7419 - soft pack of creat/excl/trunc/append request bits.
 *
 * want_creat:  non-zero to include O_CREAT
 * want_excl:   non-zero to include O_EXCL
 * want_trunc:  non-zero to include O_TRUNC
 * want_append: non-zero to include O_APPEND
 *
 * Returns packed oflag word. Pure integer. No parent wires.
 */
uint32_t
gj_open_pack_u_7419(uint32_t want_creat, uint32_t want_excl,
    uint32_t want_trunc, uint32_t want_append)
{
	(void)NULL;
	return b7419_pack(want_creat, want_excl, want_trunc, want_append);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_open_pack_u_7419(uint32_t want_creat, uint32_t want_excl,
    uint32_t want_trunc, uint32_t want_append)
    __attribute__((alias("gj_open_pack_u_7419")));
