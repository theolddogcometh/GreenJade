/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5257: pure-value NVMe Read opcode (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_opcode_read_u(void);
 *     - Return the NVMe NVM command set Read opcode (2).
 *   uint32_t __gj_nvme_opcode_read_u  (alias)
 *   __libcgj_batch5257_marker = "libcgj-batch5257"
 *
 * Exclusive continuum CREATE-ONLY (5251-5260: NVMe unique —
 * nvme_sqe_size_u, nvme_cqe_size_u, nvme_admin_qid_u, nvme_page_size_u,
 * nvme_opcode_flush_u, nvme_opcode_write_u, nvme_opcode_read_u,
 * nvme_status_ok_u, nvme_cid_next_u, batch_id_5260). Unique
 * gj_nvme_opcode_read_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5257_marker[] = "libcgj-batch5257";

/* NVMe NVM Command Set: Read opcode. */
#define B5257_OPC_READ  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5257_opc_read(void)
{
	return B5257_OPC_READ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_opcode_read_u - NVMe NVM Read opcode.
 *
 * Always returns 2. Soft continuum constant; does not issue I/O.
 * Self-contained; no parent wires.
 */
uint32_t
gj_nvme_opcode_read_u(void)
{
	(void)NULL;
	return b5257_opc_read();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_opcode_read_u(void)
    __attribute__((alias("gj_nvme_opcode_read_u")));
