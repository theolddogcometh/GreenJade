/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped envz.h. Not GNU glibc.
 */
#pragma once

#include <argz.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

char   *envz_entry(const char *pEnvz, size_t cbEnvz, const char *szName);
char   *envz_get(const char *pEnvz, size_t cbEnvz, const char *szName);
error_t envz_add(char **ppEnvz, size_t *pcbEnvz, const char *szName,
                 const char *szValue);
error_t envz_merge(char **ppEnvz, size_t *pcbEnvz, const char *pEnvz2,
                   size_t cbEnvz2, int nOverride);
void    envz_remove(char **ppEnvz, size_t *pcbEnvz, const char *szName);
void    envz_strip(char **ppEnvz, size_t *pcbEnvz);

#ifdef __cplusplus
}
#endif
