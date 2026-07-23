/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1660: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1660(void);
 *     — Returns the compile-time graph batch number for this TU (1660).
 *       Milestone for the exclusive lightweight file-magic series
 *       (batches 1651–1659: ELF/PE/Mach-O/WASM/PDF/ZIP/gzip/zstd/xz).
 *   uint32_t __gj_batch_id_1660  (alias)
 *   __libcgj_batch1660_marker = "libcgj-batch1660"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1660_marker[] = "libcgj-batch1660";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1660 — report this TU's graph batch number.
 *
 * Always returns 1660. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1660(void)
{
	return 1660u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1660(void)
    __attribute__((alias("gj_batch_id_1660")));
