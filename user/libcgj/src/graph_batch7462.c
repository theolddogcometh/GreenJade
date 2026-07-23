/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7462: unlinkat AT_REMOVEDIR bit test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uat_has_removedir_u_7462(uint32_t flags);
 *     - Return 1 if AT_REMOVEDIR is set in flags, else 0.
 *   uint32_t __gj_uat_has_removedir_u_7462  (alias)
 *   __libcgj_batch7462_marker = "libcgj-batch7462"
 *
 * Exclusive continuum CREATE-ONLY (7461-7470: unlinkat flags stubs —
 * removedir, has_removedir, known_mask, has_any, flags_ok, plain,
 * exactly_removedir, clear_removedir, apply_removedir, batch_id_7470).
 * Unique gj_uat_has_removedir_u_7462 surface only; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7462_marker[] = "libcgj-batch7462";

/* AT_REMOVEDIR: unlinkat acts like rmdir(2) (0x200). */
#define B7462_AT_REMOVEDIR ((uint32_t)0x00000200u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7462_has_removedir(uint32_t u32Flags)
{
	return ((u32Flags & B7462_AT_REMOVEDIR) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uat_has_removedir_u_7462 - 1 if AT_REMOVEDIR is set.
 *
 * flags: raw unlinkat(2) flags bitmask
 *
 * Returns 1 when the rmdir-mode bit is set, else 0.
 * Pure mask test; no parent wires.
 */
uint32_t
gj_uat_has_removedir_u_7462(uint32_t u32Flags)
{
	(void)NULL;
	return b7462_has_removedir(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uat_has_removedir_u_7462(uint32_t u32Flags)
    __attribute__((alias("gj_uat_has_removedir_u_7462")));
