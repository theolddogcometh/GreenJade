/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10084: bar3 proton checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_proton_check_u_10084(void);
 *     - Returns 0 (soft stub: proton checklist slot is soft/unprobed;
 *       not a runtime Proton/compat probe).
 *   uint32_t __gj_bar3_proton_check_u_10084  (alias)
 *   __libcgj_batch10084_marker = "libcgj-batch10084"
 *
 * Exclusive continuum CREATE-ONLY (10081-10090: bar3 checklist soft
 * stubs). Unique gj_bar3_proton_check_u_10084 surface only; no
 * multi-def. Distinct from sibling check_u soft stubs in this wave.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10084_marker[] = "libcgj-batch10084";

/* Soft-stub proton checklist value (unprobed). */
#define B10084_PROTON_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10084_proton_check(void)
{
	return B10084_PROTON_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_proton_check_u_10084 - soft bar3 proton checklist stub.
 *
 * Always returns 0 (soft/unprobed proton slot). Does not call libc or
 * probe Proton. No parent wires.
 */
uint32_t
gj_bar3_proton_check_u_10084(void)
{
	(void)NULL;
	return b10084_proton_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_proton_check_u_10084(void)
    __attribute__((alias("gj_bar3_proton_check_u_10084")));
