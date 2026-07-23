/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch87: SSE4.2-aware CRC-32C (Castagnoli) helpers.
 *
 * Already present elsewhere (NOT redefined here — avoid multi-def):
 *   crc32c / __crc32c                              (graph_batch39.c)
 *
 * This TU adds only unique symbols:
 *   int gj_cpu_has_sse4_2(void);
 *     — runtime CPUID leaf 1, ECX bit 20 (SSE4.2). Cached after first call.
 *   uint32_t crc32c_hw(uint32_t crc, const void *buf, size_t len);
 *     — SSE4.2 CRC32 instruction path when available; else same table
 *       algorithm as batch39 crc32c (poly 0x82F63B78 reflected).
 *       Chaining matches crc32c: crc_out = crc32c_hw(crc_in, buf, len).
 *   uint32_t crc32c_auto(const void *buf, size_t len);
 *     — one-shot: crc32c_hw(0, buf, len).
 *   uint32_t crc32c_update(uint32_t crc, const void *buf, size_t len);
 *     — soft deepen alias of incremental path (named for chaining clarity).
 *   __gj_cpu_has_sse4_2 / __crc32c_hw / __crc32c_auto / __crc32c_update
 *   __libcgj_batch87_marker = "libcgj-batch87"
 *
 * Compiles freestanding with -msse2 only (CRC32 via inline asm, not
 * -msse4.2 intrinsics). Clean-room Castagnoli / Intel CRC32 usage.
 *
 * Soft deepen: null-buf short-circuit preserves prior crc (chaining),
 * zero-length leaves crc unchanged, underscored aliases.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch87_marker[] = "libcgj-batch87";

/* ---- CPUID / SSE4.2 detect --------------------------------------------- */

/* -1 unknown, 0 absent, 1 present */
static int s_b87_sse42 = -1;

static void
b87_cpuid(unsigned uLeaf, unsigned uSub,
    unsigned *pEax, unsigned *pEbx, unsigned *pEcx, unsigned *pEdx)
{
	unsigned uEax = 0u;
	unsigned uEbx = 0u;
	unsigned uEcx = 0u;
	unsigned uEdx = 0u;

#if defined(__x86_64__) || defined(__i386__)
	__asm__ volatile("cpuid"
	    : "=a"(uEax), "=b"(uEbx), "=c"(uEcx), "=d"(uEdx)
	    : "a"(uLeaf), "c"(uSub)
	    : "memory");
#else
	(void)uLeaf;
	(void)uSub;
#endif
	if (pEax != NULL) {
		*pEax = uEax;
	}
	if (pEbx != NULL) {
		*pEbx = uEbx;
	}
	if (pEcx != NULL) {
		*pEcx = uEcx;
	}
	if (pEdx != NULL) {
		*pEdx = uEdx;
	}
}

/*
 * SSE4.2 is CPUID.(EAX=1):ECX[20].
 * Returns 1 if present, 0 otherwise. Result is cached.
 */
int
gj_cpu_has_sse4_2(void)
{
	unsigned uEax;
	unsigned uEcx;

	if (s_b87_sse42 >= 0) {
		return s_b87_sse42;
	}
	/* Require a basic feature leaf first. */
	b87_cpuid(0u, 0u, &uEax, NULL, NULL, NULL);
	if (uEax < 1u) {
		s_b87_sse42 = 0;
		return 0;
	}
	b87_cpuid(1u, 0u, NULL, NULL, &uEcx, NULL);
	/* ECX bit 20 = SSE4.2 */
	s_b87_sse42 = ((uEcx & (1u << 20)) != 0u) ? 1 : 0;
	return s_b87_sse42;
}

/* ---- software table (Castagnoli; matches batch39 crc32c) --------------- */

static uint32_t s_b87_crc32c_tab[256];
static int s_b87_tab_ready;

static void
b87_crc32c_init_tab(void)
{
	/* Castagnoli poly 0x1EDC6F41 reflected → 0x82F63B78 */
	const uint32_t uPoly = 0x82f63b78u;
	unsigned i;
	unsigned b;

	if (s_b87_tab_ready) {
		return;
	}
	for (i = 0; i < 256u; i++) {
		uint32_t uC = (uint32_t)i;

		for (b = 0; b < 8u; b++) {
			if ((uC & 1u) != 0u) {
				uC = uPoly ^ (uC >> 1);
			} else {
				uC >>= 1;
			}
		}
		s_b87_crc32c_tab[i] = uC;
	}
	s_b87_tab_ready = 1;
}

static uint32_t
b87_crc32c_sw(uint32_t uCrc, const unsigned char *pBuf, size_t cb)
{
	size_t i;

	b87_crc32c_init_tab();
	uCrc = ~uCrc;
	for (i = 0; i < cb; i++) {
		uCrc = s_b87_crc32c_tab[(uCrc ^ pBuf[i]) & 0xffu] ^ (uCrc >> 8);
	}
	return ~uCrc;
}

