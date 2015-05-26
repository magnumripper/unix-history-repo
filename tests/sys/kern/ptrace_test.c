/*-
 * Copyright (c) 2015 John Baldwin <jhb@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <atf-c.h>

/*
 * Verify that a parent debugger process "sees" the exit of a debugged
 * process exactly once when attached via PT_TRACE_ME.
 */
ATF_TC_WITHOUT_HEAD(ptrace__parent_wait_after_trace_me);
ATF_TC_BODY(ptrace__parent_wait_after_trace_me, tc)
{
	pid_t child, wpid;
	int status;

	ATF_REQUIRE((child = fork()) != -1);
	if (child == 0) {
		/* Child process. */
		ATF_REQUIRE(ptrace(PT_TRACE_ME, 0, NULL, 0) != -1);

		/* Trigger a stop. */
		raise(SIGSTOP);

		exit(1);
	}

	/* Parent process. */

	/* The first wait() should report the stop from SIGSTOP. */
	wpid = waitpid(child, &status, 0);
	ATF_REQUIRE(wpid == child);
	ATF_REQUIRE(WIFSTOPPED(status));
	ATF_REQUIRE(WSTOPSIG(status) == SIGSTOP);

	/* Continue the child ignoring the SIGSTOP. */
	ATF_REQUIRE(ptrace(PT_CONTINUE, child, (caddr_t)1, 0) != -1);

	/* The second wait() should report the exit status. */
	wpid = waitpid(child, &status, 0);
	ATF_REQUIRE(wpid == child);
	ATF_REQUIRE(WIFEXITED(status));
	ATF_REQUIRE(WEXITSTATUS(status) == 1);

	/* The child should no longer exist. */
	wpid = waitpid(child, &status, 0);
	ATF_REQUIRE(wpid == -1);
	ATF_REQUIRE(errno == ECHILD);
}

/*
 * Verify that a parent debugger process "sees" the exit of a debugged
 * process exactly once when attached via PT_ATTACH.
 */
ATF_TC_WITHOUT_HEAD(ptrace__parent_wait_after_attach);
ATF_TC_BODY(ptrace__parent_wait_after_attach, tc)
{
	pid_t child, wpid;
	int cpipe[2], status;
	char c;

	ATF_REQUIRE(pipe(cpipe) == 0);
	ATF_REQUIRE((child = fork()) != -1);
	if (child == 0) {
		/* Child process. */
		close(cpipe[0]);

		/* Wait for the parent to attach. */
		ATF_REQUIRE(read(cpipe[1], &c, sizeof(c)) == 0);

		exit(1);
	}
	close(cpipe[1]);

	/* Parent process. */

	/* Attach to the child process. */
	ATF_REQUIRE(ptrace(PT_ATTACH, child, NULL, 0) == 0);

	/* The first wait() should report the SIGSTOP from PT_ATTACH. */
	wpid = waitpid(child, &status, 0);
	ATF_REQUIRE(wpid == child);
	ATF_REQUIRE(WIFSTOPPED(status));
	ATF_REQUIRE(WSTOPSIG(status) == SIGSTOP);

	/* Continue the child ignoring the SIGSTOP. */
	ATF_REQUIRE(ptrace(PT_CONTINUE, child, (caddr_t)1, 0) != -1);

	/* Signal the child to exit. */
	close(cpipe[0]);

	/* The second wait() should report the exit status. */
	wpid = waitpid(child, &status, 0);
	ATF_REQUIRE(wpid == child);
	ATF_REQUIRE(WIFEXITED(status));
	ATF_REQUIRE(WEXITSTATUS(status) == 1);

	/* The child should no longer exist. */
	wpid = waitpid(child, &status, 0);
	ATF_REQUIRE(wpid == -1);
	ATF_REQUIRE(errno == ECHILD);
}

/*
 * Verify that a parent process "sees" the exit of a debugged process only
 * after the debugger has seen it.
 */
