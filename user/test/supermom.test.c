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

/// Return -1 if `/sys/kernel/debug/superlog/success` is not there.
static i64 read_success_count()
{
	FILE *f = fopen("/sys/kernel/debug/superlog/success", "r");
	if (!f) {
		errno_reset();
		return -1;
	}
	u64 count;
	if (fscanf(f, "%lu", &count) != 1)
		return -1;
	return (i64)count;
}

typedef struct {
	bool is_loaded;
	Check checker;
	u64 success_count;
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
		.success_count = 0,
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
	if (supermom(pid, uid) == 0) {
		self->success_count++;
	}
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

static bool success_count_eq(const void *a_void, const void *b_void)
{
	const i64 *a = a_void;
	const i64 *b = b_void;
	return *a == *b;
}

static void success_count_print(FILE *output, const void *count_void)
{
	const i64 *count = count_void;
	if (*count == -1) {
		fprintf(output, "none");
	} else {
		fprintf(output, "%ld", *count);
	}
}

/// stdout or fd 3 (if open) should be what's printed in the kernel log.
int main()
{
	i64 start_success_count = read_success_count();
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

	const i64 actual_end_success_count = read_success_count();
	const i64 expected_end_success_count =
		start_success_count == -1 ?
			      -1 :
			      start_success_count + (i64)checker.success_count;
	Check_eq(&checker.checker, HERE, &actual_end_success_count,
		 &expected_end_success_count, success_count_eq,
		 success_count_print, "success count");
	fprintf(stderr, "success count: %ld + %lu = %ld\n", start_success_count,
		checker.success_count, actual_end_success_count);

	SupermomCheck_exit(&checker);
}
