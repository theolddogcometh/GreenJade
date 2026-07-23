/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11656: sudo soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sudo_ok_u_11656(void);
 *     - Returns 0 (sudo soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live sudo/sudoers probe.
 *   uint32_t __gj_sudo_ok_u_11656  (alias)
 *   __libcgj_batch11656_marker = "libcgj-batch11656"
 *
 * Exclusive CREATE-ONLY (11651-11660: polkit/seatd soft id stubs). Unique
 * gj_sudo_ok_u_11656 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11656_marker[] = "libcgj-batch11656";

/* Soft lamp: sudo ok stub (not asserted). */
#define B11656_SUDO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11656_sudo_ok(void)
{
	return B11656_SUDO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sudo_ok_u_11656 - sudo soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not exec sudo, open
 * sudoers, or probe privilege escalation paths. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_sudo_ok_u_11656(void)
{
	(void)NULL;
	return b11656_sudo_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sudo_ok_u_11656(void)
    __attribute__((alias("gj_sudo_ok_u_11656")));
