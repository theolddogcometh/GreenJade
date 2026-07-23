/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5256: pure-value NVMe Write opcode (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_opcode_write_u(void);
 *     - Return the NVMe NVM command set Write opcode (1).
 *   uint32_t __gj_nvme_opcode_write_u  (alias)
 *   __libcgj_batch5256_marker = "libcgj-batch5256"
 *
 * Exclusive continuum CREATE-ONLY (5251-5260: NVMe unique —
 * nvme_sqe_size_u, nvme_cqe_size_u, nvme_admin_qid_u, nvme_page_size_u,
 * nvme_opcode_flush_u, nvme_opcode_write_u, nvme_opcode_read_u,
 * nvme_status_ok_u, nvme_cid_next_u, batch_id_5260). Unique
 * gj_nvme_opcode_write_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5256_marker[] = "libcgj-batch5256";

/* NVMe NVM Command Set: Write opcode. */
#define B5256_OPC_WRITE  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5256_opc_write(void)
{
	return B5256_OPC_WRITE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_opcode_write_u - NVMe NVM Write opcode.
 *
 * Always returns 1. Soft continuum constant; does not issue I/O.
 * Self-contained; no parent wires.
 */
uint32_t
gj_nvme_opcode_write_u(void)
{
	(void)NULL;
	return b5256_opc_write();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_opcode_write_u(void)
    __attribute__((alias("gj_nvme_opcode_write_u")));
