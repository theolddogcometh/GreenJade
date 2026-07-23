/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2096: freestanding epoch-ms plausibility hint.
 *
 * Surface (unique symbols):
 *   int gj_epoch_ms_ok_hint(uint64_t ms);
 *     - Soft plausibility check for a Unix epoch timestamp in
 *       milliseconds. Returns 1 if ms is nonzero and fits in 48 bits
 *       (ULID time field width: 0 < ms <= 0xFFFFFFFFFFFF), else 0.
 *       Not a calendar or leap-second authority - only a width/nil hint.
 *   int __gj_epoch_ms_ok_hint  (alias)
 *   __libcgj_batch2096_marker = "libcgj-batch2096"
 *
 * Post-2000 id exclusive wave (2091-2100). Unique gj_epoch_ms_ok_hint
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2096_marker[] = "libcgj-batch2096";

/* ---- freestanding helpers ---------------------------------------------- */

/* 48-bit unsigned max: ULID timestamp field width. */
static int
b2096_ms_ok(uint64_t u64Ms)
{
	if (u64Ms == 0u) {
		return 0;
	}
	if (u64Ms > 0xffffffffffffull) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_epoch_ms_ok_hint - soft check that ms is a plausible ULID-width epoch.
 *
 * ms: Unix epoch milliseconds candidate
 *
 * Returns 1 when 0 < ms and ms fits in 48 bits, else 0. Zero is treated
 * as unset. Values above 0xFFFFFFFFFFFF cannot be stored in a ULID time
 * field. Does not call libc.
 */
int
gj_epoch_ms_ok_hint(uint64_t u64Ms)
{
	(void)NULL;
	return b2096_ms_ok(u64Ms);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_epoch_ms_ok_hint(uint64_t u64Ms)
    __attribute__((alias("gj_epoch_ms_ok_hint")));
