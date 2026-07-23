/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6637: timerfd create flags pack soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tfd_flags_pack_6637(uint32_t want_cloexec,
 *                                   uint32_t want_nonblock);
 *     - Soft pack: OR of TFD_CLOEXEC (0x80000) when want_cloexec != 0
 *       and TFD_NONBLOCK (0x800) when want_nonblock != 0.
 *   uint32_t __gj_tfd_flags_pack_6637  (alias)
 *   __libcgj_batch6637_marker = "libcgj-batch6637"
 *
 * Exclusive continuum CREATE-ONLY (6631-6640: timerfd clock id stubs —
 * clock_id_ok_6631, realtime_p_6632, monotonic_p_6633, boottime_p_6634,
 * cloexec_ok_6635, nonblock_ok_6636, flags_pack_6637, abstime_p_6638,
 * clock_flags_pack_6639, clock_ready / batch_id_6640). Unique surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6637_marker[] = "libcgj-batch6637";

#define B6637_TFD_CLOEXEC   0x00080000u
#define B6637_TFD_NONBLOCK  0x00000800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6637_flags_pack(uint32_t u32Cloexec, uint32_t u32Nonblock)
{
	uint32_t u32Out;

	u32Out = 0u;
	if (u32Cloexec != 0u) {
		u32Out |= B6637_TFD_CLOEXEC;
	}
	if (u32Nonblock != 0u) {
		u32Out |= B6637_TFD_NONBLOCK;
	}
	return u32Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tfd_flags_pack_6637 - soft timerfd_create flags fold.
 *
 * want_cloexec:  non-zero → include soft TFD_CLOEXEC
 * want_nonblock: non-zero → include soft TFD_NONBLOCK
 *
 * Returns packed soft flags word. Pure integer. No parent wires.
 */
uint32_t
gj_tfd_flags_pack_6637(uint32_t want_cloexec, uint32_t want_nonblock)
{
	(void)NULL;
	return b6637_flags_pack(want_cloexec, want_nonblock);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tfd_flags_pack_6637(uint32_t want_cloexec, uint32_t want_nonblock)
    __attribute__((alias("gj_tfd_flags_pack_6637")));