ATF_TC_WITHOUT_HEAD(ptrace__parent_sees_exit_after_child_debugger);
ATF_TC_BODY(ptrace__parent_sees_exit_after_child_debugger, tc)
{
	pid_t child, debugger, wpid;
	int cpipe[2], dpipe[2], status;
	char c;

	ATF_REQUIRE(pipe(cpipe) == 0);
	ATF_REQUIRE((child = fork()) != -1);

	if (child == 0) {
		/* Child process. */
		close(cpipe[0]);

		/* Wait for parent to be ready. */
		ATF_REQUIRE(read(cpipe[1], &c, sizeof(c)) == sizeof(c));

		exit(1);
	}
	close(cpipe[1]);

	ATF_REQUIRE(pipe(dpipe) == 0);
	ATF_REQUIRE((debugger = fork()) != -1);

	if (debugger == 0) {
		/* Debugger process. */
		close(dpipe[0]);

		ATF_REQUIRE(ptrace(PT_ATTACH, child, NULL, 0) != -1);

		wpid = waitpid(child, &status, 0);
		ATF_REQUIRE(wpid == child);
		ATF_REQUIRE(WIFSTOPPED(status));
		ATF_REQUIRE(WSTOPSIG(status) == SIGSTOP);

		ATF_REQUIRE(ptrace(PT_CONTINUE, child, (caddr_t)1, 0) != -1);

		/* Signal parent that debugger is attached. */
		ATF_REQUIRE(write(dpipe[1], &c, sizeof(c)) == sizeof(c));

		/* Wait for parent's failed wait. */
		ATF_REQUIRE(read(dpipe[1], &c, sizeof(c)) == 0);

		wpid = waitpid(child, &status, 0);
		ATF_REQUIRE(wpid == child);
		ATF_REQUIRE(WIFEXITED(status));
		ATF_REQUIRE(WEXITSTATUS(status) == 1);

		exit(0);
	}
	close(dpipe[1]);

	/* Parent process. */

	/* Wait for the debugger to attach to the child. */
	ATF_REQUIRE(read(dpipe[0], &c, sizeof(c)) == sizeof(c));

	/* Release the child. */
	ATF_REQUIRE(write(cpipe[0], &c, sizeof(c)) == sizeof(c));
	ATF_REQUIRE(read(cpipe[0], &c, sizeof(c)) == 0);
	close(cpipe[0]);

	/*
	 * Wait for the child to exit.  This is kind of gross, but
	 * there is not a better way.
	 */
	for (;;) {
		struct kinfo_proc kp;
		size_t len;
		int mib[4];

		mib[0] = CTL_KERN;
		mib[1] = KERN_PROC;
		mib[2] = KERN_PROC_PID;
		mib[3] = child;
		len = sizeof(kp);
		if (sysctl(mib, nitems(mib), &kp, &len, NULL, 0) == -1) {
			/* The KERN_PROC_PID sysctl fails for zombies. */
			ATF_REQUIRE(errno == ESRCH);
			break;
		}
		usleep(5000);
	}

	/*
	 * This wait should return an empty pid.  The parent should
	 * see the child as non-exited until the debugger sees the
	 * exit.
	 */
	wpid = waitpid(child, &status, WNOHANG);
	ATF_REQUIRE(wpid == 0);

	/* Signal the debugger to wait for the child. */
	close(dpipe[0]);

	/* Wait for the debugger. */
	wpid = waitpid(debugger, &status, 0);
	ATF_REQUIRE(wpid == debugger);
	ATF_REQUIRE(WIFEXITED(status));
	ATF_REQUIRE(WEXITSTATUS(status) == 0);

	/* The child process should now be ready. */
	wpid = waitpid(child, &status, WNOHANG);
	ATF_REQUIRE(wpid == child);
	ATF_REQUIRE(WIFEXITED(status));
	ATF_REQUIRE(WEXITSTATUS(status) == 1);
}

/*
 * Verify that a parent process "sees" the exit of a debugged process
 * only after a non-direct-child debugger has seen it.  In particular,
 * various wait() calls in the parent must avoid failing with ESRCH by
 * checking the parent's orphan list for the debugee.
 */
