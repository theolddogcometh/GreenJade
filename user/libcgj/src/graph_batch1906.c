/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1906: display refresh-rate soft validity.
 *
 * Surface (unique symbols):
 *   int gj_refresh_hz_ok(int hz);
 *     — Return 1 if hz is in the closed range [24, 360] (typical
 *       desktop/monitor refresh rates from cinema floor to high-end
 *       gaming panels), else 0.
 *   int __gj_refresh_hz_ok  (alias)
 *   __libcgj_batch1906_marker = "libcgj-batch1906"
 *
 * Desktop window exclusive pure-data wave (1901–1910). Distinct from
 * gj_fps_limit_ok (batch1869). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1906_marker[] = "libcgj-batch1906";

/* Soft closed range for desktop monitor refresh rates (Hz). */
#define B1906_HZ_MIN  24
#define B1906_HZ_MAX  360

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1906_hz_ok(int nHz)
{
	if (nHz < B1906_HZ_MIN) {
		return 0;
	}
	if (nHz > B1906_HZ_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_refresh_hz_ok — 1 iff hz is in [24, 360].
 *
 * hz: candidate vertical refresh rate in hertz (signed so negatives
 * reject cleanly). Soft range hint only.
 */
int
gj_refresh_hz_ok(int nHz)
{
	(void)NULL;
	return b1906_hz_ok(nHz);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_refresh_hz_ok(int nHz)
    __attribute__((alias("gj_refresh_hz_ok")));
