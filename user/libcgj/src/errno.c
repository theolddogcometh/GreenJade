/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * errno storage + strerror / strsignal (C locale soft tables).
 *
 * greppable: CGJ_ERRNO_SOFT_TABLE
 * greppable: CGJ_STRSIGNAL_SOFT_TABLE
 *
 * Soft deepen: cover errno.h constants and common Linux signals so
 * strerror_r / strerrordesc_np / smoke KATs see real descriptions.
 * strerrorname_np / sigabbrev_np remain in graph batch surface.
 */
#include <errno.h>
#include <locale.h>
#include <string.h>

int errno;

char *
strerror(int nErr)
{
    /* greppable: CGJ_ERRNO_SOFT_TABLE */
    switch (nErr) {
    case 0:
        return "Success";
    case EPERM:
        return "Operation not permitted";
    case ENOENT:
        return "No such file or directory";
    case ESRCH:
        return "No such process";
    case EINTR:
        return "Interrupted system call";
    case EIO:
        return "Input/output error";
    case ENXIO:
        return "No such device or address";
    case E2BIG:
        return "Argument list too long";
    case ENOEXEC:
        return "Exec format error";
    case EBADF:
        return "Bad file descriptor";
    case ECHILD:
        return "No child processes";
    case EAGAIN:
        return "Resource temporarily unavailable";
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
    case EXDEV:
        return "Invalid cross-device link";
    case ENODEV:
        return "No such device";
    case ENOTDIR:
        return "Not a directory";
    case EISDIR:
        return "Is a directory";
    case EINVAL:
        return "Invalid argument";
    case ENFILE:
        return "Too many open files in system";
    case EMFILE:
        return "Too many open files";
    case ENOTTY:
        return "Inappropriate ioctl for device";
    case EFBIG:
        return "File too large";
    case ENOSPC:
        return "No space left on device";
    case ESPIPE:
        return "Illegal seek";
    case EROFS:
        return "Read-only file system";
    case EPIPE:
        return "Broken pipe";
    case ERANGE:
        return "Numerical result out of range";
    case EDEADLK:
        return "Resource deadlock avoided";
    case ENAMETOOLONG:
        return "File name too long";
    case ENOLCK:
        return "No locks available";
    case ENOSYS:
        return "Function not implemented";
    case ENOTEMPTY:
        return "Directory not empty";
    case ELOOP:
        return "Too many levels of symbolic links";
    case ENOMSG:
        return "No message of desired type";
    case EOVERFLOW:
        return "Value too large for defined data type";
    case EILSEQ:
        return "Invalid or incomplete multibyte or wide character";
    case ENOTSOCK:
        return "Socket operation on non-socket";
    case EDESTADDRREQ:
        return "Destination address required";
    case EMSGSIZE:
        return "Message too long";
    case EPROTOTYPE:
        return "Protocol wrong type for socket";
    case ENOPROTOOPT:
        return "Protocol not available";
    case EPROTONOSUPPORT:
        return "Protocol not supported";
    case ESOCKTNOSUPPORT:
        return "Socket type not supported";
    case ENOTSUP:
        return "Operation not supported";
    case EPFNOSUPPORT:
        return "Protocol family not supported";
    case EAFNOSUPPORT:
        return "Address family not supported by protocol";
    case EADDRINUSE:
        return "Address already in use";
    case EADDRNOTAVAIL:
        return "Cannot assign requested address";
    case ENETDOWN:
        return "Network is down";
    case ENETUNREACH:
        return "Network is unreachable";
    case ENETRESET:
        return "Network dropped connection on reset";
    case ECONNABORTED:
        return "Software caused connection abort";
    case ECONNRESET:
        return "Connection reset by peer";
    case ENOBUFS:
        return "No buffer space available";
    case EISCONN:
        return "Transport endpoint is already connected";
    case ENOTCONN:
        return "Transport endpoint is not connected";
    case ESHUTDOWN:
        return "Cannot send after transport endpoint shutdown";
    case ETOOMANYREFS:
        return "Too many references: cannot splice";
    case ETIMEDOUT:
        return "Connection timed out";
    case ECONNREFUSED:
        return "Connection refused";
    case EHOSTDOWN:
        return "Host is down";
    case EHOSTUNREACH:
        return "No route to host";
    case EALREADY:
        return "Operation already in progress";
    case EINPROGRESS:
        return "Operation now in progress";
    case ESTALE:
        return "Stale file handle";
    case EUCLEAN:
        return "Structure needs cleaning";
    case ENOTNAM:
        return "Not a XENIX named type file";
    case ENAVAIL:
        return "No XENIX semaphores available";
    case EISNAM:
        return "Is a named type file";
    case EREMOTEIO:
        return "Remote I/O error";
    case EDQUOT:
        return "Disk quota exceeded";
    case ENOMEDIUM:
        return "No medium found";
    case EMEDIUMTYPE:
        return "Wrong medium type";
    case ECANCELED:
        return "Operation canceled";
    case ENOKEY:
        return "Required key not available";
    case EKEYEXPIRED:
        return "Key has expired";
    case EKEYREVOKED:
        return "Key has been revoked";
    case EKEYREJECTED:
        return "Key was rejected by service";
    case EOWNERDEAD:
        return "Owner died";
    case ENOTRECOVERABLE:
        return "State not recoverable";
    case ERFKILL:
        return "Operation not possible due to RF-kill";
    case EHWPOISON:
        return "Memory page has hardware error";
    default:
        return "Unknown error";
    }
}

char *
strsignal(int nSig)
{
    /* greppable: CGJ_STRSIGNAL_SOFT_TABLE */
    switch (nSig) {
    case 1:
        return "Hangup";
    case 2:
        return "Interrupt";
    case 3:
        return "Quit";
    case 4:
        return "Illegal instruction";
    case 5:
        return "Trace/breakpoint trap";
    case 6:
        return "Aborted";
    case 7:
        return "Bus error";
    case 8:
        return "Floating point exception";
    case 9:
        return "Killed";
    case 10:
        return "User defined signal 1";
    case 11:
        return "Segmentation fault";
    case 12:
        return "User defined signal 2";
    case 13:
        return "Broken pipe";
    case 14:
        return "Alarm clock";
    case 15:
        return "Terminated";
    case 16:
        return "Stack fault";
    case 17:
        return "Child exited";
    case 18:
        return "Continued";
    case 19:
        return "Stopped (signal)";
    case 20:
        return "Stopped";
    case 21:
        return "Stopped (tty input)";
    case 22:
        return "Stopped (tty output)";
    case 23:
        return "Urgent I/O condition";
    case 24:
        return "CPU time limit exceeded";
    case 25:
        return "File size limit exceeded";
    case 26:
        return "Virtual timer expired";
    case 27:
        return "Profiling timer expired";
    case 28:
        return "Window changed";
    case 29:
        return "I/O possible";
    case 30:
        return "Power failure";
    case 31:
        return "Bad system call";
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
