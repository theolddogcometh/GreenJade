/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5258: pure-value NVMe status-OK predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_status_ok_u(uint32_t status);
 *     - Return 1 if (status & 0x7ff) == 0 (SC+SCT success), else 0.
 *   uint32_t __gj_nvme_status_ok_u  (alias)
 *   __libcgj_batch5258_marker = "libcgj-batch5258"
 *
 * Exclusive continuum CREATE-ONLY (5251-5260: NVMe unique —
 * nvme_sqe_size_u, nvme_cqe_size_u, nvme_admin_qid_u, nvme_page_size_u,
 * nvme_opcode_flush_u, nvme_opcode_write_u, nvme_opcode_read_u,
 * nvme_status_ok_u, nvme_cid_next_u, batch_id_5260). Unique
 * gj_nvme_status_ok_u surface only; no multi-def. Distinct from
 * gj_virtio_status_ok_u (batch5235) — unique gj_nvme_status_ok_u only.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5258_marker[] = "libcgj-batch5258";

/* NVMe CQE status field mask: SC (7:0) | SCT (10:8) — phase excluded. */
#define B5258_STATUS_MASK  0x7ffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5258_status_ok(uint32_t u32Status)
{
	if ((u32Status & B5258_STATUS_MASK) == 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_status_ok_u - 1 if NVMe CQE status SC+SCT bits are success.
 *
 * status: raw status word (or DW3 status field); bits [10:0] tested
 *
 * Returns 1 when (status & 0x7ff) == 0, else 0. Self-contained; no
 * parent wires.
 */
uint32_t
gj_nvme_status_ok_u(uint32_t u32Status)
{
	(void)NULL;
	return b5258_status_ok(u32Status);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_status_ok_u(uint32_t u32Status)
    __attribute__((alias("gj_nvme_status_ok_u")));
