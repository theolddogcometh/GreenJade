/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6636: timerfd TFD_NONBLOCK soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tfd_flags_nonblock_ok_6636(uint32_t flags);
 *     - Soft check: 1 when flags has only soft create bits
 *       (TFD_NONBLOCK 0x800 and/or TFD_CLOEXEC 0x80000); else 0.
 *   uint32_t __gj_tfd_flags_nonblock_ok_6636  (alias)
 *   __libcgj_batch6636_marker = "libcgj-batch6636"
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

const char __libcgj_batch6636_marker[] = "libcgj-batch6636";

/* Soft TFD_NONBLOCK (O_NONBLOCK-style) bit. */
#define B6636_TFD_NONBLOCK  0x00000800u
/* Soft create-flags mask: CLOEXEC | NONBLOCK. */
#define B6636_TFD_CREATE_MASK  0x00080800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6636_nonblock_ok(uint32_t u32Flags)
{
	if ((u32Flags & ~B6636_TFD_CREATE_MASK) != 0u) {
		return 0u;
	}
	(void)B6636_TFD_NONBLOCK;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tfd_flags_nonblock_ok_6636 - soft timerfd NONBLOCK-side create ok.
 *
 * flags: soft timerfd_create flags word
 *
 * Returns 1 when flags uses only soft create bits, else 0.
 * Pure integer. No parent wires.
 */
uint32_t
gj_tfd_flags_nonblock_ok_6636(uint32_t flags)
{
	(void)NULL;
	return b6636_nonblock_ok(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tfd_flags_nonblock_ok_6636(uint32_t flags)
    __attribute__((alias("gj_tfd_flags_nonblock_ok_6636")));
