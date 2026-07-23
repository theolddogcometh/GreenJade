/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10147: timedated soft id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_timedated_ok_u_10147(void);
 *     - Returns 0 (soft stub: systemd-timedated presence is soft/unprobed;
 *       not a runtime timedate1 / NTP probe).
 *   uint32_t __gj_timedated_ok_u_10147  (alias)
 *   __libcgj_batch10147_marker = "libcgj-batch10147"
 *
 * Exclusive continuum CREATE-ONLY (10141-10150: dbus/systemd soft id
 * stubs). Unique gj_timedated_ok_u_10147 surface only; no multi-def.
 * Distinct from sibling soft ok stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10147_marker[] = "libcgj-batch10147";

/* Soft-stub timedated ok value (unprobed / not present). */
#define B10147_TIMEDATED_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10147_timedated_ok(void)
{
	return B10147_TIMEDATED_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timedated_ok_u_10147 - soft systemd-timedated presence stub.
 *
 * Always returns 0 (soft/unprobed timedated slot). Does not call libc or
 * probe org.freedesktop.timedate1. No parent wires.
 */
uint32_t
gj_timedated_ok_u_10147(void)
{
	(void)NULL;
	return b10147_timedated_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_timedated_ok_u_10147(void)
    __attribute__((alias("gj_timedated_ok_u_10147")));
