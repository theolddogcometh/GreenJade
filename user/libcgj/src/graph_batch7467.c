/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7467: unlinkat exactly-AT_REMOVEDIR stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uat_exactly_removedir_u_7467(uint32_t flags);
 *     - Return 1 if flags is exactly AT_REMOVEDIR (0x200), else 0.
 *   uint32_t __gj_uat_exactly_removedir_u_7467  (alias)
 *   __libcgj_batch7467_marker = "libcgj-batch7467"
 *
 * Exclusive continuum CREATE-ONLY (7461-7470: unlinkat flags stubs —
 * removedir, has_removedir, known_mask, has_any, flags_ok, plain,
 * exactly_removedir, clear_removedir, apply_removedir, batch_id_7470).
 * Unique gj_uat_exactly_removedir_u_7467 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7467_marker[] = "libcgj-batch7467";

/* AT_REMOVEDIR: unlinkat acts like rmdir(2) (0x200). */
#define B7467_AT_REMOVEDIR ((uint32_t)0x00000200u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7467_exactly_removedir(uint32_t u32Flags)
{
	return (u32Flags == B7467_AT_REMOVEDIR) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uat_exactly_removedir_u_7467 - 1 if flags is exactly AT_REMOVEDIR.
 *
 * flags: raw unlinkat(2) flags bitmask
 *
 * Returns 1 only when flags equals 0x200 (no extra bits). Pure
 * equality test; no parent wires.
 */
uint32_t
gj_uat_exactly_removedir_u_7467(uint32_t u32Flags)
{
	(void)NULL;
	return b7467_exactly_removedir(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uat_exactly_removedir_u_7467(uint32_t u32Flags)
    __attribute__((alias("gj_uat_exactly_removedir_u_7467")));
