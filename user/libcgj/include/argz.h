/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped argz.h. Not GNU glibc.
 */
#pragma once

#include <errno.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

error_t argz_create(char *const argv[], char **ppArgz, size_t *pcbArgz);
error_t argz_create_sep(const char *sz, int nSep, char **ppArgz,
                        size_t *pcbArgz);
size_t  argz_count(const char *pArgz, size_t cbArgz);
void    argz_extract(const char *pArgz, size_t cbArgz, char **ppArgv);
error_t argz_append(char **ppArgz, size_t *pcbArgz, const char *pBuf,
                    size_t cbBuf);
error_t argz_add(char **ppArgz, size_t *pcbArgz, const char *sz);
error_t argz_add_sep(char **ppArgz, size_t *pcbArgz, const char *sz, int nSep);
void    argz_delete(char **ppArgz, size_t *pcbArgz, char *pEntry);
error_t argz_insert(char **ppArgz, size_t *pcbArgz, char *pBefore,
                    const char *sz);
char   *argz_next(const char *pArgz, size_t cbArgz, const char *pEntry);
error_t argz_replace(char **ppArgz, size_t *pcbArgz, const char *szStr,
                     const char *szWith, unsigned *pnReplace);
void    argz_stringify(char *pArgz, size_t cbArgz, int nSep);

/* GNU underscored aliases */
size_t  __argz_count(const char *pArgz, size_t cbArgz);
char   *__argz_next(const char *pArgz, size_t cbArgz, const char *pEntry);
void    __argz_stringify(char *pArgz, size_t cbArgz, int nSep);

#ifdef __cplusplus
}
#endif
