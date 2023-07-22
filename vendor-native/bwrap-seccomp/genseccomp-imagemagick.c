#include <seccomp.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <linux/sched.h>
#include <linux/seccomp.h>

/*
Based on https://gitlab.fem-net.de/webhosting/bwrap-seccomp,
highly modified to only allow a very small subset of syscalls.
*/

int main(int argc, char *argv[]) {
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ERRNO(EPERM));
    //scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ALLOW);

    // syscalls for bubblewrap
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(brk), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(arch_prctl), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(uname), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(readlink), 0);

    // syscalls for ImageMagick
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(wait4), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(access), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(openat), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(newfstatat), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(close), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(pread64), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(munmap), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mprotect), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(lseek), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(times), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(faccessat), 0);

    // ImageMagick tries to execute these syscalls, but they seem to be optional:
    //seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(arch_prctl), 0);
    //seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(getrandom), 0);
    //seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(sched_getaffinity), 0);
    //seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(clock_gettime), 0);
    //seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(rt_sigprocmask), 0);
    //seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(rt_sigaction), 0);
    //seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(sysinfo), 0);

    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(set_robust_list), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(rseq), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(prlimit64), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(readlinkat), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(getrandom), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mprotect), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(newfstatat), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(set_tid_address), 0);

    seccomp_export_bpf(ctx, 1);
    seccomp_release(ctx);
    return 0;
}
