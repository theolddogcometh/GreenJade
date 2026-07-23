/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * mntent bring-up: parse simple fstab/mtab lines (no escape handling).
 */
#include <errno.h>
#include <mntent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *
setmntent(const char *szFile, const char *szType)
{
    if (szFile == NULL || szType == NULL) {
        errno = EINVAL;
        return NULL;
    }
    return fopen(szFile, szType);
}

int
endmntent(FILE *pF)
{
    if (pF == NULL) {
        return 1;
    }
    return (fclose(pF) == 0) ? 1 : 0;
}

static int
parse_mnt_line(char *szLine, struct mntent *pMnt)
{
    char *p;
    char *aTok[6];
    int n = 0;

    if (szLine == NULL || pMnt == NULL) {
        return -1;
    }
    /* strip comment */
    p = strchr(szLine, '#');
    if (p != NULL) {
        *p = '\0';
    }
    p = szLine;
    while (n < 6) {
        while (*p == ' ' || *p == '\t' || *p == '\n') {
            p++;
        }
        if (*p == '\0') {
            break;
        }
        aTok[n++] = p;
        while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n') {
            p++;
        }
        if (*p != '\0') {
            *p++ = '\0';
        }
    }
    if (n < 4) {
        return -1;
    }
    pMnt->mnt_fsname = aTok[0];
    pMnt->mnt_dir = aTok[1];
    pMnt->mnt_type = aTok[2];
    pMnt->mnt_opts = aTok[3];
    pMnt->mnt_freq = (n > 4) ? (int)strtol(aTok[4], NULL, 10) : 0;
    pMnt->mnt_passno = (n > 5) ? (int)strtol(aTok[5], NULL, 10) : 0;
    return 0;
}

struct mntent *
getmntent_r(FILE *pF, struct mntent *pMnt, char *szBuf, int nBufSize)
{
    if (pF == NULL || pMnt == NULL || szBuf == NULL || nBufSize <= 0) {
        errno = EINVAL;
        return NULL;
    }
    for (;;) {
        if (fgets(szBuf, nBufSize, pF) == NULL) {
            return NULL;
        }
        if (parse_mnt_line(szBuf, pMnt) == 0) {
            return pMnt;
        }
    }
}

struct mntent *
getmntent(FILE *pF)
{
    static char aBuf[512];
    static struct mntent sMnt;

    return getmntent_r(pF, &sMnt, aBuf, (int)sizeof(aBuf));
}

int
addmntent(FILE *pF, const struct mntent *pMnt)
{
    if (pF == NULL || pMnt == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (fprintf(pF, "%s %s %s %s %d %d\n",
                pMnt->mnt_fsname != NULL ? pMnt->mnt_fsname : "none",
                pMnt->mnt_dir != NULL ? pMnt->mnt_dir : "/",
                pMnt->mnt_type != NULL ? pMnt->mnt_type : "none",
                pMnt->mnt_opts != NULL ? pMnt->mnt_opts : "defaults",
                pMnt->mnt_freq, pMnt->mnt_passno) < 0) {
        return 1;
    }
    return 0;
}

char *
hasmntopt(const struct mntent *pMnt, const char *szOpt)
{
    char *p;
    size_t nOpt;

    if (pMnt == NULL || pMnt->mnt_opts == NULL || szOpt == NULL) {
        return NULL;
    }
    nOpt = strlen(szOpt);
    p = pMnt->mnt_opts;
    while (*p != '\0') {
        if (strncmp(p, szOpt, nOpt) == 0 &&
            (p[nOpt] == '\0' || p[nOpt] == ',' || p[nOpt] == '=')) {
            return p;
        }
        while (*p != '\0' && *p != ',') {
            p++;
        }
        if (*p == ',') {
            p++;
        }
    }
    return NULL;
}
