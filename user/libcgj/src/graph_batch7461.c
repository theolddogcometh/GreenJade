/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7461: unlinkat AT_REMOVEDIR flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uat_removedir_7461(void);
 *     - Returns 0x200 (soft AT_REMOVEDIR: rmdir semantics for unlinkat).
 *       Pure-data catalog stub; not a live unlinkat syscall.
 *   uint32_t __gj_uat_removedir_7461  (alias)
 *   __libcgj_batch7461_marker = "libcgj-batch7461"
 *
 * Exclusive continuum CREATE-ONLY (7461-7470: unlinkat flags stubs —
 * removedir, has_removedir, known_mask, has_any, flags_ok, plain,
 * exactly_removedir, clear_removedir, apply_removedir, batch_id_7470).
 * Unique gj_uat_removedir_7461 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7461_marker[] = "libcgj-batch7461";

/* AT_REMOVEDIR: unlinkat acts like rmdir(2) (0x200). */
#define B7461_AT_REMOVEDIR ((uint32_t)0x00000200u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7461_removedir(void)
{
	return B7461_AT_REMOVEDIR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uat_removedir_7461 - soft AT_REMOVEDIR flag value.
 *
 * Always returns 0x200. Catalog stub for unlinkat(2) flag bit.
 * No parent wires.
 */
uint32_t
gj_uat_removedir_7461(void)
{
	(void)NULL;
	return b7461_removedir();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uat_removedir_7461(void)
    __attribute__((alias("gj_uat_removedir_7461")));
