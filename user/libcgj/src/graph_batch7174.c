/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7174: dlopen RTLD_LAZY presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtld_has_lazy_u_7174(uint32_t flags);
 *     - Return 1 if flags has RTLD_LAZY (0x1) set, else 0.
 *   uint32_t __gj_rtld_has_lazy_u_7174  (alias)
 *   __libcgj_batch7174_marker = "libcgj-batch7174"
 *
 * Exclusive continuum CREATE-ONLY (7171-7180: dlopen flag stubs —
 * lazy_id, now_id, global_id, has_lazy, has_now, has_global,
 * flags_ok, flags_pack, flags_errorish, batch_id_7180).
 * Unique gj_rtld_has_lazy_u_7174 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7174_marker[] = "libcgj-batch7174";

/* RTLD_LAZY bit (0x1). */
#define B7174_RTLD_LAZY ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7174_rtld_has_lazy(uint32_t u32Flags)
{
	return ((u32Flags & B7174_RTLD_LAZY) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtld_has_lazy_u_7174 - 1 if flags includes RTLD_LAZY.
 *
 * flags: dlopen(3) mode argument
 *
 * Returns 1 when bit 0x1 is set, else 0. Soft bit test only; does not
 * call dlopen. No parent wires.
 */
uint32_t
gj_rtld_has_lazy_u_7174(uint32_t u32Flags)
{
	(void)NULL;
	return b7174_rtld_has_lazy(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtld_has_lazy_u_7174(uint32_t u32Flags)
    __attribute__((alias("gj_rtld_has_lazy_u_7174")));
