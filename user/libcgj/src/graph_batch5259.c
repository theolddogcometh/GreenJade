/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5259: pure-value NVMe command-id next (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_cid_next_u(uint32_t cid);
 *     - Return the next NVMe command identifier after cid, wrapping
 *       in the 16-bit CID domain ((cid + 1) & 0xffff).
 *   uint32_t __gj_nvme_cid_next_u  (alias)
 *   __libcgj_batch5259_marker = "libcgj-batch5259"
 *
 * Exclusive continuum CREATE-ONLY (5251-5260: NVMe unique —
 * nvme_sqe_size_u, nvme_cqe_size_u, nvme_admin_qid_u, nvme_page_size_u,
 * nvme_opcode_flush_u, nvme_opcode_write_u, nvme_opcode_read_u,
 * nvme_status_ok_u, nvme_cid_next_u, batch_id_5260). Unique
 * gj_nvme_cid_next_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5259_marker[] = "libcgj-batch5259";

/* NVMe command identifier is a 16-bit field. */
#define B5259_CID_MASK  0xffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5259_cid_next(uint32_t u32Cid)
{
	return (u32Cid + 1u) & B5259_CID_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_cid_next_u - next NVMe command identifier after cid.
 *
 * cid: current command id (only low 16 bits matter)
 *
 * Returns (cid + 1) & 0xffff. Self-contained; no parent wires.
 */
uint32_t
gj_nvme_cid_next_u(uint32_t u32Cid)
{
	(void)NULL;
	return b5259_cid_next(u32Cid);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_cid_next_u(uint32_t u32Cid)
    __attribute__((alias("gj_nvme_cid_next_u")));
