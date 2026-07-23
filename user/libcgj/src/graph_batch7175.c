/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7175: dlopen RTLD_NOW presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtld_has_now_u_7175(uint32_t flags);
 *     - Return 1 if flags has RTLD_NOW (0x2) set, else 0.
 *   uint32_t __gj_rtld_has_now_u_7175  (alias)
 *   __libcgj_batch7175_marker = "libcgj-batch7175"
 *
 * Exclusive continuum CREATE-ONLY (7171-7180: dlopen flag stubs —
 * lazy_id, now_id, global_id, has_lazy, has_now, has_global,
 * flags_ok, flags_pack, flags_errorish, batch_id_7180).
 * Unique gj_rtld_has_now_u_7175 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7175_marker[] = "libcgj-batch7175";

/* RTLD_NOW bit (0x2). */
#define B7175_RTLD_NOW ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7175_rtld_has_now(uint32_t u32Flags)
{
	return ((u32Flags & B7175_RTLD_NOW) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtld_has_now_u_7175 - 1 if flags includes RTLD_NOW.
 *
 * flags: dlopen(3) mode argument
 *
 * Returns 1 when bit 0x2 is set, else 0. Soft bit test only; does not
 * call dlopen. No parent wires.
 */
uint32_t
gj_rtld_has_now_u_7175(uint32_t u32Flags)
{
	(void)NULL;
	return b7175_rtld_has_now(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtld_has_now_u_7175(uint32_t u32Flags)
    __attribute__((alias("gj_rtld_has_now_u_7175")));
