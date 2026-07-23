/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1820: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1820(void);
 *     — Returns the compile-time graph batch number for this TU (1820).
 *   uint32_t __gj_batch_id_1820  (alias)
 *   __libcgj_batch1820_marker = "libcgj-batch1820"
 *
 * Milestone for the IPv6 helpers exclusive continuum 1811..1819
 * (is_unspecified / is_loopback / is_linklocal / is_sitelocal /
 * is_multicast / is_v4mapped / v4mapped_extract / addr_eq / addr_clear).
 * Does NOT redefine prior batch_id_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1820_marker[] = "libcgj-batch1820";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1820 — report this TU's graph batch number.
 *
 * Always returns 1820.
 */
uint32_t
gj_batch_id_1820(void)
{
	return 1820u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1820(void)
    __attribute__((alias("gj_batch_id_1820")));
