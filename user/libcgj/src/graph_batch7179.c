/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7179: dlopen flags errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtld_flags_errorish_u_7179(uint32_t flags);
 *     - Return 1 if flags has any unknown bits outside known RTLD_*
 *       mask (0x103), else 0.
 *   uint32_t __gj_rtld_flags_errorish_u_7179  (alias)
 *   __libcgj_batch7179_marker = "libcgj-batch7179"
 *
 * Exclusive continuum CREATE-ONLY (7171-7180: dlopen flag stubs —
 * lazy_id, now_id, global_id, has_lazy, has_now, has_global,
 * flags_ok, flags_pack, flags_errorish, batch_id_7180).
 * Unique gj_rtld_flags_errorish_u_7179 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7179_marker[] = "libcgj-batch7179";

/*
 * Known dlopen mode bits for this continuum:
 *   RTLD_LAZY | RTLD_NOW | RTLD_GLOBAL = 0x103
 */
#define B7179_RTLD_KNOWN_MASK ((uint32_t)0x103u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7179_rtld_flags_errorish(uint32_t u32Flags)
{
	return ((u32Flags & ~B7179_RTLD_KNOWN_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtld_flags_errorish_u_7179 - 1 if flags has unknown bits.
 *
 * flags: dlopen(3) mode argument
 *
 * Returns 1 when any bit outside {0x1, 0x2, 0x100} is set, else 0.
 * Soft inverse of flags_ok; does not call dlopen. No parent wires.
 */
uint32_t
gj_rtld_flags_errorish_u_7179(uint32_t u32Flags)
{
	(void)NULL;
	return b7179_rtld_flags_errorish(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtld_flags_errorish_u_7179(uint32_t u32Flags)
    __attribute__((alias("gj_rtld_flags_errorish_u_7179")));
