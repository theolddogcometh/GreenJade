/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13281: bar3 install checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_check_u_13281(void);
 *     - Returns 0 (soft stub: install checklist slot is soft/unprobed;
 *       not a runtime install-path probe).
 *   uint32_t __gj_bar3_install_check_u_13281  (alias)
 *   __libcgj_batch13281_marker = "libcgj-batch13281"
 *
 * Exclusive continuum CREATE-ONLY (13281-13290: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_install_check_u_13281 surface only;
 * no multi-def. Distinct from gj_bar3_install_check_u_13081,
 * gj_bar3_install_check_u_12881, and sibling check_u soft stubs in
 * this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13281_marker[] = "libcgj-batch13281";

/* Soft-stub install checklist value (unprobed). */
#define B13281_INSTALL_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13281_install_check(void)
{
	return B13281_INSTALL_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_check_u_13281 - soft bar3 install checklist stub.
 *
 * Always returns 0 (soft/unprobed install slot). Does not call libc or
 * probe install paths. No parent wires.
 */
uint32_t
gj_bar3_install_check_u_13281(void)
{
	(void)NULL;
	return b13281_install_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_check_u_13281(void)
    __attribute__((alias("gj_bar3_install_check_u_13281")));
