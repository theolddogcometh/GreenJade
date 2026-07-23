/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8852: if_nametoindex ifindex validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_if_index_ok_u_8852(uint32_t ifindex);
 *     - Return 1 if ifindex is a non-zero interface index (if_nametoindex
 *       returns 0 on failure), else 0.
 *   uint32_t __gj_if_index_ok_u_8852  (alias)
 *   __libcgj_batch8852_marker = "libcgj-batch8852"
 *
 * Exclusive continuum CREATE-ONLY (8851-8860: if_nametoindex stubs —
 * if_name_ok_u, if_index_ok_u, if_loop_id, if_eth_ok_u, if_up_id,
 * if_running_id, if_mtu_ok_u, if_mac_len_id, if_ready_u,
 * batch_id_8860). Unique gj_if_index_ok_u_8852 surface only; no
 * multi-def. Soft nonzero gate only; no if_nametoindex. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8852_marker[] = "libcgj-batch8852";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8852_index_ok(uint32_t u32IfIndex)
{
	return (u32IfIndex != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_index_ok_u_8852 - 1 if ifindex is a non-zero interface index.
 *
 * ifindex: candidate interface index (unsigned)
 *
 * Returns 1 when ifindex != 0, else 0. Mirrors the soft if_nametoindex
 * success convention (0 = failure). Does not call libc. No parent wires.
 */
uint32_t
gj_if_index_ok_u_8852(uint32_t u32IfIndex)
{
	(void)NULL;
	return b8852_index_ok(u32IfIndex);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_if_index_ok_u_8852(uint32_t u32IfIndex)
    __attribute__((alias("gj_if_index_ok_u_8852")));