ATF_TC_WITHOUT_HEAD(ptrace__parent_sees_exit_after_unrelated_debugger);
ATF_TC_BODY(ptrace__parent_sees_exit_after_unrelated_debugger, tc)
{
	pid_t child, debugger, fpid, wpid;
	int cpipe[2], dpipe[2], status;
	char c;

	ATF_REQUIRE(pipe(cpipe) == 0);
	ATF_REQUIRE((child = fork()) != -1);

	if (child == 0) {
		/* Child process. */
		close(cpipe[0]);

		/* Wait for parent to be ready. */
		ATF_REQUIRE(read(cpipe[1], &c, sizeof(c)) == sizeof(c));

		exit(1);
	}
	close(cpipe[1]);

	ATF_REQUIRE(pipe(dpipe) == 0);
	ATF_REQUIRE((debugger = fork()) != -1);

	if (debugger == 0) {
		/* Debugger parent. */

		/*
		 * Fork again and drop the debugger parent so that the
		 * debugger is not a child of the main parent.
		 */
		ATF_REQUIRE((fpid = fork()) != -1);
		if (fpid != 0)
			exit(2);

		/* Debugger process. */
		close(dpipe[0]);

		ATF_REQUIRE(ptrace(PT_ATTACH, child, NULL, 0) != -1);

		wpid = waitpid(child, &status, 0);
		ATF_REQUIRE(wpid == child);
		ATF_REQUIRE(WIFSTOPPED(status));
		ATF_REQUIRE(WSTOPSIG(status) == SIGSTOP);

		ATF_REQUIRE(ptrace(PT_CONTINUE, child, (caddr_t)1, 0) != -1);

		/* Signal parent that debugger is attached. */
		ATF_REQUIRE(write(dpipe[1], &c, sizeof(c)) == sizeof(c));

		/* Wait for parent's failed wait. */
		ATF_REQUIRE(read(dpipe[1], &c, sizeof(c)) == 0);

		wpid = waitpid(child, &status, 0);
		ATF_REQUIRE(wpid == child);
		ATF_REQUIRE(WIFEXITED(status));
		ATF_REQUIRE(WEXITSTATUS(status) == 1);

		exit(0);
	}

	/* Parent process. */

	/* Wait for the debugger parent process to exit. */
	wpid = waitpid(debugger, &status, 0);
	ATF_REQUIRE(wpid == debugger);
	ATF_REQUIRE(WIFEXITED(status));
	ATF_REQUIRE(WEXITSTATUS(status) == 2);

	/* A WNOHANG wait here should see the non-exited child. */
	wpid = waitpid(child, &status, WNOHANG);
	ATF_REQUIRE(wpid == 0);

	/* Wait for the debugger to attach to the child. */
	ATF_REQUIRE(read(dpipe[0], &c, sizeof(c)) == sizeof(c));

	/* Release the child. */
	ATF_REQUIRE(write(cpipe[0], &c, sizeof(c)) == sizeof(c));
	ATF_REQUIRE(read(cpipe[0], &c, sizeof(c)) == 0);
	close(cpipe[0]);

	/*
	 * Wait for the child to exit.  This is kind of gross, but
	 * there is not a better way.
	 */
	for (;;) {
		struct kinfo_proc kp;
		size_t len;
		int mib[4];

		mib[0] = CTL_KERN;
		mib[1] = KERN_PROC;
		mib[2] = KERN_PROC_PID;
		mib[3] = child;
		len = sizeof(kp);
		if (sysctl(mib, nitems(mib), &kp, &len, NULL, 0) == -1) {
			/* The KERN_PROC_PID sysctl fails for zombies. */
			ATF_REQUIRE(errno == ESRCH);
			break;
		}
		usleep(5000);
	}

	/*
	 * This wait should return an empty pid.  The parent should
	 * see the child as non-exited until the debugger sees the
	 * exit.
	 */
	wpid = waitpid(child, &status, WNOHANG);
	ATF_REQUIRE(wpid == 0);

	/* Signal the debugger to wait for the child. */
	close(dpipe[0]);

	/* Wait for the debugger. */
	ATF_REQUIRE(read(dpipe[1], &c, sizeof(c)) == 0);

	/* The child process should now be ready. */
	wpid = waitpid(child, &status, WNOHANG);
	ATF_REQUIRE(wpid == child);
	ATF_REQUIRE(WIFEXITED(status));
	ATF_REQUIRE(WEXITSTATUS(status) == 1);
}

ATF_TP_ADD_TCS(tp)
{

	ATF_TP_ADD_TC(tp, ptrace__parent_wait_after_trace_me);
	ATF_TP_ADD_TC(tp, ptrace__parent_wait_after_attach);
	ATF_TP_ADD_TC(tp, ptrace__parent_sees_exit_after_child_debugger);
	ATF_TP_ADD_TC(tp, ptrace__parent_sees_exit_after_unrelated_debugger);

	return (atf_no_error());
}
