/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8856: IFF_RUNNING flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_if_running_id_8856(void);
 *     - Return the soft IFF_RUNNING interface flag id (0x40). Catalog
 *       id for operationally-running state beside if_nametoindex.
 *   uint32_t __gj_if_running_id_8856  (alias)
 *   __libcgj_batch8856_marker = "libcgj-batch8856"
 *
 * Exclusive continuum CREATE-ONLY (8851-8860: if_nametoindex stubs —
 * if_name_ok_u, if_index_ok_u, if_loop_id, if_eth_ok_u, if_up_id,
 * if_running_id, if_mtu_ok_u, if_mac_len_id, if_ready_u,
 * batch_id_8860). Unique gj_if_running_id_8856 surface only; no
 * multi-def. Catalog id only; no ioctl/netlink. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8856_marker[] = "libcgj-batch8856";

/* Soft IFF_RUNNING (interface operationally running). */
#define B8856_IF_RUNNING_ID  0x40u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8856_running_id(void)
{
	return B8856_IF_RUNNING_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_running_id_8856 - soft IFF_RUNNING interface flag id.
 *
 * Always returns 0x40 (IFF_RUNNING). Catalog id only; does not query
 * real interface flags. Does not call libc. No parent wires.
 */
uint32_t
gj_if_running_id_8856(void)
{
	(void)NULL;
	return b8856_running_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_if_running_id_8856(void)
    __attribute__((alias("gj_if_running_id_8856")));
