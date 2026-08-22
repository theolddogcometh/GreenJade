/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * POSIX / Linux x86_64 signal name table for vendored dash.
 *
 * Replaces the omitted GPL bash host generator src/mksignames.c.
 * Original table: names are POSIX trap/kill spellings without a SIG
 * prefix (HUP, INT, …), indexed by signal number. trap.c and jobs.c
 * expect:
 *
 *   extern char *signal_names[];
 *   signal_names[0] == "EXIT"
 *   signal_names[n] valid for 0 <= n < NSIG
 *
 * Dual MIT OR Apache-2.0. Not derived from bash mksignames or dash
 * generated output.
 */
#include <stddef.h>

#ifndef NSIG
#define NSIG 65
#endif

/*
 * Linux x86_64 classic + realtime names. Unused slots are "" so
 * strcasecmp in decode_signal() is safe (never NULL).
 */
static char s_exit[] = "EXIT";
static char s_hup[] = "HUP";
static char s_int[] = "INT";
static char s_quit[] = "QUIT";
static char s_ill[] = "ILL";
static char s_trap[] = "TRAP";
static char s_abrt[] = "ABRT";
static char s_bus[] = "BUS";
static char s_fpe[] = "FPE";
static char s_kill[] = "KILL";
static char s_usr1[] = "USR1";
static char s_segv[] = "SEGV";
static char s_usr2[] = "USR2";
static char s_pipe[] = "PIPE";
static char s_alrm[] = "ALRM";
static char s_term[] = "TERM";
static char s_stkflt[] = "STKFLT";
static char s_chld[] = "CHLD";
static char s_cont[] = "CONT";
static char s_stop[] = "STOP";
static char s_tstp[] = "TSTP";
static char s_ttin[] = "TTIN";
static char s_ttou[] = "TTOU";
static char s_urg[] = "URG";
static char s_xcpu[] = "XCPU";
static char s_xfsz[] = "XFSZ";
static char s_vtalrm[] = "VTALRM";
static char s_prof[] = "PROF";
static char s_winch[] = "WINCH";
static char s_io[] = "IO";
static char s_pwr[] = "PWR";
static char s_sys[] = "SYS";
static char s_empty[] = "";
static char s_rtmin[] = "RTMIN";
static char s_rtmin1[] = "RTMIN+1";
static char s_rtmin2[] = "RTMIN+2";
static char s_rtmin3[] = "RTMIN+3";
static char s_rtmin4[] = "RTMIN+4";
static char s_rtmin5[] = "RTMIN+5";
static char s_rtmin6[] = "RTMIN+6";
static char s_rtmin7[] = "RTMIN+7";
static char s_rtmin8[] = "RTMIN+8";
static char s_rtmin9[] = "RTMIN+9";
static char s_rtmin10[] = "RTMIN+10";
static char s_rtmin11[] = "RTMIN+11";
static char s_rtmin12[] = "RTMIN+12";
static char s_rtmin13[] = "RTMIN+13";
static char s_rtmin14[] = "RTMIN+14";
static char s_rtmin15[] = "RTMIN+15";
static char s_rtmin16[] = "RTMIN+16";
static char s_rtmin17[] = "RTMIN+17";
static char s_rtmin18[] = "RTMIN+18";
static char s_rtmin19[] = "RTMIN+19";
static char s_rtmin20[] = "RTMIN+20";
static char s_rtmin21[] = "RTMIN+21";
static char s_rtmin22[] = "RTMIN+22";
static char s_rtmin23[] = "RTMIN+23";
static char s_rtmin24[] = "RTMIN+24";
static char s_rtmin25[] = "RTMIN+25";
static char s_rtmin26[] = "RTMIN+26";
static char s_rtmin27[] = "RTMIN+27";
static char s_rtmin28[] = "RTMIN+28";
static char s_rtmin29[] = "RTMIN+29";
static char s_rtmax[] = "RTMAX";

char *signal_names[NSIG] = {
    s_exit,     /*  0 EXIT (dash trap 0) */
    s_hup,      /*  1 SIGHUP */
    s_int,      /*  2 SIGINT */
    s_quit,     /*  3 SIGQUIT */
    s_ill,      /*  4 SIGILL */
    s_trap,     /*  5 SIGTRAP */
    s_abrt,     /*  6 SIGABRT */
    s_bus,      /*  7 SIGBUS */
    s_fpe,      /*  8 SIGFPE */
    s_kill,     /*  9 SIGKILL */
    s_usr1,     /* 10 SIGUSR1 */
    s_segv,     /* 11 SIGSEGV */
    s_usr2,     /* 12 SIGUSR2 */
    s_pipe,     /* 13 SIGPIPE */
    s_alrm,     /* 14 SIGALRM */
    s_term,     /* 15 SIGTERM */
    s_stkflt,   /* 16 SIGSTKFLT */
    s_chld,     /* 17 SIGCHLD */
    s_cont,     /* 18 SIGCONT */
    s_stop,     /* 19 SIGSTOP */
    s_tstp,     /* 20 SIGTSTP */
    s_ttin,     /* 21 SIGTTIN */
    s_ttou,     /* 22 SIGTTOU */
    s_urg,      /* 23 SIGURG */
    s_xcpu,     /* 24 SIGXCPU */
    s_xfsz,     /* 25 SIGXFSZ */
    s_vtalrm,   /* 26 SIGVTALRM */
    s_prof,     /* 27 SIGPROF */
    s_winch,    /* 28 SIGWINCH */
    s_io,       /* 29 SIGIO / SIGPOLL */
    s_pwr,      /* 30 SIGPWR */
    s_sys,      /* 31 SIGSYS */
    s_empty,    /* 32 kernel RT reserved */
    s_empty,    /* 33 kernel RT reserved */
    s_rtmin,    /* 34 glibc SIGRTMIN */
    s_rtmin1,
    s_rtmin2,
    s_rtmin3,
    s_rtmin4,
    s_rtmin5,
    s_rtmin6,
    s_rtmin7,
    s_rtmin8,
    s_rtmin9,
    s_rtmin10,
    s_rtmin11,
    s_rtmin12,
    s_rtmin13,
    s_rtmin14,
    s_rtmin15,
    s_rtmin16,
    s_rtmin17,
    s_rtmin18,
    s_rtmin19,
    s_rtmin20,
    s_rtmin21,
    s_rtmin22,
    s_rtmin23,
    s_rtmin24,
    s_rtmin25,
    s_rtmin26,
    s_rtmin27,
    s_rtmin28,
    s_rtmin29,  /* 63 */
    s_rtmax     /* 64 SIGRTMAX */
};
