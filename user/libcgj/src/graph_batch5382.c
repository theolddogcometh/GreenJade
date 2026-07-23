/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5382: glibc ABI major product tag (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_glibc_abi_2_u(void);
 *     - Return the advertised glibc ABI major (always 2, as in
 *       GLIBC_2.* product family). Soft compile-time product constant.
 *   uint32_t __gj_glibc_abi_2_u  (alias)
 *   __libcgj_batch5382_marker = "libcgj-batch5382"
 *
 * Exclusive continuum CREATE-ONLY (5381-5390: libc soname product unique —
 * libc_soname_major_u, glibc_abi_2_u, glibc_abi_34_u, ldso_ok_u,
 * dynlink_green_u, smoke_host_green_u, cleanroom_ok_u, no_gpl_in_tree_u,
 * dual_license_ok_u, batch_id_5390). Unique gj_glibc_abi_2_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5382_marker[] = "libcgj-batch5382";

/* GLIBC_2.* ABI major product tag. */
#define B5382_ABI_MAJOR  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5382_abi_2(void)
{
	return B5382_ABI_MAJOR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glibc_abi_2_u - report glibc ABI major product tag.
 *
 * Always returns 2 (GLIBC_2.* family). Soft pure-data constant for
 * version-script / product tables. Self-contained; no parent wires.
 */
uint32_t
gj_glibc_abi_2_u(void)
{
	(void)NULL;
	return b5382_abi_2();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glibc_abi_2_u(void)
    __attribute__((alias("gj_glibc_abi_2_u")));
