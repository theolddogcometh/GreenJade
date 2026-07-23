/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5381: libc soname major product tag (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libc_soname_major_u(void);
 *     - Return the advertised libc SONAME major (always 6, as in
 *       libc.so.6 product identity). Soft compile-time product constant.
 *   uint32_t __gj_libc_soname_major_u  (alias)
 *   __libcgj_batch5381_marker = "libcgj-batch5381"
 *
 * Exclusive continuum CREATE-ONLY (5381-5390: libc soname product unique —
 * libc_soname_major_u, glibc_abi_2_u, glibc_abi_34_u, ldso_ok_u,
 * dynlink_green_u, smoke_host_green_u, cleanroom_ok_u, no_gpl_in_tree_u,
 * dual_license_ok_u, batch_id_5390). Unique gj_libc_soname_major_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5381_marker[] = "libcgj-batch5381";

/* libc.so.N SONAME major product tag (N = 6). */
#define B5381_SONAME_MAJOR  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5381_soname_major(void)
{
	return B5381_SONAME_MAJOR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libc_soname_major_u - report libc SONAME major product tag.
 *
 * Always returns 6 (libc.so.6 product identity). Soft pure-data constant
 * for soname / packaging tables. Self-contained; no parent wires.
 */
uint32_t
gj_libc_soname_major_u(void)
{
	(void)NULL;
	return b5381_soname_major();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libc_soname_major_u(void)
    __attribute__((alias("gj_libc_soname_major_u")));
