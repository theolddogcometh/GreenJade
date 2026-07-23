/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7281: pipe2 O_CLOEXEC flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipe2_cloexec_u_7281(uint32_t flags);
 *     - Return 1 when O_CLOEXEC (0x80000) is set in flags; else 0.
 *   uint32_t __gj_pipe2_cloexec_u_7281  (alias)
 *   __libcgj_batch7281_marker = "libcgj-batch7281"
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

const char __libcgj_batch7281_marker[] = "libcgj-batch7281";

/* Soft O_CLOEXEC (Linux/glibc-shaped, pipe2). */
#define B7281_O_CLOEXEC  ((uint32_t)0x00080000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7281_cloexec(uint32_t u32Flags)
{
	if ((u32Flags & B7281_O_CLOEXEC) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipe2_cloexec_u_7281 - soft O_CLOEXEC presence test.
 *
 * flags: raw pipe2 flags word
 *
 * Returns 1 when O_CLOEXEC (0x80000) is set, else 0. Pure mask.
 * No parent wires.
 */
uint32_t
gj_pipe2_cloexec_u_7281(uint32_t flags)
{
	(void)NULL;
	return b7281_cloexec(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipe2_cloexec_u_7281(uint32_t flags)
    __attribute__((alias("gj_pipe2_cloexec_u_7281")));
