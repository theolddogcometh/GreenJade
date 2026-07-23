/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * gmtime/localtime/asctime/strftime — C locale, UTC-as-local bring-up.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static const int s_aDaysInMonth[] = { 31, 28, 31, 30, 31, 30,
                                      31, 31, 30, 31, 30, 31 };
static const char *const s_aWday[] = { "Sun", "Mon", "Tue", "Wed",
                                       "Thu", "Fri", "Sat" };
static const char *const s_aMon[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

static int
is_leap(int nY)
{
    return ((nY % 4 == 0) && (nY % 100 != 0)) || (nY % 400 == 0);
}

struct tm *
gmtime_r(const time_t *pT, struct tm *pTm)
{
    int64_t sec;
    int64_t days;
    int64_t rem;
    int nY;
    int nYday;
    int nMon;

    if (pT == NULL || pTm == NULL) {
        return NULL;
    }
    sec = (int64_t)(*pT);
    if (sec < 0) {
        sec = 0; /* bring-up: clamp pre-epoch */
    }
    days = sec / 86400;
    rem = sec % 86400;
    pTm->tm_hour = (int)(rem / 3600);
    rem %= 3600;
    pTm->tm_min = (int)(rem / 60);
    pTm->tm_sec = (int)(rem % 60);
    /* 1970-01-01 was Thursday → wday = (days + 4) % 7 */
    pTm->tm_wday = (int)((days + 4) % 7);
    if (pTm->tm_wday < 0) {
        pTm->tm_wday += 7;
    }
    nY = 1970;
    for (;;) {
        int nDiy = is_leap(nY) ? 366 : 365;

        if (days < nDiy) {
            break;
        }
        days -= nDiy;
        nY++;
    }
    pTm->tm_year = nY - 1900;
    pTm->tm_yday = (int)days;
    nYday = (int)days;
    for (nMon = 0; nMon < 12; nMon++) {
        int nDim = s_aDaysInMonth[nMon];

        if (nMon == 1 && is_leap(nY)) {
            nDim = 29;
        }
        if (nYday < nDim) {
            break;
        }
        nYday -= nDim;
    }
    pTm->tm_mon = nMon;
    pTm->tm_mday = nYday + 1;
    pTm->tm_isdst = 0;
    pTm->tm_gmtoff = 0;
    pTm->tm_zone = "UTC";
    return pTm;
}

struct tm *
localtime_r(const time_t *pT, struct tm *pTm)
{
    /* Bring-up: local == UTC */
    return gmtime_r(pT, pTm);
}

struct tm *
gmtime(const time_t *pT)
{
    static struct tm s_tm;

    return gmtime_r(pT, &s_tm);
}

struct tm *
localtime(const time_t *pT)
{
    static struct tm s_tm;

    return localtime_r(pT, &s_tm);
}

char *
asctime_r(const struct tm *pTm, char *szBuf)
{
    /* "Www Mmm dd hh:mm:ss yyyy\n" */
    if (pTm == NULL || szBuf == NULL) {
        return NULL;
    }
    (void)snprintf(szBuf, 26, "%s %s %2d %02d:%02d:%02d %04d\n",
                   s_aWday[pTm->tm_wday < 0 || pTm->tm_wday > 6 ? 0
                                                                : pTm->tm_wday],
                   s_aMon[pTm->tm_mon < 0 || pTm->tm_mon > 11 ? 0 : pTm->tm_mon],
                   pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec,
                   pTm->tm_year + 1900);
    return szBuf;
}

char *
asctime(const struct tm *pTm)
{
    static char aBuf[32];

    return asctime_r(pTm, aBuf);
}

char *
ctime_r(const time_t *pT, char *szBuf)
{
    struct tm tm;

    if (localtime_r(pT, &tm) == NULL) {
        return NULL;
    }
    return asctime_r(&tm, szBuf);
}

char *
ctime(const time_t *pT)
{
    static char aBuf[32];

    return ctime_r(pT, aBuf);
}

static size_t
put_num(char *p, size_t cbLeft, int nVal, int nWidth, int fZero)
{
    char aTmp[16];
    int n = 0;
    int v = nVal;
    int i;
    size_t k;

    if (v < 0) {
        v = -v;
    }
    if (v == 0) {
        aTmp[n++] = '0';
    } else {
        while (v > 0 && n < 12) {
            aTmp[n++] = (char)('0' + (v % 10));
            v /= 10;
        }
    }
    while (n < nWidth) {
        aTmp[n++] = fZero ? '0' : ' ';
    }
    if ((size_t)n > cbLeft) {
        return 0;
    }
    for (i = n - 1, k = 0; i >= 0; i--, k++) {
        p[k] = aTmp[i];
    }
    return (size_t)n;
}

size_t
strftime(char *szBuf, size_t cb, const char *szFmt, const struct tm *pTm)
{
    size_t nOut = 0;
    const char *p;

    if (szBuf == NULL || cb == 0 || szFmt == NULL || pTm == NULL) {
        return 0;
    }
    for (p = szFmt; *p != '\0'; p++) {
        if (*p != '%' || p[1] == '\0') {
            if (nOut + 1 >= cb) {
                szBuf[0] = '\0';
                return 0;
            }
            szBuf[nOut++] = *p;
            continue;
        }
        p++;
        {
            const char *sz = NULL;
            char aNum[16];
            size_t nAdd = 0;

            switch (*p) {
            case '%':
                aNum[0] = '%';
                aNum[1] = '\0';
                sz = aNum;
                break;
            case 'Y':
                nAdd = put_num(aNum, sizeof(aNum), pTm->tm_year + 1900, 4, 1);
                aNum[nAdd] = '\0';
                sz = aNum;
                break;
            case 'y':
                nAdd = put_num(aNum, sizeof(aNum), (pTm->tm_year + 1900) % 100,
                               2, 1);
                aNum[nAdd] = '\0';
                sz = aNum;
                break;
            case 'm':
                nAdd = put_num(aNum, sizeof(aNum), pTm->tm_mon + 1, 2, 1);
                aNum[nAdd] = '\0';
                sz = aNum;
                break;
            case 'd':
                nAdd = put_num(aNum, sizeof(aNum), pTm->tm_mday, 2, 1);
                aNum[nAdd] = '\0';
                sz = aNum;
                break;
            case 'H':
                nAdd = put_num(aNum, sizeof(aNum), pTm->tm_hour, 2, 1);
                aNum[nAdd] = '\0';
                sz = aNum;
                break;
            case 'M':
                nAdd = put_num(aNum, sizeof(aNum), pTm->tm_min, 2, 1);
                aNum[nAdd] = '\0';
                sz = aNum;
                break;
            case 'S':
                nAdd = put_num(aNum, sizeof(aNum), pTm->tm_sec, 2, 1);
                aNum[nAdd] = '\0';
                sz = aNum;
                break;
            case 'a':
                sz = s_aWday[pTm->tm_wday < 0 || pTm->tm_wday > 6 ? 0
                                                                  : pTm->tm_wday];
                break;
            case 'b':
            case 'h':
                sz = s_aMon[pTm->tm_mon < 0 || pTm->tm_mon > 11 ? 0
                                                                : pTm->tm_mon];
                break;
            case 'F': /* %Y-%m-%d */
            {
                size_t n = strftime(szBuf + nOut, cb - nOut, "%Y-%m-%d", pTm);

                if (n == 0 && cb - nOut > 0) {
                    szBuf[0] = '\0';
                    return 0;
                }
                nOut += n;
                continue;
            }
            case 'T': /* %H:%M:%S */
            {
                size_t n = strftime(szBuf + nOut, cb - nOut, "%H:%M:%S", pTm);

                if (n == 0 && cb - nOut > 0) {
                    szBuf[0] = '\0';
                    return 0;
                }
                nOut += n;
                continue;
            }
            default:
                aNum[0] = '%';
                aNum[1] = *p;
                aNum[2] = '\0';
                sz = aNum;
                break;
            }
            if (sz != NULL) {
                size_t n = strlen(sz);

                if (nOut + n >= cb) {
                    szBuf[0] = '\0';
                    return 0;
                }
                memcpy(szBuf + nOut, sz, n);
                nOut += n;
            }
        }
    }
    if (nOut >= cb) {
        szBuf[0] = '\0';
        return 0;
    }
    szBuf[nOut] = '\0';
    return nOut;
}

/* ---- tzset / timegm / mktime / strptime ---- */

char *tzname[2] = { (char *)"UTC", (char *)"UTC" };
long  timezone;
int   daylight;

void
tzset(void)
{
    tzname[0] = (char *)"UTC";
    tzname[1] = (char *)"UTC";
    timezone = 0;
    daylight = 0;
}

static int
parse_digits(const char **pp, int nWidth, int *pVal)
{
    const char *p = *pp;
    int v = 0;
    int n = 0;

    while (n < nWidth && *p >= '0' && *p <= '9') {
        v = v * 10 + (*p - '0');
        p++;
        n++;
    }
    if (n == 0) {
        return -1;
    }
    *pVal = v;
    *pp = p;
    return 0;
}

static int
match_name(const char **pp, const char *const *aNames, int nCount, int *pIdx)
{
    const char *p = *pp;
    int i;

    for (i = 0; i < nCount; i++) {
        size_t n = strlen(aNames[i]);
        size_t j;

        for (j = 0; j < n; j++) {
            char c = p[j];
            char d = aNames[i][j];

            if (c >= 'A' && c <= 'Z') {
                c = (char)(c - 'A' + 'a');
            }
            if (d >= 'A' && d <= 'Z') {
                d = (char)(d - 'A' + 'a');
            }
            if (c != d) {
                break;
            }
        }
        if (j == n) {
            *pIdx = i;
            *pp = p + n;
            return 0;
        }
    }
    return -1;
}

char *
strptime(const char *szBuf, const char *szFmt, struct tm *pTm)
{
    const char *s;
    const char *f;

    if (szBuf == NULL || szFmt == NULL || pTm == NULL) {
        return NULL;
    }
    s = szBuf;
    f = szFmt;
    while (*f != '\0') {
        if (*f != '%') {
            if (*f == ' ') {
                while (*s == ' ' || *s == '\t') {
                    s++;
                }
                f++;
                continue;
            }
            if (*s != *f) {
                return NULL;
            }
            s++;
            f++;
            continue;
        }
        f++;
        if (*f == '\0') {
            return NULL;
        }
        switch (*f) {
        case '%':
            if (*s != '%') {
                return NULL;
            }
            s++;
            break;
        case 'Y': {
            int y;

            if (parse_digits(&s, 4, &y) != 0) {
                return NULL;
            }
            pTm->tm_year = y - 1900;
            break;
        }
        case 'y': {
            int y;

            if (parse_digits(&s, 2, &y) != 0) {
                return NULL;
            }
            pTm->tm_year = (y >= 69) ? y : (y + 100);
            break;
        }
        case 'm': {
            int m;

            if (parse_digits(&s, 2, &m) != 0 || m < 1 || m > 12) {
                return NULL;
            }
            pTm->tm_mon = m - 1;
            break;
        }
        case 'd': {
            int d;

            if (parse_digits(&s, 2, &d) != 0 || d < 1 || d > 31) {
                return NULL;
            }
            pTm->tm_mday = d;
            break;
        }
        case 'H': {
            int h;

            if (parse_digits(&s, 2, &h) != 0 || h > 23) {
                return NULL;
            }
            pTm->tm_hour = h;
            break;
        }
        case 'M': {
            int m;

            if (parse_digits(&s, 2, &m) != 0 || m > 59) {
                return NULL;
            }
            pTm->tm_min = m;
            break;
        }
        case 'S': {
            int sec;

            if (parse_digits(&s, 2, &sec) != 0 || sec > 60) {
                return NULL;
            }
            pTm->tm_sec = sec;
            break;
        }
        case 'a': {
            int idx;

            if (match_name(&s, s_aWday, 7, &idx) != 0) {
                return NULL;
            }
            pTm->tm_wday = idx;
            break;
        }
        case 'b':
        case 'h': {
            int idx;

            if (match_name(&s, s_aMon, 12, &idx) != 0) {
                return NULL;
            }
            pTm->tm_mon = idx;
            break;
        }
        case 'F': {
            char *p2 = strptime(s, "%Y-%m-%d", pTm);

            if (p2 == NULL) {
                return NULL;
            }
            s = p2;
            break;
        }
        case 'T': {
            char *p2 = strptime(s, "%H:%M:%S", pTm);

            if (p2 == NULL) {
                return NULL;
            }
            s = p2;
            break;
        }
        default:
            return NULL;
        }
        f++;
    }
    return (char *)(uintptr_t)s;
}

time_t
timegm(struct tm *pTm)
{
    int64_t days = 0;
    int nY;
    int nMon;
    int i;

    if (pTm == NULL) {
        return (time_t)-1;
    }
    nY = pTm->tm_year + 1900;
    nMon = pTm->tm_mon;
    if (nMon < 0 || nMon > 11 || pTm->tm_mday < 1 || pTm->tm_mday > 31) {
        return (time_t)-1;
    }
    for (i = 1970; i < nY; i++) {
        days += is_leap(i) ? 366 : 365;
    }
    for (i = 0; i < nMon; i++) {
        int nDim = s_aDaysInMonth[i];

        if (i == 1 && is_leap(nY)) {
            nDim = 29;
        }
        days += nDim;
    }
    days += pTm->tm_mday - 1;
    {
        int64_t sec = days * 86400LL + (int64_t)pTm->tm_hour * 3600LL +
                      (int64_t)pTm->tm_min * 60LL + (int64_t)pTm->tm_sec;

        /* Fill derived fields */
        {
            time_t t = (time_t)sec;
            struct tm tmp;

            if (gmtime_r(&t, &tmp) != NULL) {
                pTm->tm_wday = tmp.tm_wday;
                pTm->tm_yday = tmp.tm_yday;
            }
        }
        pTm->tm_isdst = 0;
        pTm->tm_gmtoff = 0;
        pTm->tm_zone = "UTC";
        return (time_t)sec;
    }
}

time_t
mktime(struct tm *pTm)
{
    /* Bring-up: local == UTC */
    tzset();
    return timegm(pTm);
}
