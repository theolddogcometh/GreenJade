/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * mntent soft fill: fstab/mtab parse with octal-escape decode (\\040 etc).
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

/*
 * Decode util-linux style escapes in place: \0NN (octal), \\, \t, \n, \r, \f, \a, \v, \b.
 * Returns pointer past last written NUL (field end).
 */
static char *
decode_field(char *sz)
{
    char *pIn = sz;
    char *pOut = sz;

    if (sz == NULL) {
        return NULL;
    }
    while (*pIn != '\0') {
        if (*pIn == '\\' && pIn[1] != '\0') {
            pIn++;
            if (pIn[0] >= '0' && pIn[0] <= '7') {
                int n = 0;
                int c = 0;

                while (n < 3 && pIn[0] >= '0' && pIn[0] <= '7') {
                    c = c * 8 + (pIn[0] - '0');
                    pIn++;
                    n++;
                }
                *pOut++ = (char)c;
                continue;
            }
            switch (*pIn) {
            case '\\':
                *pOut++ = '\\';
                break;
            case 't':
                *pOut++ = '\t';
                break;
            case 'n':
                *pOut++ = '\n';
                break;
            case 'r':
                *pOut++ = '\r';
                break;
            case 'f':
                *pOut++ = '\f';
                break;
            case 'a':
                *pOut++ = '\a';
                break;
            case 'v':
                *pOut++ = '\v';
                break;
            case 'b':
                *pOut++ = '\b';
                break;
            case 's':
                *pOut++ = ' ';
                break;
            default:
                *pOut++ = *pIn;
                break;
            }
            pIn++;
            continue;
        }
        *pOut++ = *pIn++;
    }
    *pOut = '\0';
    return pOut;
}

static int
parse_mnt_line(char *szLine, struct mntent *pMnt)
{
    char *p;
    char *aTok[6];
    int n = 0;
    int i;

    if (szLine == NULL || pMnt == NULL) {
        return -1;
    }
    /* strip comment (not escaped #) */
    p = szLine;
    while (*p != '\0') {
        if (*p == '#') {
            *p = '\0';
            break;
        }
        if (*p == '\\' && p[1] != '\0') {
            p += 2;
            continue;
        }
        p++;
    }
    p = szLine;
    while (n < 6) {
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
            p++;
        }
        if (*p == '\0') {
            break;
        }
        aTok[n++] = p;
        while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n' &&
               *p != '\r') {
            if (*p == '\\' && p[1] != '\0') {
                p += 2;
                continue;
            }
            p++;
        }
        if (*p != '\0') {
            *p++ = '\0';
        }
    }
    if (n < 4) {
        return -1;
    }
    for (i = 0; i < n && i < 4; i++) {
        (void)decode_field(aTok[i]);
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
        size_t nLen;

        if (fgets(szBuf, nBufSize, pF) == NULL) {
            return NULL;
        }
        nLen = strlen(szBuf);
        /* Soft: if line was truncated (no NL and buffer full), skip rest. */
        if (nLen + 1 >= (size_t)nBufSize &&
            (nLen == 0 || szBuf[nLen - 1] != '\n')) {
            int ch;

            while ((ch = fgetc(pF)) != EOF && ch != '\n') {
                /* discard */
            }
            continue;
        }
        if (parse_mnt_line(szBuf, pMnt) == 0) {
            return pMnt;
        }
    }
}

struct mntent *
getmntent(FILE *pF)
{
    static char aBuf[1024];
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
    /* Soft write without re-escaping (callers usually pass plain text). */
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

    if (pMnt == NULL || pMnt->mnt_opts == NULL || szOpt == NULL ||
        szOpt[0] == '\0') {
        return NULL;
    }
    nOpt = strlen(szOpt);
    p = pMnt->mnt_opts;
    while (*p != '\0') {
        /* Skip leading commas / spaces. */
        while (*p == ',' || *p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            break;
        }
        if (strncmp(p, szOpt, nOpt) == 0 &&
            (p[nOpt] == '\0' || p[nOpt] == ',' || p[nOpt] == '=' ||
             p[nOpt] == ' ' || p[nOpt] == '\t')) {
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
