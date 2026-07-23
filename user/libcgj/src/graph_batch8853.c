/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8853: loopback interface index id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_if_loop_id_8853(void);
 *     - Return the conventional loopback interface index (1) used as a
 *       soft if_nametoindex catalog id for "lo".
 *   uint32_t __gj_if_loop_id_8853  (alias)
 *   __libcgj_batch8853_marker = "libcgj-batch8853"
 *
 * Exclusive continuum CREATE-ONLY (8851-8860: if_nametoindex stubs —
 * if_name_ok_u, if_index_ok_u, if_loop_id, if_eth_ok_u, if_up_id,
 * if_running_id, if_mtu_ok_u, if_mac_len_id, if_ready_u,
 * batch_id_8860). Unique gj_if_loop_id_8853 surface only; no multi-def.
 * Catalog id only; no if_nametoindex. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8853_marker[] = "libcgj-batch8853";

/* Conventional loopback ifindex (lo is typically ifindex 1). */
#define B8853_IF_LOOP_ID  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8853_loop_id(void)
{
	return B8853_IF_LOOP_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_loop_id_8853 - soft loopback interface index id.
 *
 * Always returns 1 (conventional lo ifindex). Catalog id only; does
 * not probe real interfaces or call if_nametoindex. No parent wires.
 */
uint32_t
gj_if_loop_id_8853(void)
{
	(void)NULL;
	return b8853_loop_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_if_loop_id_8853(void)
    __attribute__((alias("gj_if_loop_id_8853")));
