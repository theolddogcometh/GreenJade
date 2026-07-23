/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10143: logind soft id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_logind_ok_u_10143(void);
 *     - Returns 0 (soft stub: systemd-logind presence is soft/unprobed;
 *       not a runtime seat/session probe).
 *   uint32_t __gj_logind_ok_u_10143  (alias)
 *   __libcgj_batch10143_marker = "libcgj-batch10143"
 *
 * Exclusive continuum CREATE-ONLY (10141-10150: dbus/systemd soft id
 * stubs). Unique gj_logind_ok_u_10143 surface only; no multi-def.
 * Distinct from sibling soft ok stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10143_marker[] = "libcgj-batch10143";

/* Soft-stub logind ok value (unprobed / not present). */
#define B10143_LOGIND_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10143_logind_ok(void)
{
	return B10143_LOGIND_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_logind_ok_u_10143 - soft logind presence stub.
 *
 * Always returns 0 (soft/unprobed logind slot). Does not call libc or
 * probe org.freedesktop.login1. No parent wires.
 */
uint32_t
gj_logind_ok_u_10143(void)
{
	(void)NULL;
	return b10143_logind_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_logind_ok_u_10143(void)
    __attribute__((alias("gj_logind_ok_u_10143")));
