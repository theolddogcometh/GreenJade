/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3130: exclusive checksum/net batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3130(void);
 *     - Returns the compile-time graph batch number for this TU (3130).
 *   uint32_t __gj_batch_id_3130  (alias)
 *   __libcgj_batch3130_marker = "libcgj-batch3130"
 *
 * Exclusive checksum/net CREATE-ONLY wave (3121-3130): crc8_atm_u_soft,
 * crc8_maxim_u_soft, checksum_add8_u, checksum_add16_u,
 * ones_complement16_u, ip_checksum_fold_u, net_port_ok_u, net_mtu_ok_u,
 * net_ttl_ok_u, batch_id_3130. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3130_marker[] = "libcgj-batch3130";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3130_id(void)
{
	return 3130u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3130 - report this TU's graph batch number.
 *
 * Always returns 3130. No parent wires.
 */
uint32_t
gj_batch_id_3130(void)
{
	(void)NULL;
	return b3130_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3130(void)
    __attribute__((alias("gj_batch_id_3130")));
