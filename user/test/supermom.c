#include "prelude.h"

#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <sys/wait.h>

#include "error.h"

#ifndef __NR_supermom
#define __NR_supermom 500
#endif

int supermom(pid_t pid, uid_t *uid)
{
	return (int)syscall(__NR_supermom, pid, uid);
}

void check(pid_t pid, uid_t *uid, SourceLocation location, int error,
	   const char *message)
{
	supermom(pid, uid);
	check_errno(stderr, location, error);
	if (message) {
		fprintf(stdout, "%s\n", message);
		fflush(stdout); // need to flush or we flush multiple times when exiting from the forked child
	}
}

void run_as_root_or_not(bool as_root)
{
	bool is_root = geteuid() == 0;
	if (as_root) {
		assert(is_root);
	} else {
		assert(seteuid(getuid()) != -1);
	}
}

static bool uid_eq(const void *a_void, const void *b_void)
{
	const uid_t *a = a_void;
	const uid_t *b = b_void;
	return *a == *b;
}

static void uid_print(FILE *output, const void *uid_void)
{
	const uid_t *uid = uid_void;
	fprintf(output, "%d", *uid);
}

void test(bool as_root)
{
	uid_t euid_expected = geteuid();

	const pid_t pid = fork();
	assert(pid != -1);
	if (pid != 0) {
		// parent
		wait(NULL);
		return;
	}

	// child
	run_as_root_or_not(as_root);
	int error = as_root ? 0 : EACCES;

	check(getppid(), NULL, HERE, error, NULL);

	uid_t euid_actual = -1;
	check(getppid(), &euid_actual, HERE, error, NULL);
	check_eq(stderr, HERE, &euid_actual, &euid_expected, uid_eq, uid_print,
		 "euid");
	exit(EXIT_SUCCESS);
}

int main()
{
	setlinebuf(stderr);
	check(getpid(), NULL, HERE, EINVAL, "Not Yo Mama");
	check(1 /* init */, NULL, HERE, EINVAL, "Not Yo Mama");

	test(true);
	test(false);

	return EXIT_SUCCESS;
}
