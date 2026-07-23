/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8857: interface MTU range soft-check (uint).
 *
 * Surface (unique symbols):
 *   uint32_t gj_if_mtu_ok_u_8857(uint32_t mtu);
 *     - Return 1 if mtu is a plausible link MTU in [68, 65535]
 *       (IPv4 min fragmentable through max IP total length), else 0.
 *   uint32_t __gj_if_mtu_ok_u_8857  (alias)
 *   __libcgj_batch8857_marker = "libcgj-batch8857"
 *
 * Exclusive continuum CREATE-ONLY (8851-8860: if_nametoindex stubs —
 * if_name_ok_u, if_index_ok_u, if_loop_id, if_eth_ok_u, if_up_id,
 * if_running_id, if_mtu_ok_u, if_mac_len_id, if_ready_u,
 * batch_id_8860). Unique gj_if_mtu_ok_u_8857 surface only; no multi-def.
 * Distinct from gj_if_mtu_ok_5763 (int return, batch5763). Soft MTU
 * gate only; no if_nametoindex. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8857_marker[] = "libcgj-batch8857";

/* IPv4 min fragmentable MTU .. max IP total length. */
#define B8857_MTU_MIN  68u
#define B8857_MTU_MAX  65535u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8857_mtu_ok(uint32_t u32Mtu)
{
	if (u32Mtu < B8857_MTU_MIN) {
		return 0u;
	}
	if (u32Mtu > B8857_MTU_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_mtu_ok_u_8857 - 1 if mtu is a plausible interface MTU.
 *
 * mtu: candidate maximum transmission unit in octets
 *
 * Returns 1 when mtu is in [68, 65535], else 0. Soft if_nametoindex-
 * adjacent MTU gate; does not call libc. No parent wires.
 */
uint32_t
gj_if_mtu_ok_u_8857(uint32_t u32Mtu)
{
	(void)NULL;
	return b8857_mtu_ok(u32Mtu);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_if_mtu_ok_u_8857(uint32_t u32Mtu)
    __attribute__((alias("gj_if_mtu_ok_u_8857")));
