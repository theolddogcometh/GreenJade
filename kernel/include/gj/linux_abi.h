/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Linux x86_64 syscall surface (public man pages / ABI docs).
 * Not Linux source. Hybrid Option C: hot paths in kernel, cold → personality.
 * Pure C11 — dual MIT OR Apache-2.0.
 * docs/LINUX_ABI_HYBRID.md · docs/PROTON_PERSONALITY.md
 */
#pragma once

#include <gj/types.h>

/* ---- Linux errno (subset; return as -errno in rax) ---------------------- */

#define LINUX_EPERM        1
#define LINUX_ENOENT       2
#define LINUX_ESRCH        3
#define LINUX_EINTR        4
#define LINUX_EIO          5
#define LINUX_ENXIO        6
#define LINUX_E2BIG        7
#define LINUX_ENOEXEC      8
#define LINUX_EBADF        9
#define LINUX_ECHILD      10
#define LINUX_EAGAIN      11
#define LINUX_ENOMEM      12
#define LINUX_EACCES      13
#define LINUX_EFAULT      14
#define LINUX_EBUSY       16
#define LINUX_EEXIST      17
#define LINUX_ENODEV      19
#define LINUX_ENOTDIR     20
#define LINUX_EINVAL      22
#define LINUX_ENFILE      23
#define LINUX_EMFILE      24
#define LINUX_ENOSPC      28
#define LINUX_EROFS       30
#define LINUX_EPIPE       32
#define LINUX_ERANGE      34
#define LINUX_ENOLCK      37
#define LINUX_ENOSYS      38
#define LINUX_ENOTEMPTY   39
#define LINUX_ETIMEDOUT    110
#define LINUX_ECONNREFUSED 111

/* ---- x86_64 Linux syscall numbers (public ABI) -------------------------- */

