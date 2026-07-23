/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room C11 uchar.h (subset). Not GNU glibc.
 */
#pragma once

#include <stddef.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
typedef unsigned short char16_t;
typedef unsigned int   char32_t;
#endif

size_t c16rtomb(char *pS, char16_t c16, mbstate_t *pSt);
size_t c32rtomb(char *pS, char32_t c32, mbstate_t *pSt);
size_t mbrtoc16(char16_t *pC16, const char *pS, size_t n, mbstate_t *pSt);
size_t mbrtoc32(char32_t *pC32, const char *pS, size_t n, mbstate_t *pSt);

#ifdef __cplusplus
}
#endif
