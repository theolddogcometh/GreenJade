/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8858: Ethernet MAC address length id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_if_mac_len_id_8858(void);
 *     - Return the canonical Ethernet MAC address length in octets
 *       (ETH_ALEN = 6).
 *   uint32_t __gj_if_mac_len_id_8858  (alias)
 *   __libcgj_batch8858_marker = "libcgj-batch8858"
 *
 * Exclusive continuum CREATE-ONLY (8851-8860: if_nametoindex stubs —
 * if_name_ok_u, if_index_ok_u, if_loop_id, if_eth_ok_u, if_up_id,
 * if_running_id, if_mtu_ok_u, if_mac_len_id, if_ready_u,
 * batch_id_8860). Unique gj_if_mac_len_id_8858 surface only; no
 * multi-def. Distinct from gj_net_eth_alen_u (batch5061). Catalog id
 * only; no if_nametoindex. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8858_marker[] = "libcgj-batch8858";

/* Soft ETH_ALEN (Ethernet MAC address length in octets). */
#define B8858_IF_MAC_LEN  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8858_mac_len(void)
{
	return B8858_IF_MAC_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_mac_len_id_8858 - soft Ethernet MAC address length id.
 *
 * Always returns 6 (ETH_ALEN). Catalog id only; does not probe a real
 * NIC or call libc. No parent wires.
 */
uint32_t
gj_if_mac_len_id_8858(void)
{
	(void)NULL;
	return b8858_mac_len();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_if_mac_len_id_8858(void)
    __attribute__((alias("gj_if_mac_len_id_8858")));
