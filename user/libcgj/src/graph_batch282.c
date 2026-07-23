/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch282: bump (linear) arena over a caller buffer.
 * Clean-room freestanding pure C — integer/pointer only. Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party arena source was copied.
 *
 * Existing malloc / free / realloc / posix_memalign / aligned_alloc live in
 * malloc.c / graph_batch15.c and are NOT redefined here.
 *
 * This TU adds only unique symbols:
 *   void  gj_bump_init(size_t *used, size_t cap);
 *     — clear the watermark (*used = 0). cap is accepted for API symmetry
 *       with the alloc path (caller owns the buffer); ignored on success.
 *   void *gj_bump_alloc(uint8_t *base, size_t *used, size_t cap,
 *                        size_t n, size_t align);
 *     — bump-allocate n bytes from [base, base+cap) at alignment align
 *       (power of two, or 0/1 = byte). Advances *used. Returns NULL on
 *       OOM / bad args / overflow. Does not zero memory.
 *   __gj_bump_init / __gj_bump_alloc  (aliases)
 *   __libcgj_batch282_marker = "libcgj-batch282"
 *
 * Notes:
 *   - No free of individual blocks; re-init (or set *used = 0) reclaims.
 *   - No errno (freestanding-safe); failures return NULL.
 *   - align must be 0, 1, or a power of two; other values fail.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch282_marker[] = "libcgj-batch282";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Return 1 if uAlign is 0, 1, or a power of two; else 0.
 * 0 is treated as "byte align" (same as 1) by the allocator.
 */
static int
b282_align_ok(size_t uAlign)
{
	if (uAlign == 0u || uAlign == 1u) {
		return 1;
	}
	return (uAlign & (uAlign - 1u)) == 0u ? 1 : 0;
}

/*
 * Round uOff up to the next multiple of uAlign (uAlign is 0/1 or power of 2).
 * Returns 0 and sets *pOut on success; returns -1 on size_t overflow.
 */
static int
b282_align_up(size_t uOff, size_t uAlign, size_t *pOut)
{
	size_t uMask;
	size_t uAdj;

	if (uAlign <= 1u) {
		*pOut = uOff;
		return 0;
	}
	uMask = uAlign - 1u;
	uAdj = (uOff + uMask) & ~uMask;
	/* overflow: (uOff + uMask) wrapped */
	if (uAdj < uOff) {
		return -1;
	}
	*pOut = uAdj;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bump_init — clear the bump watermark over a caller-owned buffer.
 * *used is set to 0. cap is unused (caller retains base/cap); accepted so
 * the init surface matches the alloc parameter set.
 */
void
gj_bump_init(size_t *used, size_t cap)
{
	(void)cap;
	if (used == NULL) {
		return;
	}
	*used = 0u;
}

void __gj_bump_init(size_t *used, size_t cap)
    __attribute__((alias("gj_bump_init")));

/*
 * gj_bump_alloc — bump-allocate n bytes at alignment align from caller
 * buffer [base, base+cap), advancing *used.
 * Returns pointer into the buffer, or NULL on failure.
 */
void *
gj_bump_alloc(uint8_t *base, size_t *used, size_t cap, size_t n, size_t align)
{
	size_t uStart;
	size_t uEnd;
	size_t uCur;

	if (base == NULL || used == NULL) {
		return NULL;
	}
	if (!b282_align_ok(align)) {
		return NULL;
	}
	uCur = *used;
	/* n == 0: still advance to aligned offset, return that pointer
	 * without consuming payload bytes (common bump-arena convention). */
	if (b282_align_up(uCur, align, &uStart) != 0) {
		return NULL;
	}
	if (uStart > cap) {
		return NULL;
	}
	/* end = start + n; reject overflow / OOM */
	if (n > cap - uStart) {
		return NULL;
	}
	uEnd = uStart + n;
	*used = uEnd;
	return (void *)(base + uStart);
}

void *__gj_bump_alloc(uint8_t *base, size_t *used, size_t cap, size_t n,
		       size_t align)
    __attribute__((alias("gj_bump_alloc")));
