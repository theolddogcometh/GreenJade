/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7286: pipe2 flags mask soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipe2_mask_ok_7286(uint32_t flags);
 *     - Return 1 when flags has only known soft bits
 *       O_CLOEXEC|O_NONBLOCK|O_DIRECT; 0 if any other bit is set.
 *   uint32_t __gj_pipe2_mask_ok_7286  (alias)
 *   __libcgj_batch7286_marker = "libcgj-batch7286"
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

const char __libcgj_batch7286_marker[] = "libcgj-batch7286";

/* Soft known pipe2 bits: O_CLOEXEC | O_NONBLOCK | O_DIRECT. */
#define B7286_PIPE2_KNOWN  ((uint32_t)0x00084800u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7286_mask_ok(uint32_t u32Flags)
{
	if ((u32Flags & ~B7286_PIPE2_KNOWN) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipe2_mask_ok_7286 - soft known-pipe2-bits validity.
 *
 * flags: raw pipe2 flags word
 *
 * Returns 1 when only O_CLOEXEC|O_NONBLOCK|O_DIRECT may be set
 * (including zero), else 0. Pure integer. No parent wires.
 */
uint32_t
gj_pipe2_mask_ok_7286(uint32_t flags)
{
	(void)NULL;
	return b7286_mask_ok(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipe2_mask_ok_7286(uint32_t flags)
    __attribute__((alias("gj_pipe2_mask_ok_7286")));
