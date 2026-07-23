/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped dirent (subset). Not GNU glibc.
 */
#pragma once

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DT_UNKNOWN 0
#define DT_FIFO    1
#define DT_CHR     2
#define DT_DIR     4
#define DT_BLK     6
#define DT_REG     8
#define DT_LNK     10
#define DT_SOCK    12

struct dirent {
    ino_t          d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    unsigned char  d_type;
    char           d_name[256];
};

typedef struct {
    int           nFd;
    int           nPos;
    int           nEnd;
    unsigned char aBuf[2048];
    struct dirent ent;
} DIR;

DIR           *opendir(const char *szPath);
DIR           *fdopendir(int nFd);
struct dirent *readdir(DIR *pDir);
int            closedir(DIR *pDir);
void           rewinddir(DIR *pDir);
int            scandir(const char *szPath, struct dirent ***ppList,
                       int (*pfnFilter)(const struct dirent *),
                       int (*pfnCmp)(const struct dirent **,
                                     const struct dirent **));
int            scandirat(int nDfd, const char *szPath, struct dirent ***ppList,
                         int (*pfnFilter)(const struct dirent *),
                         int (*pfnCmp)(const struct dirent **,
                                       const struct dirent **));
int            alphasort(const struct dirent **ppA, const struct dirent **ppB);
int            alphasort64(const struct dirent **ppA, const struct dirent **ppB);
int            versionsort(const struct dirent **ppA, const struct dirent **ppB);
int            versionsort64(const struct dirent **ppA, const struct dirent **ppB);
int            dirfd(DIR *pDir);
int            readdir_r(DIR *pDir, struct dirent *pEntry,
                         struct dirent **ppResult);
int            readdir64_r(DIR *pDir, struct dirent *pEntry,
                           struct dirent **ppResult);
void           seekdir(DIR *pDir, long nLoc);
long           telldir(DIR *pDir);

#ifdef __cplusplus
}
#endif
