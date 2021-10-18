#include "prelude.h"

#include <sys/syscall.h>
#include <sys/wait.h>
#include <assert.h>
#include <string.h>

#include "check.h"
#include "error.h"
#include "supermom.h"

static bool is_module_loaded(const char *module_name)
{
	// `/proc/modules` contains all the loaded modules with the module name and then a space on each line.
	FILE *f = fopen("/proc/modules", "r");
	if (!f) {
		errno_reset();
		return false;
	}

	size_t module_len = strlen(module_name);

	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	while ((read = getline(&line, &len, f)) != -1) {
		const char *space = memchr(line, ' ', len);
		if (!space) {
			continue;
		}
		size_t i = space - line;
		if (i != module_len) {
			continue;
		}
		if (memcmp(module_name, line, module_len) == 0) {
			return true;
		}
	}

	fclose(f);

	return false;
}

typedef struct {
	bool is_loaded;
	Check checker;
	FILE *output;
} SupermomCheck;

SupermomCheck SupermomCheck_new(bool dynamic_supermom)
{
	setlinebuf(stderr);
	Check checker = Check_new(ColorOutput_default(stderr));
	FILE *output = fdopen(3, "w");
	if (!output) {
		errno_reset();
		output = stdout;
	}
	bool is_loaded =
		!dynamic_supermom ? true : is_module_loaded("supermom");
	return (SupermomCheck){
		.is_loaded = is_loaded,
		.checker = checker,
		.output = output,
	};
}

__attribute__((noreturn)) static void
SupermomCheck_exit(const SupermomCheck *self)
{
	setbuf(stderr, NULL);
	if (self->output != stdout) {
		fclose(self->output);
	}
	Check_exit(&self->checker);
}

static void SupermomCheck_check(SupermomCheck *self, SourceLocation location,
				pid_t pid, uid_t *uid, int error,
				const char *message)
{
	supermom(pid, uid);
	Check_errno(&self->checker, location, self->is_loaded ? error : ENOSYS);
	if (message && self->is_loaded) {
		fprintf(self->output, "%s\n", message);
		fflush(self->output); // need to flush or we flush multiple times when exiting from the forked child
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
	SupermomCheck checker = SupermomCheck_new(true);

#define check(pid, uid, error, message)                                        \
	SupermomCheck_check(&checker, HERE, pid, uid, error, message)

	const uid_t euid = geteuid();
	bool is_root = euid == 0;

	check(getpid(), NULL, EINVAL, "Not Yo Mama");
	check(1 /* init */, NULL, EINVAL, "Not Yo Mama");

	int error = is_root ? 0 : EACCES;

	check(getppid(), NULL, error, NULL);

	uid_t supermom_euid = -1;
	check(getppid(), &supermom_euid, error, NULL);
	if (checker.is_loaded) {
		Check_eq(&checker.checker, HERE, &supermom_euid, &euid, uid_eq,
			 uid_print, "euid");
	}

#undef check

	SupermomCheck_exit(&checker);
}
