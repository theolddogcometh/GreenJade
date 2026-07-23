/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2120: milestone 2120 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2120(void);
 *     - Returns the compile-time graph batch number for this TU (2120).
 *   uint32_t gj_graph_milestone_2120(void);
 *     - Returns the current graph milestone revision (2120).
 *   uint32_t __gj_batch_id_2120  (alias)
 *   uint32_t __gj_graph_milestone_2120  (alias)
 *   __libcgj_batch2120_marker = "libcgj-batch2120"
 *
 * Milestone for the post-2100 crc exclusive wave (batches 2111-2119:
 * crc8_atm_u, crc8_maxim_u, crc16_modbus_u, crc16_xmodem_u, crc16_usb_u,
 * crc24_openpgp_u, crc32_jamcrc_u, crc32_mpeg2_u, crc64_ecma_poly).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2120_marker[] = "libcgj-batch2120";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2120_id(void)
{
	return 2120u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2120 - report this TU's graph batch number.
 *
 * Always returns 2120.
 */
uint32_t
gj_batch_id_2120(void)
{
	(void)NULL;
	return b2120_id();
}

/*
 * gj_graph_milestone_2120 - report this TU's graph milestone revision.
 *
 * Always returns 2120 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2120(void)
{
	return b2120_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2120(void)
    __attribute__((alias("gj_batch_id_2120")));

uint32_t __gj_graph_milestone_2120(void)
    __attribute__((alias("gj_graph_milestone_2120")));
