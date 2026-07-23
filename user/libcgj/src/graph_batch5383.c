/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5383: glibc ABI minor 34 product tag (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_glibc_abi_34_u(void);
 *     - Return the advertised glibc ABI minor (always 34, as in
 *       GLIBC_2.34 product reference). Soft compile-time product constant.
 *   uint32_t __gj_glibc_abi_34_u  (alias)
 *   __libcgj_batch5383_marker = "libcgj-batch5383"
 *
 * Exclusive continuum CREATE-ONLY (5381-5390: libc soname product unique —
 * libc_soname_major_u, glibc_abi_2_u, glibc_abi_34_u, ldso_ok_u,
 * dynlink_green_u, smoke_host_green_u, cleanroom_ok_u, no_gpl_in_tree_u,
 * dual_license_ok_u, batch_id_5390). Unique gj_glibc_abi_34_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5383_marker[] = "libcgj-batch5383";

/* GLIBC_2.34 ABI minor product tag. */
#define B5383_ABI_MINOR  34u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5383_abi_34(void)
{
	return B5383_ABI_MINOR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glibc_abi_34_u - report glibc ABI minor 34 product tag.
 *
 * Always returns 34 (GLIBC_2.34 reference). Soft pure-data constant for
 * version-script / product tables. Self-contained; no parent wires.
 */
uint32_t
gj_glibc_abi_34_u(void)
{
	(void)NULL;
	return b5383_abi_34();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glibc_abi_34_u(void)
    __attribute__((alias("gj_glibc_abi_34_u")));
