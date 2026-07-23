/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7786: glob GLOB_NOCHECK presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glob_has_nocheck_u_7786(uint32_t flags);
 *     - Return 1 if flags has GLOB_NOCHECK (0x10) set, else 0.
 *   uint32_t __gj_glob_has_nocheck_u_7786  (alias)
 *   __libcgj_batch7786_marker = "libcgj-batch7786"
 *
 * Exclusive continuum CREATE-ONLY (7781-7790: glob flags stubs —
 * err_id, mark_id, nocheck_id, has_err, has_mark, has_nocheck,
 * flags_ok, flags_pack, flags_errorish, batch_id_7790).
 * Unique gj_glob_has_nocheck_u_7786 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7786_marker[] = "libcgj-batch7786";

/* GLOB_NOCHECK bit (0x10). */
#define B7786_GLOB_NOCHECK ((uint32_t)0x10u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7786_glob_has_nocheck(uint32_t u32Flags)
{
	return ((u32Flags & B7786_GLOB_NOCHECK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glob_has_nocheck_u_7786 - 1 if flags includes GLOB_NOCHECK.
 *
 * flags: glob(3) flags argument
 *
 * Returns 1 when bit 0x10 is set, else 0. Soft bit test only; does
 * not call glob(3). No parent wires.
 */
uint32_t
gj_glob_has_nocheck_u_7786(uint32_t u32Flags)
{
	(void)NULL;
	return b7786_glob_has_nocheck(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glob_has_nocheck_u_7786(uint32_t u32Flags)
    __attribute__((alias("gj_glob_has_nocheck_u_7786")));
