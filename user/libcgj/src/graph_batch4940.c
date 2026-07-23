/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4940: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4940(void);
 *     - Returns the compile-time graph batch number for this TU (4940).
 *   uint32_t __gj_batch_id_4940  (alias)
 *   __libcgj_batch4940_marker = "libcgj-batch4940"
 *
 * Exclusive continuum CREATE-ONLY socket-addr wave (4931-4940:
 * sockaddr_family_inet_u, sockaddr_family_inet6_u,
 * sockaddr_family_unix_u, ip4_pack_u, ip4_a_u, ip4_b_u, ip4_c_u,
 * ip4_d_u, ip4_is_loopback_u, batch_id_4940).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4940_marker[] = "libcgj-batch4940";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4940_id(void)
{
	return 4940u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4940 - report this TU's graph batch number.
 *
 * Always returns 4940. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4940(void)
{
	(void)NULL;
	return b4940_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4940(void)
    __attribute__((alias("gj_batch_id_4940")));
