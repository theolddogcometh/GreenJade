/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6639: timerfd clockid+flags pack soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tfd_clock_flags_pack_6639(uint32_t clockid, uint32_t flags);
 *     - Soft key: (clockid & 0xff) | ((flags & 0x00ffffff) << 8).
 *       Pure fold; not a wire format.
 *   uint32_t __gj_tfd_clock_flags_pack_6639  (alias)
 *   __libcgj_batch6639_marker = "libcgj-batch6639"
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

const char __libcgj_batch6639_marker[] = "libcgj-batch6639";

#define B6639_CLOCK_MASK  0x000000ffu
#define B6639_FLAGS_MASK  0x00ffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6639_pack(uint32_t u32Clock, uint32_t u32Flags)
{
	return (u32Clock & B6639_CLOCK_MASK)
	    | ((u32Flags & B6639_FLAGS_MASK) << 8);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tfd_clock_flags_pack_6639 - soft timerfd clockid+flags key fold.
 *
 * clockid: soft clock identifier (low 8 bits used)
 * flags:   soft create flags (low 24 bits used, shifted up 8)
 *
 * Returns packed soft key. Pure integer. No parent wires.
 */
uint32_t
gj_tfd_clock_flags_pack_6639(uint32_t clockid, uint32_t flags)
{
	(void)NULL;
	return b6639_pack(clockid, flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tfd_clock_flags_pack_6639(uint32_t clockid, uint32_t flags)
    __attribute__((alias("gj_tfd_clock_flags_pack_6639")));
