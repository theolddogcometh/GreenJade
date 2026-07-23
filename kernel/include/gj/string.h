/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding kernel string/memory helpers (soft mem- and str- set).
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 */
#pragma once

#include <gj/types.h>

/* ---- memory ---- */
void  *memset(void *pDst, int nFill, size_t cbCount);
void  *memcpy(void *pDst, const void *pSrc, size_t cbCount);
void  *memmove(void *pDst, const void *pSrc, size_t cbCount);
int    memcmp(const void *pA, const void *pB, size_t cbCount);
void  *memchr(const void *pHay, int nNeedle, size_t cbCount);

/* ---- length / compare ---- */
size_t strlen(const char *sz);
size_t strnlen(const char *sz, size_t cbMax);
int    strcmp(const char *szA, const char *szB);
int    strncmp(const char *szA, const char *szB, size_t cbMax);

/* ---- bounded copy / append (BSD-style; always NUL when cbDst > 0) ---- */
size_t strlcpy(char *szDst, const char *szSrc, size_t cbDst);
size_t strlcat(char *szDst, const char *szSrc, size_t cbDst);

/* ---- search ---- */
char  *strchr(const char *sz, int nCh);
char  *strrchr(const char *sz, int nCh);
char  *strstr(const char *szHay, const char *szNeedle);
