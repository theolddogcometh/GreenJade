/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8880: resolv/dns id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8880(void);
 *     - Returns the compile-time graph batch number for this TU (8880).
 *   uint32_t __gj_batch_id_8880  (alias)
 *   __libcgj_batch8880_marker = "libcgj-batch8880"
 *
 * Exclusive continuum CREATE-ONLY (8871-8880: resolv/dns id stubs —
 * dns_a_id_8871, dns_aaaa_id_8872, dns_cname_id_8873, dns_mx_id_8874,
 * dns_txt_id_8875, dns_ns_id_8876, dns_ptr_id_8877, dns_port_id_8878,
 * resolv_ready_u_8879, batch_id_8880).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8880_marker[] = "libcgj-batch8880";

#define B8880_BATCH_ID  8880u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8880_id(void)
{
	return B8880_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8880 - report this TU's graph batch number.
 *
 * Always returns 8880.
 */
uint32_t
gj_batch_id_8880(void)
{
	(void)NULL;
	return b8880_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8880(void)
    __attribute__((alias("gj_batch_id_8880")));
