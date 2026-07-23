/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8854: eth-ish ifindex soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_if_eth_ok_u_8854(uint32_t ifindex);
 *     - Return 1 if ifindex looks like a non-loopback Ethernet-class
 *       index (ifindex >= 2), else 0.
 *   uint32_t __gj_if_eth_ok_u_8854  (alias)
 *   __libcgj_batch8854_marker = "libcgj-batch8854"
 *
 * Exclusive continuum CREATE-ONLY (8851-8860: if_nametoindex stubs —
 * if_name_ok_u, if_index_ok_u, if_loop_id, if_eth_ok_u, if_up_id,
 * if_running_id, if_mtu_ok_u, if_mac_len_id, if_ready_u,
 * batch_id_8860). Unique gj_if_eth_ok_u_8854 surface only; no multi-def.
 * Soft eth-class gate only; no if_nametoindex. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8854_marker[] = "libcgj-batch8854";

/* Soft min ifindex for eth-class (loopback conventionally occupies 1). */
#define B8854_IF_ETH_MIN  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8854_eth_ok(uint32_t u32IfIndex)
{
	return (u32IfIndex >= B8854_IF_ETH_MIN) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_eth_ok_u_8854 - 1 if ifindex is a soft eth-class interface index.
 *
 * ifindex: candidate interface index (unsigned)
 *
 * Returns 1 when ifindex >= 2 (non-zero, not conventional loopback),
 * else 0. Soft if_nametoindex eth-class gate; does not call libc.
 * No parent wires.
 */
uint32_t
gj_if_eth_ok_u_8854(uint32_t u32IfIndex)
{
	(void)NULL;
	return b8854_eth_ok(u32IfIndex);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_if_eth_ok_u_8854(uint32_t u32IfIndex)
    __attribute__((alias("gj_if_eth_ok_u_8854")));
