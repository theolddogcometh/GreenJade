/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped nl_types.h (message catalogs). Not GNU glibc.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void *nl_catd;
typedef int   nl_item;

#define NL_SETD       1
#define NL_CAT_LOCALE 1

nl_catd catopen(const char *szName, int nFlag);
char   *catgets(nl_catd cat, int nSet, int nMsg, const char *szDefault);
int     catclose(nl_catd cat);

#ifdef __cplusplus
}
#endif
