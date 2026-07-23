/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14282: bar3 boot checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_boot_check_u_14282(void);
 *     - Returns 0 (soft stub: boot checklist slot is soft/unprobed;
 *       not a runtime boot-path probe).
 *   uint32_t __gj_bar3_boot_check_u_14282  (alias)
 *   __libcgj_batch14282_marker = "libcgj-batch14282"
 *
 * Exclusive continuum CREATE-ONLY (14281-14290: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_boot_check_u_14282 surface only; no
 * multi-def. Distinct from gj_bar3_boot_check_u_14182,
 * gj_bar3_boot_check_u_13882, and sibling check_u soft stubs in this
 * wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14282_marker[] = "libcgj-batch14282";

/* Soft-stub boot checklist value (unprobed). */
#define B14282_BOOT_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14282_boot_check(void)
{
	return B14282_BOOT_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_boot_check_u_14282 - soft bar3 boot checklist stub.
 *
 * Always returns 0 (soft/unprobed boot slot). Does not call libc or
 * probe boot paths. No parent wires.
 */
uint32_t
gj_bar3_boot_check_u_14282(void)
{
	(void)NULL;
	return b14282_boot_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_boot_check_u_14282(void)
    __attribute__((alias("gj_bar3_boot_check_u_14282")));
