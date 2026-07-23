/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7468: unlinkat clear-AT_REMOVEDIR mask stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uat_clear_removedir_u_7468(uint32_t flags);
 *     - Return flags with AT_REMOVEDIR cleared (file-unlink mode).
 *   uint32_t __gj_uat_clear_removedir_u_7468  (alias)
 *   __libcgj_batch7468_marker = "libcgj-batch7468"
 *
 * Exclusive continuum CREATE-ONLY (7461-7470: unlinkat flags stubs —
 * removedir, has_removedir, known_mask, has_any, flags_ok, plain,
 * exactly_removedir, clear_removedir, apply_removedir, batch_id_7470).
 * Unique gj_uat_clear_removedir_u_7468 surface only; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7468_marker[] = "libcgj-batch7468";

/* AT_REMOVEDIR: unlinkat acts like rmdir(2) (0x200). */
#define B7468_AT_REMOVEDIR ((uint32_t)0x00000200u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7468_clear_removedir(uint32_t u32Flags)
{
	return u32Flags & ~B7468_AT_REMOVEDIR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uat_clear_removedir_u_7468 - flags with AT_REMOVEDIR bit cleared.
 *
 * flags: raw unlinkat(2) flags bitmask
 *
 * Returns flags & ~AT_REMOVEDIR. Soft mask helper; not a live
 * unlinkat. No parent wires.
 */
uint32_t
gj_uat_clear_removedir_u_7468(uint32_t u32Flags)
{
	(void)NULL;
	return b7468_clear_removedir(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uat_clear_removedir_u_7468(uint32_t u32Flags)
    __attribute__((alias("gj_uat_clear_removedir_u_7468")));
