/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch780: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_780(void);
 *     — Returns the compile-time graph batch number for this TU (780).
 *   uint32_t __gj_batch_id_780  (alias)
 *   __libcgj_batch780_marker = "libcgj-batch780"
 *
 * Does NOT define gj_batch_id / gj_batch_id_550 / _600 / _630 / _650 /
 * _670 / _680 / _690 / _700 / _710 / _720 / _730 / _740 / _750 / _760 /
 * _770 — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch780_marker[] = "libcgj-batch780";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_780 — report this TU's graph batch number.
 *
 * Always returns 780. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_780(void)
{
	return 780u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_780(void)
    __attribute__((alias("gj_batch_id_780")));
