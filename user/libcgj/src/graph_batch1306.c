/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1306: Usable SPSC ring capacity (power-of-two free-running design).
 *
 * Surface (unique symbols):
 *   size_t gj_spsc_capacity_usable(size_t cap);
 *     — For free-running SPSC (batch883/884 / batch185 style), usable
 *       slot count equals cap when cap is a positive power of two;
 *       else 0 (invalid capacity).
 *   size_t __gj_spsc_capacity_usable  (alias)
 *   __libcgj_batch1306_marker = "libcgj-batch1306"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stddef.h>

const char __libcgj_batch1306_marker[] = "libcgj-batch1306";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spsc_capacity_usable — how many elements a free-running SPSC can hold.
 *
 * Free-running write/read indices use the full power-of-two buffer; full
 * when (w - r) == cap. Non-pot2 or zero capacity is rejected (return 0).
 */
size_t
gj_spsc_capacity_usable(size_t uCap)
{
	if (uCap == 0u) {
		return 0u;
	}
	if ((uCap & (uCap - 1u)) != 0u) {
		return 0u;
	}
	return uCap;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_spsc_capacity_usable(size_t uCap)
    __attribute__((alias("gj_spsc_capacity_usable")));

