/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped fnmatch (subset). Not GNU glibc.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define FNM_NOMATCH     1
#define FNM_PATHNAME    (1 << 0)
#define FNM_NOESCAPE    (1 << 1)
#define FNM_PERIOD      (1 << 2)
#define FNM_CASEFOLD    (1 << 4)

int fnmatch(const char *szPattern, const char *szString, int nFlags);

#ifdef __cplusplus
}
#endif
