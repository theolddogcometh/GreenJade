/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5253: pure-value NVMe admin queue id (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_admin_qid_u(void);
 *     - Return the NVMe admin queue identifier (always 0).
 *   uint32_t __gj_nvme_admin_qid_u  (alias)
 *   __libcgj_batch5253_marker = "libcgj-batch5253"
 *
 * Exclusive continuum CREATE-ONLY (5251-5260: NVMe unique —
 * nvme_sqe_size_u, nvme_cqe_size_u, nvme_admin_qid_u, nvme_page_size_u,
 * nvme_opcode_flush_u, nvme_opcode_write_u, nvme_opcode_read_u,
 * nvme_status_ok_u, nvme_cid_next_u, batch_id_5260). Unique
 * gj_nvme_admin_qid_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5253_marker[] = "libcgj-batch5253";

/* NVMe admin submission/completion queue identifier. */
#define B5253_ADMIN_QID  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5253_admin_qid(void)
{
	return B5253_ADMIN_QID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_admin_qid_u - NVMe admin queue identifier.
 *
 * Always returns 0. Soft continuum constant; does not program hardware.
 * Self-contained; no parent wires.
 */
uint32_t
gj_nvme_admin_qid_u(void)
{
	(void)NULL;
	return b5253_admin_qid();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_admin_qid_u(void)
    __attribute__((alias("gj_nvme_admin_qid_u")));