#define LINUX_NR_read              0
#define LINUX_NR_write             1
#define LINUX_NR_open              2
#define LINUX_NR_close             3
#define LINUX_NR_stat              4
#define LINUX_NR_fstat             5
#define LINUX_NR_lstat             6
#define LINUX_NR_poll              7
#define LINUX_NR_lseek             8
#define LINUX_NR_mmap              9
#define LINUX_NR_mprotect         10
#define LINUX_NR_munmap           11
#define LINUX_NR_brk              12
#define LINUX_NR_rt_sigaction     13
#define LINUX_NR_rt_sigprocmask   14
#define LINUX_NR_ioctl            16
#define LINUX_NR_pread64          17
#define LINUX_NR_pwrite64         18
#define LINUX_NR_readv            19
#define LINUX_NR_writev           20
#define LINUX_NR_preadv          295
#define LINUX_NR_pwritev         296
#define LINUX_NR_preadv2         327
#define LINUX_NR_pwritev2        328
#define LINUX_NR_access           21
#define LINUX_NR_pipe             22
#define LINUX_NR_select           23
#define LINUX_NR_sched_yield      24
#define LINUX_NR_mremap           25
#define LINUX_NR_msync            26
#define LINUX_NR_mincore          27
#define LINUX_NR_madvise         28
#define LINUX_NR_dup              32
#define LINUX_NR_dup2             33
#define LINUX_NR_dup3            292
#define LINUX_NR_recvmmsg        299
#define LINUX_NR_sendmmsg        307
#define LINUX_NR_nanosleep        35
#define LINUX_NR_getitimer        36
#define LINUX_NR_setitimer        38
#define LINUX_NR_getpid           39
#define LINUX_NR_socket           41
#define LINUX_NR_connect          42
#define LINUX_NR_accept           43
#define LINUX_NR_sendto           44
#define LINUX_NR_recvfrom         45
#define LINUX_NR_sendmsg          46
#define LINUX_NR_recvmsg          47
#define LINUX_NR_shutdown         48
#define LINUX_NR_socketpair       53
#define LINUX_NR_bind             49
#define LINUX_NR_listen           50
#define LINUX_NR_getsockname      51
#define LINUX_NR_getpeername      52
#define LINUX_NR_setsockopt       54
#define LINUX_NR_getsockopt       55
#define LINUX_NR_clone            56
#define LINUX_NR_fork             57
#define LINUX_NR_vfork            58
#define LINUX_NR_execve           59
#define LINUX_NR_execveat        322
#define LINUX_NR_exit             60
#define LINUX_NR_wait4            61
#define LINUX_NR_kill             62
#define LINUX_NR_tkill           200
#define LINUX_NR_tgkill          234
#define LINUX_NR_uname            63
#define LINUX_NR_rt_sigreturn     15
#define LINUX_NR_rt_sigpending    127
#define LINUX_NR_rt_sigtimedwait  128
#define LINUX_NR_rt_sigqueueinfo  129
#define LINUX_NR_rt_sigsuspend    130
#define LINUX_NR_sigaltstack      131
#define LINUX_NR_sched_setparam   142
#define LINUX_NR_sched_getparam   143
#define LINUX_NR_sched_setscheduler 144
#define LINUX_NR_sched_getscheduler 145
#define LINUX_NR_sched_get_priority_max 146
#define LINUX_NR_sched_get_priority_min 147
#define LINUX_NR_sched_rr_get_interval 148
#define LINUX_NR_unshare          272
#define LINUX_NR_setns            308
#define LINUX_NR_chroot            161
#define LINUX_NR_mount            165
#define LINUX_NR_umount2          166
#define LINUX_NR_pivot_root       155
#define LINUX_NR_swapon           167
#define LINUX_NR_swapoff          168
#define LINUX_NR_reboot           169
#define LINUX_NR_sethostname      170
#define LINUX_NR_setdomainname    171
#define LINUX_NR_readahead        187
#define LINUX_NR_sync_file_range  277
#define LINUX_NR_vmsplice         278
#define LINUX_NR_getdents          78
#define LINUX_NR_syslog           103
#define LINUX_NR_getsid           124
#define LINUX_NR_bdflush          134
#define LINUX_NR_personality      135
#define LINUX_NR_ustat            136
#define LINUX_NR_sysfs            139
#define LINUX_NR_fcntl            72
#define LINUX_NR_fsync            74
#define LINUX_NR_fdatasync        75
#define LINUX_NR_sync             162
#define LINUX_NR_syncfs           306
#define LINUX_NR_ftruncate        77
#define LINUX_NR_getcwd           79
#define LINUX_NR_chdir            80
#define LINUX_NR_fchdir           81
#define LINUX_NR_rename           82
#define LINUX_NR_mkdir            83
#define LINUX_NR_rmdir            84
#define LINUX_NR_creat            85
#define LINUX_NR_link             86
#define LINUX_NR_unlink           87
#define LINUX_NR_symlink          88
#define LINUX_NR_readlink         89
#define LINUX_NR_readlinkat      267
#define LINUX_NR_chmod            90
#define LINUX_NR_fchmodat        268
#define LINUX_NR_getxattr        191
#define LINUX_NR_lgetxattr       192
#define LINUX_NR_fgetxattr       193
#define LINUX_NR_setxattr        188
#define LINUX_NR_lsetxattr       189
#define LINUX_NR_fsetxattr       190
#define LINUX_NR_listxattr       194
#define LINUX_NR_llistxattr      195
#define LINUX_NR_flistxattr      196
#define LINUX_NR_removexattr     197
#define LINUX_NR_lremovexattr    198
#define LINUX_NR_fremovexattr    199
#define LINUX_NR_utime            132
#define LINUX_NR_utimensat        280
#define LINUX_NR_getuid          102
#define LINUX_NR_getgid          104
#define LINUX_NR_setuid          105
#define LINUX_NR_setgid          106
#define LINUX_NR_geteuid         107
#define LINUX_NR_getegid         108
#define LINUX_NR_setpgid         109
#define LINUX_NR_getppid         110
#define LINUX_NR_getpgrp         111
#define LINUX_NR_setsid          112
#define LINUX_NR_setreuid        113
#define LINUX_NR_setregid        114
#define LINUX_NR_getgroups       115
#define LINUX_NR_setgroups       116
#define LINUX_NR_setresuid       117
#define LINUX_NR_getresuid       118
#define LINUX_NR_setresgid       119
#define LINUX_NR_getresgid       120
#define LINUX_NR_mlock           149
#define LINUX_NR_munlock         150
#define LINUX_NR_mlockall        151
#define LINUX_NR_munlockall      152
#define LINUX_NR_mlock2          325
#define LINUX_NR_pkey_mprotect   329
#define LINUX_NR_pkey_alloc      330
#define LINUX_NR_pkey_free       331
#define LINUX_NR_gettimeofday     96
#define LINUX_NR_getrlimit        97
#define LINUX_NR_setrlimit       160
#define LINUX_NR_getrusage        98
#define LINUX_NR_sysinfo          99
#define LINUX_NR_times           100
#define LINUX_NR_time            201
#define LINUX_NR_settimeofday    164
#define LINUX_NR_getpriority     140
#define LINUX_NR_setpriority     141
#define LINUX_NR_capget          125
#define LINUX_NR_capset          126
#define LINUX_NR_sched_setaffinity 203
#define LINUX_NR_sched_getaffinity 204
#define LINUX_NR_getcpu          309
#define LINUX_NR_alarm           37
#define LINUX_NR_pause           34
#define LINUX_NR_flock           73
#define LINUX_NR_fchmod          91
#define LINUX_NR_fchown          93
#define LINUX_NR_arch_prctl      158
#define LINUX_NR_gettid          186
#define LINUX_NR_futex           202
#define LINUX_NR_set_tid_address 218
#define LINUX_NR_clock_gettime   228
#define LINUX_NR_clock_getres    229
#define LINUX_NR_clock_nanosleep 230
#define LINUX_NR_exit_group      231
#define LINUX_NR_epoll_wait      232
#define LINUX_NR_epoll_ctl       233
#define LINUX_NR_sendfile         40
#define LINUX_NR_prctl           157
#define LINUX_NR_getdents64      217
#define LINUX_NR_waitid          247
#define LINUX_NR_openat          257
#define LINUX_NR_mkdirat         258
#define LINUX_NR_newfstatat      262
#define LINUX_NR_unlinkat        263
#define LINUX_NR_faccessat       269
#define LINUX_NR_pselect6        270
#define LINUX_NR_ppoll           271
#define LINUX_NR_set_robust_list 273
#define LINUX_NR_get_robust_list 274
#define LINUX_NR_splice          275
#define LINUX_NR_tee             276
#define LINUX_NR_inotify_init1   294
#define LINUX_NR_inotify_add_watch 254
#define LINUX_NR_inotify_rm_watch  255
#define LINUX_NR_renameat        264
#define LINUX_NR_linkat          265
#define LINUX_NR_symlinkat       266
#define LINUX_NR_renameat2       316
#define LINUX_NR_copy_file_range 326
#define LINUX_NR_clock_settime   227
#define LINUX_NR_clock_adjtime   305
#define LINUX_NR_io_pgetevents   333
#define LINUX_NR_userfaultfd     323
#define LINUX_NR_seccomp         317
#define LINUX_NR_bpf             321
#define LINUX_NR_keyctl          250
#define LINUX_NR_add_key         248
#define LINUX_NR_request_key     249
#define LINUX_NR_name_to_handle_at 303
#define LINUX_NR_open_by_handle_at 304
#define LINUX_NR_epoll_pwait     281
#define LINUX_NR_signalfd4       282
#define LINUX_NR_timerfd_create  283
#define LINUX_NR_eventfd         284
#define LINUX_NR_fallocate       285
#define LINUX_NR_timerfd_settime 286
#define LINUX_NR_timerfd_gettime 287
#define LINUX_NR_accept4         288
#define LINUX_NR_eventfd2        290
#define LINUX_NR_io_setup        206
#define LINUX_NR_io_destroy      207
#define LINUX_NR_io_getevents    208
#define LINUX_NR_io_submit       209
#define LINUX_NR_io_cancel       210
#define LINUX_NR_epoll_create    213
#define LINUX_NR_epoll_create1   291
#define LINUX_NR_pipe2           293
#define LINUX_NR_prlimit64       302
#define LINUX_NR_getrandom       318
#define LINUX_NR_memfd_create    319
#define LINUX_NR_process_vm_readv  310
#define LINUX_NR_process_vm_writev 311
#define LINUX_NR_membarrier      324
#define LINUX_NR_statfs          137
#define LINUX_NR_fstatfs         138
#define LINUX_NR_statx           332
#define LINUX_NR_rseq            334
#define LINUX_NR_openat2         437
#define LINUX_NR_faccessat2      439
#define LINUX_NR_pidfd_send_signal 424
#define LINUX_NR_pidfd_open      434
#define LINUX_NR_clone3          435
#define LINUX_NR_close_range     436
#define LINUX_NR_landlock_create_ruleset 444
#define LINUX_NR_landlock_add_rule       445
#define LINUX_NR_landlock_restrict_self  446
#define LINUX_NR_memfd_secret    447
#define LINUX_NR_process_mrelease 448
#define LINUX_NR_futex_waitv     449
#define LINUX_NR_set_mempolicy_home_node 450
#define LINUX_NR_cachestat       451
#define LINUX_NR_fchmodat2       452
#define LINUX_NR_map_shadow_stack 453
#define LINUX_NR_futex_wake      454
#define LINUX_NR_futex_wait      455
#define LINUX_NR_futex_requeue   456
#define LINUX_NR_io_uring_setup  425
#define LINUX_NR_io_uring_enter  426
#define LINUX_NR_io_uring_register 427
#define LINUX_NR_open_tree       428
#define LINUX_NR_move_mount      429
#define LINUX_NR_fsopen          430
#define LINUX_NR_fsconfig        431
#define LINUX_NR_fsmount         432
#define LINUX_NR_fspick          433
#define LINUX_NR_mount_setattr   442
#define LINUX_NR_quotactl_fd     443
#define LINUX_NR_pidfd_getfd     438
#define LINUX_NR_process_madvise 440
#define LINUX_NR_epoll_pwait2    441
#define LINUX_NR_fanotify_init   300
#define LINUX_NR_fanotify_mark   301
#define LINUX_NR_kcmp            312
#define LINUX_NR_finit_module    313
#define LINUX_NR_sched_setattr   314
#define LINUX_NR_sched_getattr   315
#define LINUX_NR_kexec_file_load 320
#define LINUX_NR_quotactl        179
#define LINUX_NR_nfsservctl      180
#define LINUX_NR_getpmsg         181
#define LINUX_NR_putpmsg         182
#define LINUX_NR_afs_syscall     183
#define LINUX_NR_tuxcall         184
#define LINUX_NR_security        185
#define LINUX_NR_lookup_dcookie  212
#define LINUX_NR_epoll_ctl_old   214
#define LINUX_NR_epoll_wait_old  215
#define LINUX_NR_remap_file_pages 216
#define LINUX_NR_restart_syscall 219
#define LINUX_NR_mbind           237
#define LINUX_NR_set_mempolicy   238
#define LINUX_NR_get_mempolicy   239
#define LINUX_NR_migrate_pages   256
#define LINUX_NR_move_pages      279
#define LINUX_NR_perf_event_open 298
#define LINUX_NR_timer_create    222
#define LINUX_NR_timer_settime   223
#define LINUX_NR_timer_gettime   224
#define LINUX_NR_timer_getoverrun 225
#define LINUX_NR_timer_delete    226
#define LINUX_NR_mq_open         240
#define LINUX_NR_mq_unlink       241
#define LINUX_NR_mq_timedsend    242
#define LINUX_NR_mq_timedreceive 243
#define LINUX_NR_mq_notify       244
#define LINUX_NR_mq_getsetattr   245
#define LINUX_NR_inotify_init    253

