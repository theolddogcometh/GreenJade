/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12881: bar3 install checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_check_u_12881(void);
 *     - Returns 0 (soft stub: install checklist slot is soft/unprobed;
 *       not a runtime install-path probe).
 *   uint32_t __gj_bar3_install_check_u_12881  (alias)
 *   __libcgj_batch12881_marker = "libcgj-batch12881"
 *
 * Exclusive continuum CREATE-ONLY (12881-12890: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_install_check_u_12881 surface only;
 * no multi-def. Distinct from gj_bar3_install_check_u_12681,
 * gj_bar3_install_check_u_12481, and sibling check_u soft stubs in
 * this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12881_marker[] = "libcgj-batch12881";

/* Soft-stub install checklist value (unprobed). */
#define B12881_INSTALL_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12881_install_check(void)
{
	return B12881_INSTALL_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_check_u_12881 - soft bar3 install checklist stub.
 *
 * Always returns 0 (soft/unprobed install slot). Does not call libc or
 * probe install paths. No parent wires.
 */
uint32_t
gj_bar3_install_check_u_12881(void)
{
	(void)NULL;
	return b12881_install_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_check_u_12881(void)
    __attribute__((alias("gj_bar3_install_check_u_12881")));
