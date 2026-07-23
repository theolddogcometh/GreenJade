/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7463: unlinkat known-flags mask stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uat_known_mask_7463(void);
 *     - Returns AT_REMOVEDIR (0x200). Soft catalog of currently
 *       documented unlinkat flag bits.
 *   uint32_t __gj_uat_known_mask_7463  (alias)
 *   __libcgj_batch7463_marker = "libcgj-batch7463"
 *
 * Exclusive continuum CREATE-ONLY (7461-7470: unlinkat flags stubs —
 * removedir, has_removedir, known_mask, has_any, flags_ok, plain,
 * exactly_removedir, clear_removedir, apply_removedir, batch_id_7470).
 * Unique gj_uat_known_mask_7463 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7463_marker[] = "libcgj-batch7463";

/* Documented unlinkat(2) flag bits (uapi AT_REMOVEDIR). */
#define B7463_AT_REMOVEDIR ((uint32_t)0x00000200u)
#define B7463_KNOWN_MASK   (B7463_AT_REMOVEDIR)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7463_known_mask(void)
{
	return B7463_KNOWN_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uat_known_mask_7463 - soft mask of documented unlinkat flags.
 *
 * Always returns 0x200 (AT_REMOVEDIR). Pure catalog; no parent wires.
 */
uint32_t
gj_uat_known_mask_7463(void)
{
	(void)NULL;
	return b7463_known_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uat_known_mask_7463(void)
    __attribute__((alias("gj_uat_known_mask_7463")));
