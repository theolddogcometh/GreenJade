/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12081: bar3 install checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_check_u_12081(void);
 *     - Returns 0 (soft stub: install checklist slot is soft/unprobed;
 *       not a runtime install-path probe).
 *   uint32_t __gj_bar3_install_check_u_12081  (alias)
 *   __libcgj_batch12081_marker = "libcgj-batch12081"
 *
 * Exclusive continuum CREATE-ONLY (12081-12090: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_install_check_u_12081 surface only;
 * no multi-def. Distinct from gj_bar3_install_check_u_11981,
 * gj_bar3_install_check_u_11781, and sibling check_u soft stubs in
 * this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12081_marker[] = "libcgj-batch12081";

/* Soft-stub install checklist value (unprobed). */
#define B12081_INSTALL_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12081_install_check(void)
{
	return B12081_INSTALL_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_check_u_12081 - soft bar3 install checklist stub.
 *
 * Always returns 0 (soft/unprobed install slot). Does not call libc or
 * probe install paths. No parent wires.
 */
uint32_t
gj_bar3_install_check_u_12081(void)
{
	(void)NULL;
	return b12081_install_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_check_u_12081(void)
    __attribute__((alias("gj_bar3_install_check_u_12081")));
