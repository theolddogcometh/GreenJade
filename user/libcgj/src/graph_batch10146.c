/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10146: networkd soft id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_networkd_ok_u_10146(void);
 *     - Returns 0 (soft stub: systemd-networkd presence is soft/unprobed;
 *       not a runtime network1 / link probe).
 *   uint32_t __gj_networkd_ok_u_10146  (alias)
 *   __libcgj_batch10146_marker = "libcgj-batch10146"
 *
 * Exclusive continuum CREATE-ONLY (10141-10150: dbus/systemd soft id
 * stubs). Unique gj_networkd_ok_u_10146 surface only; no multi-def.
 * Distinct from sibling soft ok stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10146_marker[] = "libcgj-batch10146";

/* Soft-stub networkd ok value (unprobed / not present). */
#define B10146_NETWORKD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10146_networkd_ok(void)
{
	return B10146_NETWORKD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_networkd_ok_u_10146 - soft systemd-networkd presence stub.
 *
 * Always returns 0 (soft/unprobed networkd slot). Does not call libc or
 * probe org.freedesktop.network1. No parent wires.
 */
uint32_t
gj_networkd_ok_u_10146(void)
{
	(void)NULL;
	return b10146_networkd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_networkd_ok_u_10146(void)
    __attribute__((alias("gj_networkd_ok_u_10146")));
