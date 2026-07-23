/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped fts.h (BSD FTS subset). Not GNU glibc.
 */
#pragma once

#include <sys/stat.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* fts_open options */
#define FTS_COMFOLLOW  0x0001
#define FTS_LOGICAL    0x0002
#define FTS_NOCHDIR    0x0004
#define FTS_NOSTAT     0x0008
#define FTS_PHYSICAL   0x0010
#define FTS_SEEDOT     0x0020
#define FTS_XDEV       0x0040
#define FTS_WHITEOUT   0x0080

/* fts_info values */
#define FTS_D       1  /* preorder directory */
#define FTS_DC      2  /* directory that causes cycles */
#define FTS_DEFAULT 3
#define FTS_DNR     4  /* unreadable directory */
#define FTS_DOT     5
#define FTS_DP      6  /* postorder directory */
#define FTS_ERR     7
#define FTS_F       8  /* regular file */
#define FTS_INIT    9
#define FTS_NS      10 /* stat failed */
#define FTS_NSOK    11
#define FTS_SL      12 /* symbolic link */
#define FTS_SLNONE  13
#define FTS_W       14

/* fts_set instructions */
#define FTS_AGAIN   1
#define FTS_FOLLOW  2
#define FTS_NOINSTR 3
#define FTS_SKIP    4

typedef struct {
    struct _ftsent *fts_cur;
    struct _ftsent *fts_child;
    struct _ftsent **fts_array;
    dev_t           fts_dev;
    char           *fts_path;
    int             fts_rfd;
    size_t          fts_pathlen;
    size_t          fts_nitems;
    int (*fts_compar)(const struct _ftsent **, const struct _ftsent **);
    int             fts_options;
    void           *fts_priv; /* libcgj private walk state */
} FTS;

typedef struct _ftsent {
    struct _ftsent *fts_cycle;
    struct _ftsent *fts_parent;
    struct _ftsent *fts_link;
    long            fts_number;
    void           *fts_pointer;
    char           *fts_accpath;
    char           *fts_path;
    int             fts_errno;
    size_t          fts_pathlen;
    size_t          fts_namelen;
    ino_t           fts_ino;
    dev_t           fts_dev;
    nlink_t         fts_nlink;
    short           fts_level;
    unsigned short  fts_info;
    unsigned short  fts_flags;
    unsigned short  fts_instr;
    struct stat    *fts_statp;
    char            fts_name[1];
} FTSENT;

FTS    *fts_open(char *const *ppPathv, int nOptions,
                 int (*pfnCompar)(const FTSENT **, const FTSENT **));
FTSENT *fts_read(FTS *pFts);
int     fts_close(FTS *pFts);
int     fts_set(FTS *pFts, FTSENT *pEnt, int nInstr);
FTSENT *fts_children(FTS *pFts, int nOptions);

#ifdef __cplusplus
}
#endif
