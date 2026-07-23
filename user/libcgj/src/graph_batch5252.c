/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5252: pure-value NVMe CQE size (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_cqe_size_u(void);
 *     - Return the NVMe completion queue entry size in bytes (16).
 *   uint32_t __gj_nvme_cqe_size_u  (alias)
 *   __libcgj_batch5252_marker = "libcgj-batch5252"
 *
 * Exclusive continuum CREATE-ONLY (5251-5260: NVMe unique —
 * nvme_sqe_size_u, nvme_cqe_size_u, nvme_admin_qid_u, nvme_page_size_u,
 * nvme_opcode_flush_u, nvme_opcode_write_u, nvme_opcode_read_u,
 * nvme_status_ok_u, nvme_cid_next_u, batch_id_5260). Unique
 * gj_nvme_cqe_size_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5252_marker[] = "libcgj-batch5252";

/* NVMe completion queue entry size (bytes). */
#define B5252_CQE_SIZE  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5252_cqe_size(void)
{
	return B5252_CQE_SIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_cqe_size_u - NVMe completion queue entry size in bytes.
 *
 * Always returns 16. Soft continuum constant for CQ layout; does not
 * program hardware. Self-contained; no parent wires.
 */
uint32_t
gj_nvme_cqe_size_u(void)
{
	(void)NULL;
	return b5252_cqe_size();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_cqe_size_u(void)
    __attribute__((alias("gj_nvme_cqe_size_u")));
