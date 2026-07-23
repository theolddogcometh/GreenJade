/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10148: hostnamed soft id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hostnamed_ok_u_10148(void);
 *     - Returns 0 (soft stub: systemd-hostnamed presence is soft/unprobed;
 *       not a runtime hostname1 probe).
 *   uint32_t __gj_hostnamed_ok_u_10148  (alias)
 *   __libcgj_batch10148_marker = "libcgj-batch10148"
 *
 * Exclusive continuum CREATE-ONLY (10141-10150: dbus/systemd soft id
 * stubs). Unique gj_hostnamed_ok_u_10148 surface only; no multi-def.
 * Distinct from sibling soft ok stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10148_marker[] = "libcgj-batch10148";

/* Soft-stub hostnamed ok value (unprobed / not present). */
#define B10148_HOSTNAMED_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10148_hostnamed_ok(void)
{
	return B10148_HOSTNAMED_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hostnamed_ok_u_10148 - soft systemd-hostnamed presence stub.
 *
 * Always returns 0 (soft/unprobed hostnamed slot). Does not call libc or
 * probe org.freedesktop.hostname1. No parent wires.
 */
uint32_t
gj_hostnamed_ok_u_10148(void)
{
	(void)NULL;
	return b10148_hostnamed_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hostnamed_ok_u_10148(void)
    __attribute__((alias("gj_hostnamed_ok_u_10148")));
