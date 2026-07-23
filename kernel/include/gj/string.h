/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 */
#pragma once

#include <gj/types.h>

void *memset(void *pDst, int nFill, size_t cbCount);
void *memcpy(void *pDst, const void *pSrc, size_t cbCount);
int   memcmp(const void *pA, const void *pB, size_t cbCount);
size_t strlen(const char *sz);
