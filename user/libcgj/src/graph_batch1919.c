/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1919: multitouch slot index soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_touch_slot_ok(int slot);
 *     — Soft validity: return 1 if slot is in [0, 16) (common
 *       ABS_MT_SLOT soft upper bound used by many input stacks),
 *       else 0. Not a guarantee a device exposes that many slots.
 *   int __gj_touch_slot_ok  (alias)
 *   __libcgj_batch1919_marker = "libcgj-batch1919"
 *
 * Input-device exclusive pure-data wave (1911–1919; milestone 1920).
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno,
 * no libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1919_marker[] = "libcgj-batch1919";

/* Soft upper bound for multitouch slot indices. */
#define B1919_SLOT_SOFT  16

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if slot is in [0, B1919_SLOT_SOFT). */
static int
b1919_slot_ok(int nSlot)
{
	if (nSlot < 0) {
		return 0;
	}
	return (nSlot < B1919_SLOT_SOFT) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_touch_slot_ok — soft ok if 0 <= slot < 16.
 *
 * slot: multitouch slot index (signed; negatives rejected)
 * Returns 1 when in soft range, else 0.
 */
int
gj_touch_slot_ok(int nSlot)
{
	(void)NULL;
	return b1919_slot_ok(nSlot);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_touch_slot_ok(int nSlot)
    __attribute__((alias("gj_touch_slot_ok")));
