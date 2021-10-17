#include "check.h"

#include <sys/mman.h>
#include <assert.h>

#include "error.h"

Check Check_new(ColorOutput output)
{
	// put it in shared memory b/c we might fork
	CheckStats *stats = mmap(NULL, sizeof(*stats), PROT_READ | PROT_WRITE,
				 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	assert(stats != MAP_FAILED);
	return (Check){
		.output = output,
		.stats = stats,
	};
}

void Check_free(const Check *self)
{
	assert(munmap(self->stats, sizeof(*self->stats)) != -1);
}

static u64 CheckStats_add(CheckStats *self, bool passed)
{
	u64 num_tests = ++self->num_tests;
	if (!passed) {
		self->num_failed++;
	}
	return num_tests;
}

void Check_eq(Check *self, SourceLocation location, const void *actual,
	      const void *expected, bool (*eq)(const void *, const void *),
	      void (*print)(FILE *, const void *), const char *prefix)
{
	bool passed = eq(actual, expected);
	u64 test_num = CheckStats_add(self->stats, passed);
	if (passed) {
		return;
	}

	ColorOutput co = self->output;
	FILE *o = co.output;

	ColorOutput_printf(co, Colors(Color_FG_Yellow, Color_Bright),
			   "[test %lu] ", test_num);

	SourceLocation_print(location, co);

	fprintf(o, " ");
	if (prefix) {
		fprintf(o, "%s: ", prefix);
	}

	ColorOutput_printf(co, Colors(Color_Bright), "expected");
	fprintf(o, " ");
	ColorOutput_set(co, Colors(Color_FG_Green, Color_Bright));
	print(o, expected);
	ColorOutput_reset(co);
	fprintf(o, " but ");
	ColorOutput_printf(co, Colors(Color_Bright), "found");
	fprintf(o, " ");
	ColorOutput_set(co, Colors(Color_FG_Red, Color_Bright));
	print(o, actual);
	ColorOutput_reset(co);

	fprintf(o, "\n");
	fflush(o);
}

static bool errno_eq(const void *a_void, const void *b_void)
{
	const int *a = a_void;
	const int *b = b_void;
	return *a == *b;
}

static void errno_print(FILE *output, const void *e_void)
{
	const int *e = e_void;
	fprintf(output, "%s", errno_name(*e));
}

void Check_errno(Check *self, SourceLocation location, int expected_errno)
{
	const int expected = expected_errno;
	const int actual = errno_reset();
	Check_eq(self, location, &actual, &expected, errno_eq, errno_print,
		 NULL);
}

void Check_print_summary(const Check *self)
{
	u64 num_total = self->stats->num_tests;
	u64 num_failed = self->stats->num_failed;
	u64 num_passed = num_total - num_failed;

	ColorOutput co = self->output;
	FILE *o = co.output;

	fprintf(o, "\n");
	ColorOutput_printf(co, Colors(Color_FG_Green, Color_Bright), "%lu/%lu",
			   num_passed, num_total);
	fprintf(o, " tests ");
	ColorOutput_printf(co, Colors(Color_Bright), "passed");
	fprintf(o, ", ");
	ColorOutput_printf(
		co, Colors(num_failed == 0 ? Color_FG_Green : Color_FG_Red),
		"%lu", num_failed);
	fprintf(o, " ");
	ColorOutput_printf(co, Colors(Color_Bright), "failures");
	fprintf(o, "\n");
	fflush(o);
}

void Check_exit(const Check *self)
{
	bool success = Check_success(self);
	Check_print_summary(self);
	Check_free(self);
	exit(success ? EXIT_SUCCESS : EXIT_FAILURE);
}
