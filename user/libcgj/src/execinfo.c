/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * backtrace bring-up: walk frame pointers when available; else 0 frames.
 * Soft deepen: x86_64 + aarch64 FP chains, stricter stack bounds,
 * hex+index symbol strings.
 */
#include <execinfo.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Soft upper bound on walk distance from current SP (bytes). */
#define BT_STACK_MAX (1u << 20)
/* Soft max frames even if nSize is huge. */
#define BT_FRAME_CAP 256

static int
frame_sane(uintptr_t uFp, uintptr_t uSp, uintptr_t uPrev)
{
	if (uFp == 0) {
		return 0;
	}
	/* Frame must sit at/above SP, aligned, not runaway. */
	if (uFp < uSp) {
		return 0;
	}
#if defined(__x86_64__) || defined(__aarch64__)
	if ((uFp & 0x7u) != 0) {
		return 0;
	}
#else
	if ((uFp & 0x3u) != 0) {
		return 0;
	}
#endif
	if (uFp - uSp > BT_STACK_MAX) {
		return 0;
	}
	if (uPrev != 0 && uFp <= uPrev) {
		return 0; /* must strictly unwind up the stack */
	}
	return 1;
}

static int
ret_sane(uintptr_t uRet)
{
	/* Reject null page and clearly non-code low values. */
	if (uRet < 0x1000u) {
		return 0;
	}
	return 1;
}

int
backtrace(void **ppBuffer, int nSize)
{
	int n = 0;
	int nCap;
	uintptr_t *pFp;
	uintptr_t uSp;
	uintptr_t uPrev = 0;

	if (ppBuffer == NULL || nSize <= 0) {
		return 0;
	}
	nCap = nSize;
	if (nCap > BT_FRAME_CAP) {
		nCap = BT_FRAME_CAP;
	}

#if defined(__x86_64__)
	/* x86_64: rbp chain (best-effort; -fomit-frame-pointer may leave junk) */
	__asm__ volatile("movq %%rbp, %0" : "=r"(pFp));
	__asm__ volatile("movq %%rsp, %0" : "=r"(uSp));
#elif defined(__aarch64__)
	/* aarch64: x29 FP, x31 SP */
	__asm__ volatile("mov %0, x29" : "=r"(pFp));
	__asm__ volatile("mov %0, sp" : "=r"(uSp));
#else
	/* Soft: no known FP ABI — empty walk. */
	(void)pFp;
	(void)uSp;
	return 0;
#endif

	while (pFp != NULL && n < nCap) {
		uintptr_t uFp = (uintptr_t)pFp;
		uintptr_t uRet;
		uintptr_t uNext;

		if (!frame_sane(uFp, uSp, uPrev)) {
			break;
		}
		/*
		 * Frame layout (both x86_64 SysV and aarch64 AAPCS64 with FP):
		 *   [0] saved FP / previous frame
		 *   [1] return address
		 */
		uNext = pFp[0];
		uRet = pFp[1];
		if (!ret_sane(uRet)) {
			break;
		}
		ppBuffer[n++] = (void *)uRet;
		uPrev = uFp;
		pFp = (uintptr_t *)uNext;
	}
	return n;
}

/* Format "0xHEX" into aTmp; returns length including NUL. */
static size_t
fmt_hex_ptr(char *aTmp, size_t cb, uintptr_t u)
{
	static const char aHex[] = "0123456789abcdef";
	size_t n = 0;
	int k;
	int cDigits = (int)(sizeof(uintptr_t) * 2);

	if (cb < 4) {
		if (cb > 0) {
			aTmp[0] = '\0';
		}
		return 1;
	}
	aTmp[n++] = '0';
	aTmp[n++] = 'x';
	for (k = cDigits - 1; k >= 0 && n + 1 < cb; k--) {
		aTmp[n++] = aHex[(u >> (k * 4)) & 0xfu];
	}
	aTmp[n] = '\0';
	return n + 1;
}

char **
backtrace_symbols(void *const *ppBuffer, int nSize)
{
	char **pp;
	int i;
	size_t cb;
	/* " [#NN] 0x..." up to ~40 bytes per frame */
	const size_t cbPer = 48u;

	if (ppBuffer == NULL || nSize <= 0) {
		return NULL;
	}
	if (nSize > BT_FRAME_CAP) {
		nSize = BT_FRAME_CAP;
	}
	cb = (size_t)nSize * sizeof(char *);
	pp = (char **)malloc(cb + (size_t)nSize * cbPer);
	if (pp == NULL) {
		return NULL;
	}
	{
		char *pStr = (char *)(pp + nSize);

		for (i = 0; i < nSize; i++) {
			char aTmp[48];
			uintptr_t u = (uintptr_t)ppBuffer[i];
			size_t nHex;
			size_t n = 0;
			int nIdx = i;
			char aIdx[8];
			int d = 0;

			/* "[N] 0x..." soft symbol string */
			aTmp[n++] = '[';
			if (nIdx == 0) {
				aIdx[d++] = '0';
			} else {
				while (nIdx > 0 && d < 7) {
					aIdx[d++] = (char)('0' + (nIdx % 10));
					nIdx /= 10;
				}
			}
			while (d > 0 && n + 1 < sizeof(aTmp)) {
				aTmp[n++] = aIdx[--d];
			}
			if (n + 2 < sizeof(aTmp)) {
				aTmp[n++] = ']';
				aTmp[n++] = ' ';
			}
			nHex = fmt_hex_ptr(aTmp + n, sizeof(aTmp) - n, u);
			n += nHex - 1; /* exclude NUL counted by fmt */
			aTmp[n] = '\0';
			pp[i] = pStr;
			memcpy(pStr, aTmp, n + 1);
			pStr += cbPer;
		}
	}
	return pp;
}

void
backtrace_symbols_fd(void *const *ppBuffer, int nSize, int nFd)
{
	int i;

	if (ppBuffer == NULL || nSize <= 0 || nFd < 0) {
		return;
	}
	if (nSize > BT_FRAME_CAP) {
		nSize = BT_FRAME_CAP;
	}
	for (i = 0; i < nSize; i++) {
		char aLine[56];
		uintptr_t u = (uintptr_t)ppBuffer[i];
		size_t n = 0;
		size_t nHex;
		int nIdx = i;
		char aIdx[8];
		int d = 0;

		aLine[n++] = '[';
		if (nIdx == 0) {
			aIdx[d++] = '0';
		} else {
			while (nIdx > 0 && d < 7) {
				aIdx[d++] = (char)('0' + (nIdx % 10));
				nIdx /= 10;
			}
		}
		while (d > 0 && n + 1 < sizeof(aLine)) {
			aLine[n++] = aIdx[--d];
		}
		if (n + 2 < sizeof(aLine)) {
			aLine[n++] = ']';
			aLine[n++] = ' ';
		}
		nHex = fmt_hex_ptr(aLine + n, sizeof(aLine) - n - 1, u);
		n += nHex - 1;
		aLine[n++] = '\n';
		(void)write(nFd, aLine, n);
	}
}
