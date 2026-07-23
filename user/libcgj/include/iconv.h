/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room iconv (identity / Latin-1 / UTF-8). Not GNU libiconv/glibc.
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *iconv_t;

iconv_t iconv_open(const char *szTo, const char *szFrom);
int     iconv_close(iconv_t cd);
size_t  iconv(iconv_t cd, char **ppIn, size_t *pInLeft, char **ppOut,
              size_t *pOutLeft);

#ifdef __cplusplus
}
#endif
