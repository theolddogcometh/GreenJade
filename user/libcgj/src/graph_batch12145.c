/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12145: resolved soft id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_resolved_ok_u_12145(void);
 *     - Returns 0 (soft stub: systemd-resolved presence is soft/unprobed;
 *       not a runtime DNS/resolve1 probe).
 *   uint32_t __gj_resolved_ok_u_12145  (alias)
 *   __libcgj_batch12145_marker = "libcgj-batch12145"
 *
 * Exclusive continuum CREATE-ONLY (12141-12150: dbus/systemd soft id
 * stubs). Unique gj_resolved_ok_u_12145 surface only; no multi-def.
 * Distinct from sibling soft ok stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12145_marker[] = "libcgj-batch12145";

/* Soft-stub resolved ok value (unprobed / not present). */
#define B12145_RESOLVED_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12145_resolved_ok(void)
{
	return B12145_RESOLVED_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_resolved_ok_u_12145 - soft systemd-resolved presence stub.
 *
 * Always returns 0 (soft/unprobed resolved slot). Does not call libc or
 * probe org.freedesktop.resolve1. No parent wires.
 */
uint32_t
gj_resolved_ok_u_12145(void)
{
	(void)NULL;
	return b12145_resolved_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_resolved_ok_u_12145(void)
    __attribute__((alias("gj_resolved_ok_u_12145")));
