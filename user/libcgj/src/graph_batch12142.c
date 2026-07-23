/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12142: systemd soft id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_systemd_ok_u_12142(void);
 *     - Returns 0 (soft stub: systemd presence is soft/unprobed; not a
 *       runtime sd_booted or unit probe).
 *   uint32_t __gj_systemd_ok_u_12142  (alias)
 *   __libcgj_batch12142_marker = "libcgj-batch12142"
 *
 * Exclusive continuum CREATE-ONLY (12141-12150: dbus/systemd soft id
 * stubs). Unique gj_systemd_ok_u_12142 surface only; no multi-def.
 * Distinct from sibling soft ok stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12142_marker[] = "libcgj-batch12142";

/* Soft-stub systemd ok value (unprobed / not present). */
#define B12142_SYSTEMD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12142_systemd_ok(void)
{
	return B12142_SYSTEMD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_systemd_ok_u_12142 - soft systemd presence stub.
 *
 * Always returns 0 (soft/unprobed systemd slot). Does not call libc or
 * probe the init system. No parent wires.
 */
uint32_t
gj_systemd_ok_u_12142(void)
{
	(void)NULL;
	return b12142_systemd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_systemd_ok_u_12142(void)
    __attribute__((alias("gj_systemd_ok_u_12142")));
