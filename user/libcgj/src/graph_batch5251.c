/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5251: pure-value NVMe SQE size (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_sqe_size_u(void);
 *     - Return the NVMe submission queue entry size in bytes (64).
 *   uint32_t __gj_nvme_sqe_size_u  (alias)
 *   __libcgj_batch5251_marker = "libcgj-batch5251"
 *
 * Exclusive continuum CREATE-ONLY (5251-5260: NVMe unique —
 * nvme_sqe_size_u, nvme_cqe_size_u, nvme_admin_qid_u, nvme_page_size_u,
 * nvme_opcode_flush_u, nvme_opcode_write_u, nvme_opcode_read_u,
 * nvme_status_ok_u, nvme_cid_next_u, batch_id_5260). Unique
 * gj_nvme_sqe_size_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5251_marker[] = "libcgj-batch5251";

/* NVMe submission queue entry size (bytes). */
#define B5251_SQE_SIZE  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5251_sqe_size(void)
{
	return B5251_SQE_SIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_sqe_size_u - NVMe submission queue entry size in bytes.
 *
 * Always returns 64. Soft continuum constant for SQ layout; does not
 * program hardware. Self-contained; no parent wires.
 */
uint32_t
gj_nvme_sqe_size_u(void)
{
	(void)NULL;
	return b5251_sqe_size();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_sqe_size_u(void)
    __attribute__((alias("gj_nvme_sqe_size_u")));
