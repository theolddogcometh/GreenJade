/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6237: HDR HLG system-gamma milli stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdr_hlg_system_gamma_milli_6237(uint32_t lw_nits);
 *     - Soft integer approximation of BT.2100 HLG system gamma:
 *         gamma ≈ 1.2 + 0.42 * log10(Lw / 1000)
 *       Returned as milli-gamma (1200 = 1.200). For Lw outside the
 *       soft [100, 2000] window, returns the 1000-nit reference 1200.
 *       Uses a tiny integer log10 table over discrete Lw steps — not a
 *       full math lib.
 *   uint32_t __gj_hdr_hlg_system_gamma_milli_6237  (alias)
 *   __libcgj_batch6237_marker = "libcgj-batch6237"
 *
 * Exclusive continuum CREATE-ONLY (6231-6240: HDR PQ/HLG transfer stubs).
 * Unique gj_hdr_hlg_system_gamma_milli_6237 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6237_marker[] = "libcgj-batch6237";

#define B6237_LW_MIN_NITS       100u
#define B6237_LW_MAX_NITS       2000u
#define B6237_GAMMA_REF_MILLI   1200u  /* 1.2 at Lw = 1000 nits */
#define B6237_LOG_SCALE         420u   /* 0.42 * 1000 milli factor */

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft integer log10(Lw/1000) * 1000 for discrete Lw buckets.
 * Returns milli-log10 of (Lw/1000): negative below 1000, zero at 1000,
 * positive above. Table covers the soft policy window only.
 */
static int32_t
b6237_log10_ratio_milli(uint32_t u32LwNits)
{
	/* Piecewise linear-ish buckets around reference 1000 nits. */
	if (u32LwNits <= 100u) {
		return -1000; /* log10(0.1) = -1.0 */
	}
	if (u32LwNits <= 200u) {
		return -700;  /* ~log10(0.2) */
	}
	if (u32LwNits <= 400u) {
		return -400;  /* ~log10(0.4) */
	}
	if (u32LwNits <= 700u) {
		return -150;  /* ~log10(0.7) */
	}
	if (u32LwNits <= 1000u) {
		return 0;
	}
	if (u32LwNits <= 1400u) {
		return 150;   /* ~log10(1.4) */
	}
	if (u32LwNits <= 1700u) {
		return 230;   /* ~log10(1.7) */
	}
	/* up to 2000 */
	return 300;           /* ~log10(2.0) */
}

static uint32_t
b6237_system_gamma_milli(uint32_t u32LwNits)
{
	int32_t i32LogMilli;
	int32_t i32GammaMilli;

	if (u32LwNits < B6237_LW_MIN_NITS || u32LwNits > B6237_LW_MAX_NITS) {
		return B6237_GAMMA_REF_MILLI;
	}
	i32LogMilli = b6237_log10_ratio_milli(u32LwNits);
	/* gamma_milli = 1200 + 0.42 * log10_milli  (0.42 * x ≈ 420*x/1000) */
	i32GammaMilli = (int32_t)B6237_GAMMA_REF_MILLI +
	    (i32LogMilli * (int32_t)B6237_LOG_SCALE) / 1000;
	if (i32GammaMilli < 0) {
		return 0u;
	}
	return (uint32_t)i32GammaMilli;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_hlg_system_gamma_milli_6237 - soft HLG system gamma in milli units.
 *
 * lw_nits: nominal peak white Lw (cd/m^2).
 * Returns milli-gamma (e.g. 1200 ≈ 1.2 at 1000 nits). Soft integer OOTF
 * helper for the HLG transfer continuum; not a full BT.2100 OOTF.
 * No parent wires.
 */
uint32_t
gj_hdr_hlg_system_gamma_milli_6237(uint32_t u32LwNits)
{
	(void)NULL;
	return b6237_system_gamma_milli(u32LwNits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdr_hlg_system_gamma_milli_6237(uint32_t u32LwNits)
    __attribute__((alias("gj_hdr_hlg_system_gamma_milli_6237")));
