/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * gmtime/localtime/asctime/strftime/strptime/mktime/timegm —
 * C locale, UTC-as-local soft fill. (timelocal/getdate live in graph batches.)
 */
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static const int s_aDaysInMonth[] = { 31, 28, 31, 30, 31, 30,
                                      31, 31, 30, 31, 30, 31 };
static const char *const s_aWday[] = { "Sun", "Mon", "Tue", "Wed",
                                       "Thu", "Fri", "Sat" };
static const char *const s_aWdayFull[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
static const char *const s_aMon[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static const char *const s_aMonFull[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

static int
is_leap(int nY)
{
    return ((nY % 4 == 0) && (nY % 100 != 0)) || (nY % 400 == 0);
}

static int
days_in_month(int nY, int nMon)
{
    if (nMon < 0 || nMon > 11) {
        return 0;
    }
    if (nMon == 1 && is_leap(nY)) {
        return 29;
    }
    return s_aDaysInMonth[nMon];
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
    int fNeg = 0;

    if (pT == NULL || pTm == NULL) {
        return NULL;
    }
    sec = (int64_t)(*pT);
    if (sec < 0) {
        /* Soft: support modest pre-epoch (back to ~1902 for 32-bit paths). */
        fNeg = 1;
        sec = -sec;
    }
    days = sec / 86400;
    rem = sec % 86400;
    if (fNeg) {
        if (rem != 0) {
            days++;
            rem = 86400 - rem;
        }
        days = -days;
    }
    pTm->tm_hour = (int)(rem / 3600);
    rem %= 3600;
    pTm->tm_min = (int)(rem / 60);
    pTm->tm_sec = (int)(rem % 60);
    /* 1970-01-01 was Thursday → wday = (days + 4) % 7 */
    {
        int64_t w = (days % 7) + 4;

        w %= 7;
        if (w < 0) {
            w += 7;
        }
        pTm->tm_wday = (int)w;
    }
    nY = 1970;
    if (days >= 0) {
        for (;;) {
            int nDiy = is_leap(nY) ? 366 : 365;

            if (days < nDiy) {
                break;
            }
            days -= nDiy;
            nY++;
            if (nY > 10000) {
                break;
            }
        }
    } else {
        while (days < 0) {
            nY--;
            if (nY < 1) {
                nY = 1;
                days = 0;
                break;
            }
            days += is_leap(nY) ? 366 : 365;
        }
    }
    pTm->tm_year = nY - 1900;
    pTm->tm_yday = (int)days;
    nYday = (int)days;
    for (nMon = 0; nMon < 12; nMon++) {
        int nDim = days_in_month(nY, nMon);

        if (nYday < nDim) {
            break;
        }
        nYday -= nDim;
    }
    pTm->tm_mon = nMon < 12 ? nMon : 11;
    pTm->tm_mday = nYday + 1;
    pTm->tm_isdst = 0;
    pTm->tm_gmtoff = 0;
    pTm->tm_zone = "UTC";
    return pTm;
}

struct tm *
localtime_r(const time_t *pT, struct tm *pTm)
{
    /* Soft: local == UTC */
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
    /* "Www Mmm dd hh:mm:ss yyyy\n" — classic 26-byte shape */
    int nW;
    int nM;
    int nDay;
    int nH;
    int nMin;
    int nSec;
    int nY;

    if (pTm == NULL || szBuf == NULL) {
        return NULL;
    }
    nW = pTm->tm_wday;
    nM = pTm->tm_mon;
    if (nW < 0 || nW > 6) {
        nW = 0;
    }
    if (nM < 0 || nM > 11) {
        nM = 0;
    }
    nDay = pTm->tm_mday;
    nH = pTm->tm_hour;
    nMin = pTm->tm_min;
    nSec = pTm->tm_sec;
    nY = pTm->tm_year + 1900;
    if (nDay < 1) {
        nDay = 1;
    }
    if (nDay > 31) {
        nDay = 31;
    }
    if (nH < 0) {
        nH = 0;
    }
    if (nH > 23) {
        nH = 23;
    }
    if (nMin < 0) {
        nMin = 0;
    }
    if (nMin > 59) {
        nMin = 59;
    }
    if (nSec < 0) {
        nSec = 0;
    }
    if (nSec > 60) {
        nSec = 60;
    }
    (void)snprintf(szBuf, 26, "%s %s %2d %02d:%02d:%02d %04d\n",
                   s_aWday[nW], s_aMon[nM], nDay, nH, nMin, nSec, nY);
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
    int fNeg = 0;
    int i;
    size_t k;

    if (v < 0) {
        fNeg = 1;
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
    if (fNeg) {
        aTmp[n++] = '-';
    }
    if ((size_t)n > cbLeft) {
        return 0;
    }
    for (i = n - 1, k = 0; i >= 0; i--, k++) {
        p[k] = aTmp[i];
    }
    return (size_t)n;
}

static size_t
emit_str(char *szBuf, size_t nOut, size_t cb, const char *sz)
{
    size_t n;

    if (sz == NULL) {
        return nOut;
    }
    n = strlen(sz);
    if (nOut + n >= cb) {
        return (size_t)-1;
    }
    memcpy(szBuf + nOut, sz, n);
    return nOut + n;
}

static size_t
emit_num(char *szBuf, size_t nOut, size_t cb, int nVal, int nWidth, int fZero)
{
    char aNum[16];
    size_t nAdd;

    nAdd = put_num(aNum, sizeof(aNum) - 1, nVal, nWidth, fZero);
    if (nAdd == 0 && nVal != 0) {
        return (size_t)-1;
    }
    aNum[nAdd] = '\0';
    return emit_str(szBuf, nOut, cb, aNum);
}

size_t
strftime(char *szBuf, size_t cb, const char *szFmt, const struct tm *pTm)
{
    size_t nOut = 0;
    const char *p;
    int nW;
    int nM;
    int nHour12;

    if (szBuf == NULL || cb == 0 || szFmt == NULL || pTm == NULL) {
        return 0;
    }
    nW = pTm->tm_wday;
    nM = pTm->tm_mon;
    if (nW < 0 || nW > 6) {
        nW = 0;
    }
    if (nM < 0 || nM > 11) {
        nM = 0;
    }
    nHour12 = pTm->tm_hour % 12;
    if (nHour12 == 0) {
        nHour12 = 12;
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
        switch (*p) {
        case '%':
            nOut = emit_str(szBuf, nOut, cb, "%");
            break;
        case 'n':
            nOut = emit_str(szBuf, nOut, cb, "\n");
            break;
        case 't':
            nOut = emit_str(szBuf, nOut, cb, "\t");
            break;
        case 'Y':
            nOut = emit_num(szBuf, nOut, cb, pTm->tm_year + 1900, 4, 1);
            break;
        case 'y':
            nOut = emit_num(szBuf, nOut, cb, (pTm->tm_year + 1900) % 100, 2, 1);
            break;
        case 'C':
            nOut = emit_num(szBuf, nOut, cb, (pTm->tm_year + 1900) / 100, 2, 1);
            break;
        case 'm':
            nOut = emit_num(szBuf, nOut, cb, pTm->tm_mon + 1, 2, 1);
            break;
        case 'd':
            nOut = emit_num(szBuf, nOut, cb, pTm->tm_mday, 2, 1);
            break;
        case 'e':
            nOut = emit_num(szBuf, nOut, cb, pTm->tm_mday, 2, 0);
            break;
        case 'H':
            nOut = emit_num(szBuf, nOut, cb, pTm->tm_hour, 2, 1);
            break;
        case 'I':
            nOut = emit_num(szBuf, nOut, cb, nHour12, 2, 1);
            break;
        case 'k':
            nOut = emit_num(szBuf, nOut, cb, pTm->tm_hour, 2, 0);
            break;
        case 'l':
            nOut = emit_num(szBuf, nOut, cb, nHour12, 2, 0);
            break;
        case 'M':
            nOut = emit_num(szBuf, nOut, cb, pTm->tm_min, 2, 1);
            break;
        case 'S':
            nOut = emit_num(szBuf, nOut, cb, pTm->tm_sec, 2, 1);
            break;
        case 'j':
            nOut = emit_num(szBuf, nOut, cb, pTm->tm_yday + 1, 3, 1);
            break;
        case 'w':
            nOut = emit_num(szBuf, nOut, cb, nW, 1, 0);
            break;
        case 'u': {
            int nU = nW == 0 ? 7 : nW;

            nOut = emit_num(szBuf, nOut, cb, nU, 1, 0);
            break;
        }
        case 'p':
            nOut = emit_str(szBuf, nOut, cb, pTm->tm_hour < 12 ? "AM" : "PM");
            break;
        case 'P':
            nOut = emit_str(szBuf, nOut, cb, pTm->tm_hour < 12 ? "am" : "pm");
            break;
        case 'a':
            nOut = emit_str(szBuf, nOut, cb, s_aWday[nW]);
            break;
        case 'A':
            nOut = emit_str(szBuf, nOut, cb, s_aWdayFull[nW]);
            break;
        case 'b':
        case 'h':
            nOut = emit_str(szBuf, nOut, cb, s_aMon[nM]);
            break;
        case 'B':
            nOut = emit_str(szBuf, nOut, cb, s_aMonFull[nM]);
            break;
        case 'z':
            nOut = emit_str(szBuf, nOut, cb, "+0000");
            break;
        case 'Z':
            nOut = emit_str(szBuf, nOut, cb,
                            pTm->tm_zone != NULL ? pTm->tm_zone : "UTC");
            break;
        case 's': {
            struct tm tmCopy = *pTm;
            time_t t = timegm(&tmCopy);
            /* Soft: emit decimal seconds since epoch (may truncate on 32-bit). */
            nOut = emit_num(szBuf, nOut, cb, (int)t, 1, 0);
            break;
        }
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
        case 'X':
        {
            size_t n = strftime(szBuf + nOut, cb - nOut, "%H:%M:%S", pTm);

            if (n == 0 && cb - nOut > 0) {
                szBuf[0] = '\0';
                return 0;
            }
            nOut += n;
            continue;
        }
        case 'R': /* %H:%M */
        {
            size_t n = strftime(szBuf + nOut, cb - nOut, "%H:%M", pTm);

            if (n == 0 && cb - nOut > 0) {
                szBuf[0] = '\0';
                return 0;
            }
            nOut += n;
            continue;
        }
        case 'r': /* %I:%M:%S %p */
        {
            size_t n = strftime(szBuf + nOut, cb - nOut, "%I:%M:%S %p", pTm);

            if (n == 0 && cb - nOut > 0) {
                szBuf[0] = '\0';
                return 0;
            }
            nOut += n;
            continue;
        }
        case 'D': /* %m/%d/%y */
        {
            size_t n = strftime(szBuf + nOut, cb - nOut, "%m/%d/%y", pTm);

            if (n == 0 && cb - nOut > 0) {
                szBuf[0] = '\0';
                return 0;
            }
            nOut += n;
            continue;
        }
        case 'x': /* C locale date */
        {
            size_t n = strftime(szBuf + nOut, cb - nOut, "%m/%d/%y", pTm);

            if (n == 0 && cb - nOut > 0) {
                szBuf[0] = '\0';
                return 0;
            }
            nOut += n;
            continue;
        }
        case 'c': /* C locale date-time */
        {
            size_t n =
                strftime(szBuf + nOut, cb - nOut, "%a %b %e %H:%M:%S %Y", pTm);

            if (n == 0 && cb - nOut > 0) {
                szBuf[0] = '\0';
                return 0;
            }
            nOut += n;
            continue;
        }
        default: {
            char aNum[4];

            aNum[0] = '%';
            aNum[1] = *p;
            aNum[2] = '\0';
            nOut = emit_str(szBuf, nOut, cb, aNum);
            break;
        }
        }
        if (nOut == (size_t)-1) {
            szBuf[0] = '\0';
            return 0;
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
    int nBest = -1;
    size_t nBestLen = 0;

    for (i = 0; i < nCount; i++) {
        size_t n = strlen(aNames[i]);
        size_t j;

        if (n < nBestLen) {
            continue;
        }
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
            nBest = i;
            nBestLen = n;
        }
    }
    if (nBest < 0) {
        return -1;
    }
    *pIdx = nBest;
    *pp = p + nBestLen;
    return 0;
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
            if (*f == ' ' || *f == '\t') {
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
        case 'n':
        case 't':
            while (*s == ' ' || *s == '\t' || *s == '\n') {
                s++;
            }
            break;
        case 'Y': {
            int y;

            if (parse_digits(&s, 4, &y) != 0) {
                return NULL;
            }
            pTm->tm_year = y - 1900;
            break;
        }
        case 'C': {
            int c;

            if (parse_digits(&s, 2, &c) != 0) {
                return NULL;
            }
            /* Combine with existing %y if present; soft store century. */
            pTm->tm_year = (c * 100 - 1900) + ((pTm->tm_year + 1900) % 100);
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
        case 'd':
        case 'e': {
            int d;

            while (*s == ' ') {
                s++;
            }
            if (parse_digits(&s, 2, &d) != 0 || d < 1 || d > 31) {
                return NULL;
            }
            pTm->tm_mday = d;
            break;
        }
        case 'H':
        case 'k': {
            int h;

            while (*s == ' ') {
                s++;
            }
            if (parse_digits(&s, 2, &h) != 0 || h > 23) {
                return NULL;
            }
            pTm->tm_hour = h;
            break;
        }
        case 'I':
        case 'l': {
            int h;

            while (*s == ' ') {
                s++;
            }
            if (parse_digits(&s, 2, &h) != 0 || h < 1 || h > 12) {
                return NULL;
            }
            /* Soft: store 1..12; %p may adjust later. */
            pTm->tm_hour = h % 12;
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
        case 'p':
        case 'P': {
            if ((s[0] == 'A' || s[0] == 'a') &&
                (s[1] == 'M' || s[1] == 'm')) {
                /* 12 AM → 0 */
                if (pTm->tm_hour == 12) {
                    pTm->tm_hour = 0;
                }
                s += 2;
            } else if ((s[0] == 'P' || s[0] == 'p') &&
                       (s[1] == 'M' || s[1] == 'm')) {
                if (pTm->tm_hour < 12) {
                    pTm->tm_hour += 12;
                }
                s += 2;
            } else {
                return NULL;
            }
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
        case 'A': {
            int idx;

            if (match_name(&s, s_aWdayFull, 7, &idx) != 0) {
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
        case 'B': {
            int idx;

            if (match_name(&s, s_aMonFull, 12, &idx) != 0) {
                return NULL;
            }
            pTm->tm_mon = idx;
            break;
        }
        case 'j': {
            int j;

            if (parse_digits(&s, 3, &j) != 0 || j < 1 || j > 366) {
                return NULL;
            }
            pTm->tm_yday = j - 1;
            break;
        }
        case 'w': {
            int w;

            if (parse_digits(&s, 1, &w) != 0 || w > 6) {
                return NULL;
            }
            pTm->tm_wday = w;
            break;
        }
        case 'u': {
            int u;

            if (parse_digits(&s, 1, &u) != 0 || u < 1 || u > 7) {
                return NULL;
            }
            pTm->tm_wday = (u == 7) ? 0 : u;
            break;
        }
        case 'z': {
            /* Soft: accept +HHMM / -HHMM / Z, ignore offset (UTC local). */
            if (*s == 'Z' || *s == 'z') {
                s++;
            } else if (*s == '+' || *s == '-') {
                int hh;
                int mm;
                const char *p0 = s + 1;

                if (parse_digits(&p0, 2, &hh) != 0) {
                    return NULL;
                }
                if (*p0 == ':') {
                    p0++;
                }
                if (parse_digits(&p0, 2, &mm) != 0) {
                    return NULL;
                }
                s = p0;
            } else {
                return NULL;
            }
            break;
        }
        case 'Z': {
            /* Skip a run of letters (timezone name). */
            if ((*s >= 'A' && *s <= 'Z') || (*s >= 'a' && *s <= 'z')) {
                while ((*s >= 'A' && *s <= 'Z') || (*s >= 'a' && *s <= 'z')) {
                    s++;
                }
            } else {
                return NULL;
            }
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
        case 'T':
        case 'X': {
            char *p2 = strptime(s, "%H:%M:%S", pTm);

            if (p2 == NULL) {
                return NULL;
            }
            s = p2;
            break;
        }
        case 'R': {
            char *p2 = strptime(s, "%H:%M", pTm);

            if (p2 == NULL) {
                return NULL;
            }
            s = p2;
            break;
        }
        case 'D':
        case 'x': {
            char *p2 = strptime(s, "%m/%d/%y", pTm);

            if (p2 == NULL) {
                return NULL;
            }
            s = p2;
            break;
        }
        case 'r': {
            char *p2 = strptime(s, "%I:%M:%S %p", pTm);

            if (p2 == NULL) {
                return NULL;
            }
            s = p2;
            break;
        }
        case 'c': {
            char *p2 = strptime(s, "%a %b %e %H:%M:%S %Y", pTm);

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
    int nMday;
    int nHour;
    int nMin;
    int nSec;
    int i;

    if (pTm == NULL) {
        return (time_t)-1;
    }
    nY = pTm->tm_year + 1900;
    nMon = pTm->tm_mon;
    nMday = pTm->tm_mday;
    nHour = pTm->tm_hour;
    nMin = pTm->tm_min;
    nSec = pTm->tm_sec;

    /* Soft normalize out-of-range fields (mktime-shaped). */
    if (nSec >= 60 || nSec < 0) {
        int nAdd = nSec / 60;

        if (nSec < 0) {
            nAdd = (nSec - 59) / 60;
        }
        nSec -= nAdd * 60;
        nMin += nAdd;
    }
    if (nMin >= 60 || nMin < 0) {
        int nAdd = nMin / 60;

        if (nMin < 0) {
            nAdd = (nMin - 59) / 60;
        }
        nMin -= nAdd * 60;
        nHour += nAdd;
    }
    if (nHour >= 24 || nHour < 0) {
        int nAdd = nHour / 24;

        if (nHour < 0) {
            nAdd = (nHour - 23) / 24;
        }
        nHour -= nAdd * 24;
        nMday += nAdd;
    }
    while (nMon < 0) {
        nMon += 12;
        nY--;
    }
    while (nMon > 11) {
        nMon -= 12;
        nY++;
    }
    while (nMday < 1) {
        nMon--;
        if (nMon < 0) {
            nMon = 11;
            nY--;
        }
        nMday += days_in_month(nY, nMon);
    }
    for (;;) {
        int nDim = days_in_month(nY, nMon);

        if (nMday <= nDim) {
            break;
        }
        nMday -= nDim;
        nMon++;
        if (nMon > 11) {
            nMon = 0;
            nY++;
        }
    }

    if (nY < 1970) {
        /* Soft: reject deep pre-epoch for 32-bit time_t safety. */
        for (i = nY; i < 1970; i++) {
            days -= is_leap(i) ? 366 : 365;
        }
    } else {
        for (i = 1970; i < nY; i++) {
            days += is_leap(i) ? 366 : 365;
        }
    }
    for (i = 0; i < nMon; i++) {
        days += days_in_month(nY, i);
    }
    days += nMday - 1;
    {
        int64_t sec = days * 86400LL + (int64_t)nHour * 3600LL +
                      (int64_t)nMin * 60LL + (int64_t)nSec;
        time_t t = (time_t)sec;
        struct tm tmp;

        pTm->tm_year = nY - 1900;
        pTm->tm_mon = nMon;
        pTm->tm_mday = nMday;
        pTm->tm_hour = nHour;
        pTm->tm_min = nMin;
        pTm->tm_sec = nSec;
        if (gmtime_r(&t, &tmp) != NULL) {
            pTm->tm_wday = tmp.tm_wday;
            pTm->tm_yday = tmp.tm_yday;
        }
        pTm->tm_isdst = 0;
        pTm->tm_gmtoff = 0;
        pTm->tm_zone = "UTC";
        return t;
    }
}

time_t
mktime(struct tm *pTm)
{
    /* Soft: local == UTC */
    tzset();
    return timegm(pTm);
}
