/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1730: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1730(void);
 *     — Returns the compile-time graph batch number for this TU (1730).
 *   uint32_t __gj_batch_id_1730  (alias)
 *   __libcgj_batch1730_marker = "libcgj-batch1730"
 *
 * Milestone for the ELF/PE/Mach-O lightweight exclusive wave
 * (batches 1721–1729). Does NOT redefine prior batch_id_* symbols —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1730_marker[] = "libcgj-batch1730";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1730 — report this TU's graph batch number.
 *
 * Always returns 1730. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1730(void)
{
	return 1730u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1730(void)
    __attribute__((alias("gj_batch_id_1730")));
