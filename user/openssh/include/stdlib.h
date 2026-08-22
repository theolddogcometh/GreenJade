/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <stdlib.h> for OpenSSH-portable. Host POSIX: glibc.
 * DUT: libcgj stdlib.h does not declare strtod (float conversion residual).
 * Declaration only — not a libcgj implementation claim.
 */
#pragma once
#include_next <stdlib.h>
#if !__STDC_HOSTED__
#ifndef MB_CUR_MAX
# define MB_CUR_MAX 4
#endif
double strtod(const char *sz, char **ppEnd);
float strtof(const char *sz, char **ppEnd);
long double strtold(const char *sz, char **ppEnd);
#endif
