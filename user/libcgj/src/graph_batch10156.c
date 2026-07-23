/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10156: sudo soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sudo_ok_u_10156(void);
 *     - Returns 0 (sudo soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live sudo/sudoers probe.
 *   uint32_t __gj_sudo_ok_u_10156  (alias)
 *   __libcgj_batch10156_marker = "libcgj-batch10156"
 *
 * Exclusive CREATE-ONLY (10151-10160: polkit/seatd soft id stubs). Unique
 * gj_sudo_ok_u_10156 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10156_marker[] = "libcgj-batch10156";

/* Soft lamp: sudo ok stub (not asserted). */
#define B10156_SUDO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10156_sudo_ok(void)
{
	return B10156_SUDO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sudo_ok_u_10156 - sudo soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not exec sudo, open
 * sudoers, or probe privilege escalation paths. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_sudo_ok_u_10156(void)
{
	(void)NULL;
	return b10156_sudo_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sudo_ok_u_10156(void)
    __attribute__((alias("gj_sudo_ok_u_10156")));
