/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6187: cgroup freezer state validity stub.
 *
 * Surface (unique symbols):
 *   int gj_cg_freezer_state_ok_6187(uint32_t state);
 *     - Return 1 if state is a known freezer encoding:
 *         0 = THAWED, 1 = FROZEN (soft container runtime codes).
 *       Any other value → 0.
 *   int __gj_cg_freezer_state_ok_6187  (alias)
 *   __libcgj_batch6187_marker = "libcgj-batch6187"
 *
 * Exclusive continuum CREATE-ONLY (6181-6190: container runtime cgroup
 * stubs). Unique gj_cg_freezer_state_ok_6187 surface only; no multi-def.
 * Pure predicate — not a cgroup.freeze writer. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6187_marker[] = "libcgj-batch6187";

#define B6187_THAWED  0u
#define B6187_FROZEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6187_state_ok(uint32_t uState)
{
	if (uState == B6187_THAWED) {
		return 1;
	}
	if (uState == B6187_FROZEN) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cg_freezer_state_ok_6187 - 1 if state is THAWED or FROZEN.
 *
 * Screens parsed freezer / cgroup.freeze fields before container
 * lifecycle transitions. Pure predicate; no cgroupfs. Does not call
 * libc. No parent wires.
 */
int
gj_cg_freezer_state_ok_6187(uint32_t uState)
{
	(void)NULL;
	return b6187_state_ok(uState);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cg_freezer_state_ok_6187(uint32_t uState)
    __attribute__((alias("gj_cg_freezer_state_ok_6187")));
