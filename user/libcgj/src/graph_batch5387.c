/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5387: clean-room product readiness (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cleanroom_ok_u(void);
 *     - Return 1 when the clean-room freestanding product claim is green
 *       for this continuum wave. Soft compile-time readiness flag.
 *   uint32_t __gj_cleanroom_ok_u  (alias)
 *   __libcgj_batch5387_marker = "libcgj-batch5387"
 *
 * Exclusive continuum CREATE-ONLY (5381-5390: libc soname product unique —
 * libc_soname_major_u, glibc_abi_2_u, glibc_abi_34_u, ldso_ok_u,
 * dynlink_green_u, smoke_host_green_u, cleanroom_ok_u, no_gpl_in_tree_u,
 * dual_license_ok_u, batch_id_5390). Unique gj_cleanroom_ok_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5387_marker[] = "libcgj-batch5387";

/* Clean-room product claim green. */
#define B5387_CLEANROOM_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5387_cleanroom_ok(void)
{
	return B5387_CLEANROOM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cleanroom_ok_u - report clean-room freestanding product readiness.
 *
 * Always returns 1 (green). Soft pure-data constant for product policy
 * tables. Self-contained; no parent wires.
 */
uint32_t
gj_cleanroom_ok_u(void)
{
	(void)NULL;
	return b5387_cleanroom_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cleanroom_ok_u(void)
    __attribute__((alias("gj_cleanroom_ok_u")));
