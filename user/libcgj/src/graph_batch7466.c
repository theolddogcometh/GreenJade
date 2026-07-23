/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7466: unlinkat plain (zero-flags) stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uat_plain_u_7466(uint32_t flags);
 *     - Return 1 if flags is exactly 0 (plain unlink, not rmdir),
 *       else 0.
 *   uint32_t __gj_uat_plain_u_7466  (alias)
 *   __libcgj_batch7466_marker = "libcgj-batch7466"
 *
 * Exclusive continuum CREATE-ONLY (7461-7470: unlinkat flags stubs —
 * removedir, has_removedir, known_mask, has_any, flags_ok, plain,
 * exactly_removedir, clear_removedir, apply_removedir, batch_id_7470).
 * Unique gj_uat_plain_u_7466 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7466_marker[] = "libcgj-batch7466";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7466_plain(uint32_t u32Flags)
{
	return (u32Flags == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uat_plain_u_7466 - 1 if unlinkat flags are exactly zero.
 *
 * flags: raw unlinkat(2) flags bitmask
 *
 * Returns 1 for plain unlink (file removal; not AT_REMOVEDIR).
 * Pure equality test; no parent wires.
 */
uint32_t
gj_uat_plain_u_7466(uint32_t u32Flags)
{
	(void)NULL;
	return b7466_plain(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uat_plain_u_7466(uint32_t u32Flags)
    __attribute__((alias("gj_uat_plain_u_7466")));
