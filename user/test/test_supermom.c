#include "prelude.h"

#include <sys/syscall.h>
#include <sys/wait.h>
#include <assert.h>

#include "check.h"
#include "error.h"
#include "supermom.h"

void check(pid_t pid, uid_t *uid, Check *checker, SourceLocation location,
	   int error, const char *message)
{
	supermom(pid, uid);
	Check_errno(checker, location, error);
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

void test(Check *checker, bool as_root)
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

	check(getppid(), NULL, checker, HERE, error, NULL);

	uid_t euid_actual = -1;
	check(getppid(), &euid_actual, checker, HERE, error, NULL);
	Check_eq(checker, HERE, &euid_actual, &euid_expected, uid_eq, uid_print,
		 "euid");
	exit(EXIT_SUCCESS);
}

/// stdout should be what's printed in the kernel log.
int main()
{
	setlinebuf(stderr);
	Check checker = Check_new(ColorOutput_default(stderr));

	check(getpid(), NULL, &checker, HERE, EINVAL, "Not Yo Mama");
	check(1 /* init */, NULL, &checker, HERE, EINVAL, "Not Yo Mama");

	test(&checker, true);
	test(&checker, false);

	Check_exit(&checker);
}
