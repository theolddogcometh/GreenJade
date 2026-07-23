/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7287: pipe2 O_CLOEXEC|O_NONBLOCK combo stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipe2_combo_u_7287(uint32_t flags);
 *     - Return 1 when both O_CLOEXEC and O_NONBLOCK are set; else 0.
 *   uint32_t __gj_pipe2_combo_u_7287  (alias)
 *   __libcgj_batch7287_marker = "libcgj-batch7287"
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

const char __libcgj_batch7287_marker[] = "libcgj-batch7287";

#define B7287_O_CLOEXEC   ((uint32_t)0x00080000u)
#define B7287_O_NONBLOCK  ((uint32_t)0x00000800u)
#define B7287_COMBO       ((uint32_t)0x00080800u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7287_combo(uint32_t u32Flags)
{
	/* Both cloexec and nonblock must be present. */
	if ((u32Flags & B7287_COMBO) == B7287_COMBO) {
		return 1u;
	}
	(void)B7287_O_CLOEXEC;
	(void)B7287_O_NONBLOCK;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipe2_combo_u_7287 - soft O_CLOEXEC|O_NONBLOCK both-set test.
 *
 * flags: raw pipe2 flags word
 *
 * Returns 1 when both O_CLOEXEC and O_NONBLOCK are set, else 0.
 * Self-contained; does not call cloexec/nonblock stubs. No parent wires.
 */
uint32_t
gj_pipe2_combo_u_7287(uint32_t flags)
{
	(void)NULL;
	return b7287_combo(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipe2_combo_u_7287(uint32_t flags)
    __attribute__((alias("gj_pipe2_combo_u_7287")));
