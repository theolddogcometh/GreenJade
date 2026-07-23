/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10548: HDR soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdr_ok_u_10548(void);
 *     - Returns 0 (soft stub: HDR slot is soft/unprobed; not a runtime
 *       HDR capability probe).
 *   uint32_t __gj_hdr_ok_u_10548  (alias)
 *   __libcgj_batch10548_marker = "libcgj-batch10548"
 *
 * Exclusive continuum CREATE-ONLY (10541-10550: gamescope soft stubs —
 * all→0). Unique gj_hdr_ok_u_10548 surface only; no multi-def.
 * Distinct from sibling gamescope soft ok_u stubs in this wave. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10548_marker[] = "libcgj-batch10548";

/* Soft-stub HDR ok value (unprobed). */
#define B10548_HDR_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10548_hdr_ok(void)
{
	return B10548_HDR_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_ok_u_10548 - soft HDR ok stub.
 *
 * Always returns 0 (soft/unprobed HDR slot). Does not call libc or
 * probe HDR capability. No parent wires.
 */
uint32_t
gj_hdr_ok_u_10548(void)
{
	(void)NULL;
	return b10548_hdr_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdr_ok_u_10548(void)
    __attribute__((alias("gj_hdr_ok_u_10548")));
