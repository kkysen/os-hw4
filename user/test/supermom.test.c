#include "prelude.h"

#include <sys/syscall.h>
#include <sys/wait.h>
#include <assert.h>

#include "check.h"
#include "error.h"
#include "supermom.h"

void check(pid_t pid, uid_t *uid, Check *checker, SourceLocation location,
	   int error, FILE *output, const char *message)
{
	supermom(pid, uid);
	Check_errno(checker, location, error);
	if (message) {
		fprintf(output, "%s\n", message);
		fflush(output); // need to flush or we flush multiple times when exiting from the forked child
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

/// stdout or fd 3 (if open) should be what's printed in the kernel log.
int main()
{
	setlinebuf(stderr);
	Check checker = Check_new(ColorOutput_default(stderr));
	FILE *output = fdopen(3, "w");
	if (!output) {
		errno_reset();
		output = stdout;
	}

	const uid_t euid = geteuid();
	bool is_root = euid == 0;

	check(getpid(), NULL, &checker, HERE, EINVAL, output, "Not Yo Mama");
	check(1 /* init */, NULL, &checker, HERE, EINVAL, output,
	      "Not Yo Mama");

	int error = is_root ? 0 : EACCES;

	check(getppid(), NULL, &checker, HERE, error, output, NULL);

	uid_t supermom_euid = -1;
	check(getppid(), &supermom_euid, &checker, HERE, error, output, NULL);
	Check_eq(&checker, HERE, &supermom_euid, &euid, uid_eq, uid_print,
		 "euid");

	if (output != stdout) {
		fclose(output);
	}
	Check_exit(&checker);
}
