/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7178: dlopen flags pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rtld_flags_pack_u_7178(uint32_t want_lazy,
 *                                      uint32_t want_now,
 *                                      uint32_t want_global);
 *     - Pack soft RTLD_* wants into a mode mask (0x1|0x2|0x100).
 *   uint32_t __gj_rtld_flags_pack_u_7178  (alias)
 *   __libcgj_batch7178_marker = "libcgj-batch7178"
 *
 * Exclusive continuum CREATE-ONLY (7171-7180: dlopen flag stubs —
 * lazy_id, now_id, global_id, has_lazy, has_now, has_global,
 * flags_ok, flags_pack, flags_errorish, batch_id_7180).
 * Unique gj_rtld_flags_pack_u_7178 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7178_marker[] = "libcgj-batch7178";

/* Soft pack bits matching POSIX/glibc RTLD_* layout. */
#define B7178_RTLD_LAZY   ((uint32_t)0x1u)
#define B7178_RTLD_NOW    ((uint32_t)0x2u)
#define B7178_RTLD_GLOBAL ((uint32_t)0x100u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7178_rtld_flags_pack(uint32_t u32WantLazy,
                      uint32_t u32WantNow,
                      uint32_t u32WantGlobal)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantLazy != 0u)
		u32Pack |= B7178_RTLD_LAZY;
	if (u32WantNow != 0u)
		u32Pack |= B7178_RTLD_NOW;
	if (u32WantGlobal != 0u)
		u32Pack |= B7178_RTLD_GLOBAL;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rtld_flags_pack_u_7178 - soft-pack dlopen mode wants.
 *
 * want_lazy:   non-zero to set RTLD_LAZY (0x1)
 * want_now:    non-zero to set RTLD_NOW (0x2)
 * want_global: non-zero to set RTLD_GLOBAL (0x100)
 *
 * Returns bitmask of requested RTLD_* bits. Pure integer pack; does not
 * call dlopen. No parent wires.
 */
uint32_t
gj_rtld_flags_pack_u_7178(uint32_t u32WantLazy,
                          uint32_t u32WantNow,
                          uint32_t u32WantGlobal)
{
	(void)NULL;
	return b7178_rtld_flags_pack(u32WantLazy, u32WantNow,
	                             u32WantGlobal);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rtld_flags_pack_u_7178(uint32_t u32WantLazy,
                                     uint32_t u32WantNow,
                                     uint32_t u32WantGlobal)
    __attribute__((alias("gj_rtld_flags_pack_u_7178")));
