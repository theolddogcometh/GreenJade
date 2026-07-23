/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room GNU error.h (subset). Not GNU glibc.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned int error_message_count;
extern int error_one_per_line;
extern void (*error_print_progname)(void);

void error(int nStatus, int nErrnum, const char *szFmt, ...)
    __attribute__((format(printf, 3, 4)));
void error_at_line(int nStatus, int nErrnum, const char *szFile,
                   unsigned nLine, const char *szFmt, ...)
    __attribute__((format(printf, 5, 6)));

#ifdef __cplusplus
}
#endif
