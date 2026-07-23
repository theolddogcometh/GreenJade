/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7285: pipe2 flags pack soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipe2_pack_u_7285(uint32_t want_cloexec, uint32_t want_nonblock,
 *                                 uint32_t want_direct);
 *     - Soft pack: non-zero want_cloexec → O_CLOEXEC, want_nonblock →
 *       O_NONBLOCK, want_direct → O_DIRECT; OR the selected bits.
 *   uint32_t __gj_pipe2_pack_u_7285  (alias)
 *   __libcgj_batch7285_marker = "libcgj-batch7285"
 *
 * Exclusive continuum CREATE-ONLY (7281-7290: pipe2 flag stubs —
 * cloexec, nonblock, direct, zero_ok, pack, mask_ok, combo, strip,
 * any_known, flags_ready). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7285_marker[] = "libcgj-batch7285";

#define B7285_O_CLOEXEC   ((uint32_t)0x00080000u)
#define B7285_O_NONBLOCK  ((uint32_t)0x00000800u)
#define B7285_O_DIRECT    ((uint32_t)0x00004000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7285_pack(uint32_t u32Cloexec, uint32_t u32Nonblock, uint32_t u32Direct)
{
	uint32_t u32Out = 0u;

	if (u32Cloexec != 0u) {
		u32Out |= B7285_O_CLOEXEC;
	}
	if (u32Nonblock != 0u) {
		u32Out |= B7285_O_NONBLOCK;
	}
	if (u32Direct != 0u) {
		u32Out |= B7285_O_DIRECT;
	}
	return u32Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipe2_pack_u_7285 - soft pack of cloexec/nonblock/direct request bits.
 *
 * want_cloexec:  non-zero to include O_CLOEXEC
 * want_nonblock: non-zero to include O_NONBLOCK
 * want_direct:   non-zero to include O_DIRECT
 *
 * Returns packed flags word. Pure integer. No parent wires.
 */
uint32_t
gj_pipe2_pack_u_7285(uint32_t want_cloexec, uint32_t want_nonblock,
    uint32_t want_direct)
{
	(void)NULL;
	return b7285_pack(want_cloexec, want_nonblock, want_direct);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipe2_pack_u_7285(uint32_t want_cloexec, uint32_t want_nonblock,
    uint32_t want_direct)
    __attribute__((alias("gj_pipe2_pack_u_7285")));
