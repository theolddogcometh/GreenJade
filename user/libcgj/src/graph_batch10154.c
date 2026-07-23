/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10154: PAM soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pam_ok_u_10154(void);
 *     - Returns 0 (PAM soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live libpam probe.
 *   uint32_t __gj_pam_ok_u_10154  (alias)
 *   __libcgj_batch10154_marker = "libcgj-batch10154"
 *
 * Exclusive CREATE-ONLY (10151-10160: polkit/seatd soft id stubs). Unique
 * gj_pam_ok_u_10154 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10154_marker[] = "libcgj-batch10154";

/* Soft lamp: PAM ok stub (not asserted). */
#define B10154_PAM_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10154_pam_ok(void)
{
	return B10154_PAM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pam_ok_u_10154 - PAM soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not open PAM stacks,
 * call pam_start, or probe /etc/pam.d. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_pam_ok_u_10154(void)
{
	(void)NULL;
	return b10154_pam_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pam_ok_u_10154(void)
    __attribute__((alias("gj_pam_ok_u_10154")));
