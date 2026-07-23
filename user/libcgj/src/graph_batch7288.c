/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7288: pipe2 flags strip soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipe2_strip_u_7288(uint32_t flags);
 *     - Return flags masked to known soft bits
 *       O_CLOEXEC|O_NONBLOCK|O_DIRECT (unknown bits cleared).
 *   uint32_t __gj_pipe2_strip_u_7288  (alias)
 *   __libcgj_batch7288_marker = "libcgj-batch7288"
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

const char __libcgj_batch7288_marker[] = "libcgj-batch7288";

/* Soft known pipe2 bits: O_CLOEXEC | O_NONBLOCK | O_DIRECT. */
#define B7288_PIPE2_KNOWN  ((uint32_t)0x00084800u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7288_strip(uint32_t u32Flags)
{
	return u32Flags & B7288_PIPE2_KNOWN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipe2_strip_u_7288 - soft strip of unknown pipe2 flag bits.
 *
 * flags: raw pipe2 flags word
 *
 * Returns flags & (O_CLOEXEC|O_NONBLOCK|O_DIRECT). Pure integer.
 * No parent wires.
 */
uint32_t
gj_pipe2_strip_u_7288(uint32_t flags)
{
	(void)NULL;
	return b7288_strip(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipe2_strip_u_7288(uint32_t flags)
    __attribute__((alias("gj_pipe2_strip_u_7288")));
