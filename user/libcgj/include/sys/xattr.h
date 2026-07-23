/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/xattr.h (subset). Not GNU glibc.
 */
#pragma once

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XATTR_CREATE  1
#define XATTR_REPLACE 2

int     setxattr(const char *szPath, const char *szName, const void *pValue,
                 size_t cb, int nFlags);
int     lsetxattr(const char *szPath, const char *szName, const void *pValue,
                  size_t cb, int nFlags);
int     fsetxattr(int nFd, const char *szName, const void *pValue, size_t cb,
                  int nFlags);
ssize_t getxattr(const char *szPath, const char *szName, void *pValue,
                 size_t cb);
ssize_t lgetxattr(const char *szPath, const char *szName, void *pValue,
                  size_t cb);
ssize_t fgetxattr(int nFd, const char *szName, void *pValue, size_t cb);
ssize_t listxattr(const char *szPath, char *szList, size_t cb);
ssize_t llistxattr(const char *szPath, char *szList, size_t cb);
ssize_t flistxattr(int nFd, char *szList, size_t cb);
int     removexattr(const char *szPath, const char *szName);
int     lremovexattr(const char *szPath, const char *szName);
int     fremovexattr(int nFd, const char *szName);

#ifdef __cplusplus
}
#endif
