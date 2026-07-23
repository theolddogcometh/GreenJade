/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5260: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5260(void);
 *     - Returns the compile-time graph batch number for this TU (5260).
 *   uint32_t __gj_batch_id_5260  (alias)
 *   __libcgj_batch5260_marker = "libcgj-batch5260"
 *
 * Exclusive continuum CREATE-ONLY (5251-5260: NVMe unique —
 * nvme_sqe_size_u, nvme_cqe_size_u, nvme_admin_qid_u, nvme_page_size_u,
 * nvme_opcode_flush_u, nvme_opcode_write_u, nvme_opcode_read_u,
 * nvme_status_ok_u, nvme_cid_next_u, batch_id_5260).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5260_marker[] = "libcgj-batch5260";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5260_id(void)
{
	return 5260u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5260 - report this TU's graph batch number.
 *
 * Always returns 5260. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5260(void)
{
	(void)NULL;
	return b5260_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5260(void)
    __attribute__((alias("gj_batch_id_5260")));
