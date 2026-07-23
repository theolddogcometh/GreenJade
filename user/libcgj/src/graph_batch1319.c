/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1319: Manual-reset style event clear (flag = 0).
 *
 * Surface (unique symbols):
 *   void gj_event_clear(uint32_t *flag);
 *     — Store 0 into *flag. NULL is a no-op.
 *   void __gj_event_clear  (alias)
 *   __libcgj_batch1319_marker = "libcgj-batch1319"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1319_marker[] = "libcgj-batch1319";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_event_clear — clear an event flag (cooperative / external-sync).
 *
 * Plain store of 0; no atomics. NULL is ignored.
 */
void
gj_event_clear(uint32_t *pFlag)
{
	if (pFlag == NULL) {
		return;
	}
	*pFlag = 0u;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_event_clear(uint32_t *pFlag)
    __attribute__((alias("gj_event_clear")));

