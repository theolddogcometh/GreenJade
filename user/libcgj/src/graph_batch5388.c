/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5388: no-GPL-in-tree product claim (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_no_gpl_in_tree_u(void);
 *     - Return 1 when the product tree asserts no GPL-contaminated source
 *       in this continuum wave. Soft compile-time policy flag.
 *   uint32_t __gj_no_gpl_in_tree_u  (alias)
 *   __libcgj_batch5388_marker = "libcgj-batch5388"
 *
 * Exclusive continuum CREATE-ONLY (5381-5390: libc soname product unique —
 * libc_soname_major_u, glibc_abi_2_u, glibc_abi_34_u, ldso_ok_u,
 * dynlink_green_u, smoke_host_green_u, cleanroom_ok_u, no_gpl_in_tree_u,
 * dual_license_ok_u, batch_id_5390). Unique gj_no_gpl_in_tree_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5388_marker[] = "libcgj-batch5388";

/* No-GPL-in-tree product claim green. */
#define B5388_NO_GPL_IN_TREE  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5388_no_gpl_in_tree(void)
{
	return B5388_NO_GPL_IN_TREE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_no_gpl_in_tree_u - report no-GPL-in-tree product claim.
 *
 * Always returns 1 (claim holds). Soft pure-data constant for license
 * policy tables. Self-contained; no parent wires.
 */
uint32_t
gj_no_gpl_in_tree_u(void)
{
	(void)NULL;
	return b5388_no_gpl_in_tree();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_no_gpl_in_tree_u(void)
    __attribute__((alias("gj_no_gpl_in_tree_u")));
