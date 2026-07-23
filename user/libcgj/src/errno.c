/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 */
#include <errno.h>
#include <locale.h>
#include <string.h>

int errno;

char *
strerror(int nErr)
{
    switch (nErr) {
    case 0:
        return "Success";
    case EPERM:
        return "Operation not permitted";
    case ENOENT:
        return "No such file or directory";
    case EIO:
        return "Input/output error";
    case ENOMEM:
        return "Cannot allocate memory";
    case EACCES:
        return "Permission denied";
    case EFAULT:
        return "Bad address";
    case EBUSY:
        return "Device or resource busy";
    case EEXIST:
        return "File exists";
    case ENOTDIR:
        return "Not a directory";
    case EISDIR:
        return "Is a directory";
    case EINVAL:
        return "Invalid argument";
    case ENOSPC:
        return "No space left on device";
    case EPIPE:
        return "Broken pipe";
    case ERANGE:
        return "Numerical result out of range";
    case EAGAIN:
        return "Resource temporarily unavailable";
    case ENOSYS:
        return "Function not implemented";
    case ETIMEDOUT:
        return "Connection timed out";
    default:
        return "Unknown error";
    }
}

char *
strsignal(int nSig)
{
    switch (nSig) {
    case 1:
        return "Hangup";
    case 2:
        return "Interrupt";
    case 3:
        return "Quit";
    case 6:
        return "Aborted";
    case 9:
        return "Killed";
    case 11:
        return "Segmentation fault";
    case 13:
        return "Broken pipe";
    case 14:
        return "Alarm clock";
    case 15:
        return "Terminated";
    default:
        return "Unknown signal";
    }
}

/* Locale-aware variants: C locale only → same strings as non-l forms */
char *
strerror_l(int nErr, locale_t loc)
{
    (void)loc;
    return strerror(nErr);
}

char *
strsignal_l(int nSig, locale_t loc)
{
    (void)loc;
    return strsignal(nSig);
}
