/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5254: pure-value NVMe page size (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_page_size_u(void);
 *     - Return the conventional NVMe host page size in bytes (4096).
 *   uint32_t __gj_nvme_page_size_u  (alias)
 *   __libcgj_batch5254_marker = "libcgj-batch5254"
 *
 * Exclusive continuum CREATE-ONLY (5251-5260: NVMe unique —
 * nvme_sqe_size_u, nvme_cqe_size_u, nvme_admin_qid_u, nvme_page_size_u,
 * nvme_opcode_flush_u, nvme_opcode_write_u, nvme_opcode_read_u,
 * nvme_status_ok_u, nvme_cid_next_u, batch_id_5260). Unique
 * gj_nvme_page_size_u surface only; no multi-def. Distinct from
 * gj_mm_page_size_u (batch5032) and gj_page_size (batch988) —
 * unique gj_nvme_page_size_u only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5254_marker[] = "libcgj-batch5254";

/* Conventional host memory page size used with NVMe PRP/SGL (bytes). */
#define B5254_PAGE_SIZE  4096u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5254_page_size(void)
{
	return B5254_PAGE_SIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_page_size_u - conventional NVMe host page size in bytes.
 *
 * Always returns 4096 (4 KiB). Soft continuum constant for PRP/SGL
 * math; does not probe hardware. Self-contained; no parent wires.
 */
uint32_t
gj_nvme_page_size_u(void)
{
	(void)NULL;
	return b5254_page_size();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_page_size_u(void)
    __attribute__((alias("gj_nvme_page_size_u")));
