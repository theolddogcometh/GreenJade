/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7176: dlopen RTLD_GLOBAL presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtld_has_global_u_7176(uint32_t flags);
 *     - Return 1 if flags has RTLD_GLOBAL (0x100) set, else 0.
 *   uint32_t __gj_rtld_has_global_u_7176  (alias)
 *   __libcgj_batch7176_marker = "libcgj-batch7176"
 *
 * Exclusive continuum CREATE-ONLY (7171-7180: dlopen flag stubs —
 * lazy_id, now_id, global_id, has_lazy, has_now, has_global,
 * flags_ok, flags_pack, flags_errorish, batch_id_7180).
 * Unique gj_rtld_has_global_u_7176 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7176_marker[] = "libcgj-batch7176";

/* RTLD_GLOBAL bit (0x100). */
#define B7176_RTLD_GLOBAL ((uint32_t)0x100u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7176_rtld_has_global(uint32_t u32Flags)
{
	return ((u32Flags & B7176_RTLD_GLOBAL) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtld_has_global_u_7176 - 1 if flags includes RTLD_GLOBAL.
 *
 * flags: dlopen(3) mode argument
 *
 * Returns 1 when bit 0x100 is set, else 0. Soft bit test only; does
 * not call dlopen. No parent wires.
 */
uint32_t
gj_rtld_has_global_u_7176(uint32_t u32Flags)
{
	(void)NULL;
	return b7176_rtld_has_global(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtld_has_global_u_7176(uint32_t u32Flags)
    __attribute__((alias("gj_rtld_has_global_u_7176")));
