/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * struct nlist for OpenSSH misc.c (HAVE_NLIST_H). HAVE_NLIST is unset;
 * this header only has to parse. Dual DoD B OPEN.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct nlist {
    char          *n_name;
    unsigned char  n_type;
    char           n_other;
    short          n_desc;
    unsigned long  n_value;
};

int nlist(const char *szFilename, struct nlist *pNl);

#ifdef __cplusplus
}
#endif