/* clock_gettime clock ids */
#define LINUX_CLOCK_REALTIME           0
#define LINUX_CLOCK_MONOTONIC          1
#define LINUX_CLOCK_PROCESS_CPUTIME_ID 2
#define LINUX_CLOCK_THREAD_CPUTIME_ID  3
#define LINUX_CLOCK_MONOTONIC_RAW      4
#define LINUX_CLOCK_BOOTTIME           7

/* mmap prot / flags (subset) */
#define LINUX_PROT_READ   0x1
#define LINUX_PROT_WRITE  0x2
#define LINUX_PROT_EXEC   0x4
#define LINUX_PROT_NONE   0x0
#define LINUX_MAP_SHARED    0x01
#define LINUX_MAP_PRIVATE   0x02
#define LINUX_MAP_FIXED     0x10
#define LINUX_MAP_ANONYMOUS 0x20
#define LINUX_MREMAP_MAYMOVE 1
#define LINUX_MREMAP_FIXED   2

/* arch_prctl codes */
#define LINUX_ARCH_SET_FS 0x1002
#define LINUX_ARCH_GET_FS 0x1003
#define LINUX_ARCH_SET_GS 0x1001
#define LINUX_ARCH_GET_GS 0x1004

/*
 * Linux x86_64 calling convention at syscall entry:
 *   nr  = rax
 *   a0  = rdi, a1 = rsi, a2 = rdx, a3 = r10, a4 = r8, a5 = r9
 *   ret = rax  (negative = -errno)
 */
struct gj_linux_regs {
    u64 u64Nr;
    u64 u64Arg0;
    u64 u64Arg1;
    u64 u64Arg2;
    u64 u64Arg3;
    u64 u64Arg4;
    u64 u64Arg5;
    i64 i64Ret; /* set by dispatcher */
};

struct linux_timespec {
    i64 i64Sec;
    i64 i64Nsec;
};

/* Path classification for hybrid Option C */
enum gj_linux_path {
    GJ_LINUX_PATH_HOT  = 0, /* handled entirely in kernel */
    GJ_LINUX_PATH_COLD = 1, /* upcall / personality */
    GJ_LINUX_PATH_NONE = 2, /* unknown → ENOSYS */
};
