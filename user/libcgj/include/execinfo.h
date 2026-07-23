/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room backtrace stubs (bring-up). Not GNU glibc.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int   backtrace(void **ppBuffer, int nSize);
char **backtrace_symbols(void *const *ppBuffer, int nSize);
void  backtrace_symbols_fd(void *const *ppBuffer, int nSize, int nFd);
int   __backtrace(void **ppBuffer, int nSize);
char **__backtrace_symbols(void *const *ppBuffer, int nSize);
void  __backtrace_symbols_fd(void *const *ppBuffer, int nSize, int nFd);

#ifdef __cplusplus
}
#endif