/* ---- hardware CRC32 (SSE4.2); inline asm so -msse2 TU builds ---------- */

#if defined(__x86_64__) || defined(__i386__)

static uint32_t
b87_crc32b(uint32_t uCrc, unsigned char ch)
{
	__asm__("crc32b %1, %0" : "+r"(uCrc) : "rm"(ch));
	return uCrc;
}

static uint32_t
b87_crc32l(uint32_t uCrc, uint32_t uWord)
{
	__asm__("crc32l %1, %0" : "+r"(uCrc) : "rm"(uWord));
	return uCrc;
}

#if defined(__x86_64__)
static uint32_t
b87_crc32q(uint32_t uCrc, uint64_t uQ)
{
	uint64_t uAcc = (uint64_t)uCrc;

	__asm__("crc32q %1, %0" : "+r"(uAcc) : "rm"(uQ));
	return (uint32_t)uAcc;
}
#endif

static uint32_t
b87_crc32c_hw_path(uint32_t uCrc, const unsigned char *pBuf, size_t cb)
{
	uCrc = ~uCrc;

#if defined(__x86_64__)
	/* Align to 8 bytes for crc32q. */
	while (cb > 0u && (((uintptr_t)pBuf) & 7u) != 0u) {
		uCrc = b87_crc32b(uCrc, *pBuf);
		pBuf++;
		cb--;
	}
	while (cb >= 8u) {
		uint64_t uQ;

		__builtin_memcpy(&uQ, pBuf, 8);
		uCrc = b87_crc32q(uCrc, uQ);
		pBuf += 8;
		cb -= 8u;
	}
	if (cb >= 4u) {
		uint32_t uW;

		__builtin_memcpy(&uW, pBuf, 4);
		uCrc = b87_crc32l(uCrc, uW);
		pBuf += 4;
		cb -= 4u;
	}
#else
	while (cb > 0u && (((uintptr_t)pBuf) & 3u) != 0u) {
		uCrc = b87_crc32b(uCrc, *pBuf);
		pBuf++;
		cb--;
	}
	while (cb >= 4u) {
		uint32_t uW;

		__builtin_memcpy(&uW, pBuf, 4);
		uCrc = b87_crc32l(uCrc, uW);
		pBuf += 4;
		cb -= 4u;
	}
#endif
	while (cb > 0u) {
		uCrc = b87_crc32b(uCrc, *pBuf);
		pBuf++;
		cb--;
	}
	return ~uCrc;
}

#else /* !x86 */

static uint32_t
b87_crc32c_hw_path(uint32_t uCrc, const unsigned char *pBuf, size_t cb)
{
	/* No CRC32 insn — fall through to software. */
	return b87_crc32c_sw(uCrc, pBuf, cb);
}

#endif

/* ---- public API -------------------------------------------------------- */

/*
 * Incremental Castagnoli CRC-32C. Uses SSE4.2 when gj_cpu_has_sse4_2()
 * is true; otherwise the reflected table path (identical to batch39
 * crc32c chaining).
 *
 * Soft deepen: NULL buf with cb == 0 leaves uCrc unchanged (chain end);
 * NULL buf with cb != 0 returns 0 (reject). cb == 0 with non-NULL is
 * identity on uCrc.
 */
uint32_t
crc32c_hw(uint32_t uCrc, const void *pBuf, size_t cb)
{
	const unsigned char *p = (const unsigned char *)pBuf;

	if (cb == 0u) {
		return uCrc;
	}
	if (p == NULL) {
		return 0u;
	}
	if (gj_cpu_has_sse4_2() != 0) {
		return b87_crc32c_hw_path(uCrc, p, cb);
	}
	return b87_crc32c_sw(uCrc, p, cb);
}

/*
 * One-shot CRC-32C of [buf, buf+len). Equivalent to crc32c_hw(0, buf, len).
 * Check("123456789") == 0xe3069283 (same as batch39 crc32c).
 */
uint32_t
crc32c_auto(const void *pBuf, size_t cb)
{
	return crc32c_hw(0u, pBuf, cb);
}

/*
 * crc32c_update — soft deepen incremental name (identical to crc32c_hw).
 * Prefer this at call sites that read as "running = update(running, …)".
 */
uint32_t
crc32c_update(uint32_t uCrc, const void *pBuf, size_t cb)
{
	return crc32c_hw(uCrc, pBuf, cb);
}

int __gj_cpu_has_sse4_2(void) __attribute__((alias("gj_cpu_has_sse4_2")));
uint32_t __crc32c_hw(uint32_t uCrc, const void *pBuf, size_t cb)
    __attribute__((alias("crc32c_hw")));
uint32_t __crc32c_auto(const void *pBuf, size_t cb)
    __attribute__((alias("crc32c_auto")));
uint32_t __crc32c_update(uint32_t uCrc, const void *pBuf, size_t cb)
    __attribute__((alias("crc32c_update")));

