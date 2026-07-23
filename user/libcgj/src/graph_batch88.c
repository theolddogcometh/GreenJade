/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch88: POSIX drand48 / erand48 (double return).
 * Clean-room freestanding C with SSE2 (-msse2). Userspace only.
 *
 * Existing surface NOT redefined (avoid multi-def with rand48.c):
 *   lrand48 / nrand48 / mrand48 / jrand48 / srand48 / seed48 / lcong48
 *   *_r integer paths and stub drand48_r / erand48_r (IEEE +0.0 outs)
 *     → rand48.c
 *
 * This TU adds only the missing double-return APIs:
 *   double drand48(void);
 *   double erand48(unsigned short xsubi[3]);
 *   __libcgj_batch88_marker = "libcgj-batch88"
 *
 * State: rand48.c keeps g_x / g_a / g_c static (not exported). This TU
 * bridges via public lrand48 / nrand48 / seed48 so srand48 / lcong48 and
 * drand48 / erand48 share one stream when linked into libcgj.
 *
 * Conversion: after the 48-bit LCG step, return u * 2^-48 ∈ [0.0, 1.0).
 * Double return requires SSE under the SysV AMD64 ABI.
 * No third-party rand48 source was copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch88_marker[] = "libcgj-batch88";

/* Integer rand48 surface (rand48.c) — holds the shared 48-bit state. */
extern long lrand48(void);
extern long nrand48(unsigned short aXsubi[3]);
extern unsigned short *seed48(unsigned short aSeed16v[3]);

/*
 * Own fallback state matching POSIX defaults (used only if seed48 returns
 * NULL — should not happen with our seed48; keeps the TU analyzable alone).
 *   x = 0x1234abcd330e, a = 0x5deece66d, c = 0xb
 */
static unsigned short g_b88_x[3] = { 0x330e, 0xabcd, 0x1234 };
static unsigned short g_b88_c = 0x000b;
static unsigned long long g_b88_a = 0x5deece66dULL;

static void
b88_step(unsigned short x[3], unsigned short c, unsigned long long a)
{
	unsigned long long u;

	u = ((unsigned long long)x[2] << 32) | ((unsigned long long)x[1] << 16) |
	    (unsigned long long)x[0];
	u = a * u + (unsigned long long)c;
	x[0] = (unsigned short)(u & 0xffffULL);
	x[1] = (unsigned short)((u >> 16) & 0xffffULL);
	x[2] = (unsigned short)((u >> 32) & 0xffffULL);
}

/*
 * Map 48-bit state bits to double in [0.0, 1.0).
 * Emits SSE for the FP multiply and double return (SysV AMD64).
 */
static double
b88_x48_to_unit(const unsigned short x[3])
{
	unsigned long long u;

	u = ((unsigned long long)x[2] << 32) | ((unsigned long long)x[1] << 16) |
	    (unsigned long long)x[0];
	return (double)u * 0x1.0p-48;
}

/* --------------------------------------------------------------------------
 * erand48 — step caller-provided 48-bit state, return unit double
 * -------------------------------------------------------------------------- */

double
erand48(unsigned short xsubi[3])
{
	if (xsubi == NULL) {
		return 0.0;
	}

	/*
	 * nrand48 advances xsubi with the global a/c from rand48.c
	 * (respects lcong48). We convert the full 48-bit residue, not the
	 * 31-bit long that nrand48 returns.
	 */
	(void)nrand48(xsubi);
	return b88_x48_to_unit(xsubi);
}

/* --------------------------------------------------------------------------
 * drand48 — step shared global state, return unit double
 * -------------------------------------------------------------------------- */

double
drand48(void)
{
	unsigned short *pSnap;
	unsigned short aX[3];

	/*
	 * Advance the rand48.c global via lrand48, then snapshot with
	 * seed48(NULL) (copies g_x into seed48's static aOld without
	 * changing g_x). Convert the snapshot to a unit double.
	 */
	(void)lrand48();
	pSnap = seed48(NULL);
	if (pSnap == NULL) {
		b88_step(g_b88_x, g_b88_c, g_b88_a);
		return b88_x48_to_unit(g_b88_x);
	}
	aX[0] = pSnap[0];
	aX[1] = pSnap[1];
	aX[2] = pSnap[2];
	g_b88_x[0] = aX[0];
	g_b88_x[1] = aX[1];
	g_b88_x[2] = aX[2];
	return b88_x48_to_unit(aX);
}
