/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped monetary.h. Not GNU glibc.
 * strfmon bring-up is integer/literal only (no SSE doubles).
 */
#pragma once

#include <locale.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

ssize_t strfmon(char *sz, size_t cbMax, const char *szFmt, ...);
ssize_t strfmon_l(char *sz, size_t cbMax, locale_t loc, const char *szFmt, ...);

#ifdef __cplusplus
}
#endif
