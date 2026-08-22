/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <libgen.h> for libcgj (HAVE_LIBGEN_H).
 * basename/dirname implementations live in string.c. Dual DoD B OPEN.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

char *basename(char *szPath);
char *dirname(char *szPath);

#ifdef __cplusplus
}
#endif
