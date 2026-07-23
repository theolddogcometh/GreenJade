/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6534: NVMe MDTS soft clamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_mdts_clamp_6534(uint32_t bytes, uint32_t mdts_pages);
 *     - Soft max-data-transfer clamp: if mdts_pages == 0 use default
 *       128 KiB; else cap = mdts_pages * 4096; return min(bytes, cap).
 *   uint32_t __gj_nvme_mdts_clamp_6534  (alias)
 *   __libcgj_batch6534_marker = "libcgj-batch6534"
 *
 * Exclusive continuum CREATE-ONLY (6531-6540: NVMe namespace/queue depth
 * stubs — nsid_ok_6531, ns_active_p_6532, lba_count_ok_6533,
 * mdts_clamp_6534, qdepth_ok_6535, sq_size_pack_6536, cq_size_pack_6537,
 * qid_ok_6538, io_queues_ok_6539, ns_qd_ready_6540). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6534_marker[] = "libcgj-batch6534";

#define B6534_PAGE           4096u
#define B6534_DEFAULT_CAP  131072u
#define B6534_U32_MAX  0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6534_mdts_clamp(uint32_t u32Bytes, uint32_t u32Pages)
{
	uint32_t u32Cap;

	if (u32Pages == 0u) {
		u32Cap = B6534_DEFAULT_CAP;
	} else if (u32Pages > (B6534_U32_MAX / B6534_PAGE)) {
		u32Cap = B6534_U32_MAX;
	} else {
		u32Cap = u32Pages * B6534_PAGE;
	}
	if (u32Bytes > u32Cap) {
		return u32Cap;
	}
	return u32Bytes;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_mdts_clamp_6534 - soft NVMe max data transfer size clamp.
 *
 * bytes:      requested transfer size in bytes
 * mdts_pages: soft MDTS page count (0 → default 128 KiB cap)
 *
 * Returns min(bytes, cap). Pure integer. No parent wires.
 */
uint32_t
gj_nvme_mdts_clamp_6534(uint32_t bytes, uint32_t mdts_pages)
{
	(void)NULL;
	return b6534_mdts_clamp(bytes, mdts_pages);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_mdts_clamp_6534(uint32_t bytes, uint32_t mdts_pages)
    __attribute__((alias("gj_nvme_mdts_clamp_6534")));
