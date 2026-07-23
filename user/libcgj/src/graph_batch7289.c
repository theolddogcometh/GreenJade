/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7289: pipe2 any-known-flag soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipe2_any_known_u_7289(uint32_t flags);
 *     - Return 1 when any of O_CLOEXEC|O_NONBLOCK|O_DIRECT is set; else 0.
 *   uint32_t __gj_pipe2_any_known_u_7289  (alias)
 *   __libcgj_batch7289_marker = "libcgj-batch7289"
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

const char __libcgj_batch7289_marker[] = "libcgj-batch7289";

/* Soft known pipe2 bits: O_CLOEXEC | O_NONBLOCK | O_DIRECT. */
#define B7289_PIPE2_KNOWN  ((uint32_t)0x00084800u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7289_any_known(uint32_t u32Flags)
{
	if ((u32Flags & B7289_PIPE2_KNOWN) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipe2_any_known_u_7289 - soft any-known-pipe2-flag presence test.
 *
 * flags: raw pipe2 flags word
 *
 * Returns 1 when any of O_CLOEXEC|O_NONBLOCK|O_DIRECT is set, else 0.
 * Pure integer. No parent wires.
 */
uint32_t
gj_pipe2_any_known_u_7289(uint32_t flags)
{
	(void)NULL;
	return b7289_any_known(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipe2_any_known_u_7289(uint32_t flags)
    __attribute__((alias("gj_pipe2_any_known_u_7289")));
