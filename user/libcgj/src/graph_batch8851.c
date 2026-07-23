/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8851: if_nametoindex interface-name length ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_if_name_ok_u_8851(uint32_t name_len);
 *     - Return 1 if name_len is a plausible interface name length for
 *       if_nametoindex (1 .. IFNAMSIZ-1, IFNAMSIZ = 16), else 0.
 *   uint32_t __gj_if_name_ok_u_8851  (alias)
 *   __libcgj_batch8851_marker = "libcgj-batch8851"
 *
 * Exclusive continuum CREATE-ONLY (8851-8860: if_nametoindex stubs —
 * if_name_ok_u, if_index_ok_u, if_loop_id, if_eth_ok_u, if_up_id,
 * if_running_id, if_mtu_ok_u, if_mac_len_id, if_ready_u,
 * batch_id_8860). Unique gj_if_name_ok_u_8851 surface only; no
 * multi-def. Soft length gate only; no if_nametoindex. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8851_marker[] = "libcgj-batch8851";

/* IFNAMSIZ = 16; non-empty name fits in IFNAMSIZ-1 octets + NUL. */
#define B8851_IFNAMSIZ     16u
#define B8851_NAME_LEN_MIN 1u
#define B8851_NAME_LEN_MAX (B8851_IFNAMSIZ - 1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8851_name_ok(uint32_t u32NameLen)
{
	if (u32NameLen < B8851_NAME_LEN_MIN) {
		return 0u;
	}
	if (u32NameLen > B8851_NAME_LEN_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_name_ok_u_8851 - 1 if name_len is a plausible if name length.
 *
 * name_len: candidate interface name length in octets (not including NUL)
 *
 * Returns 1 when name_len is in [1, 15] (IFNAMSIZ-1), else 0. Soft
 * if_nametoindex name-length gate; does not call libc. No parent wires.
 */
uint32_t
gj_if_name_ok_u_8851(uint32_t u32NameLen)
{
	(void)NULL;
	return b8851_name_ok(u32NameLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_if_name_ok_u_8851(uint32_t u32NameLen)
    __attribute__((alias("gj_if_name_ok_u_8851")));
