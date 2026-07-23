/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8855: IFF_UP flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_if_up_id_8855(void);
 *     - Return the soft IFF_UP interface flag id (0x1). Catalog id for
 *       administratively-up state beside if_nametoindex resolution.
 *   uint32_t __gj_if_up_id_8855  (alias)
 *   __libcgj_batch8855_marker = "libcgj-batch8855"
 *
 * Exclusive continuum CREATE-ONLY (8851-8860: if_nametoindex stubs —
 * if_name_ok_u, if_index_ok_u, if_loop_id, if_eth_ok_u, if_up_id,
 * if_running_id, if_mtu_ok_u, if_mac_len_id, if_ready_u,
 * batch_id_8860). Unique gj_if_up_id_8855 surface only; no multi-def.
 * Catalog id only; no ioctl/netlink. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8855_marker[] = "libcgj-batch8855";

/* Soft IFF_UP (interface administratively up). */
#define B8855_IF_UP_ID  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8855_up_id(void)
{
	return B8855_IF_UP_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_up_id_8855 - soft IFF_UP interface flag id.
 *
 * Always returns 0x1 (IFF_UP). Catalog id only; does not query real
 * interface flags. Does not call libc. No parent wires.
 */
uint32_t
gj_if_up_id_8855(void)
{
	(void)NULL;
	return b8855_up_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_if_up_id_8855(void)
    __attribute__((alias("gj_if_up_id_8855")));
