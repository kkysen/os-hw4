#pragma once

#include "prelude.h"

#include "color.h"
#include "source_location.h"

typedef struct {
	_Atomic u64 num_failed;
	_Atomic u64 num_tests;
} CheckStats;

INLINE u64 CheckStats_num_tests(const CheckStats *self)
{
	return self->num_tests;
}

INLINE u64 CheckStats_num_passed(const CheckStats *self)
{
	// can't underflow
	return self->num_tests - self->num_failed;
}

INLINE u64 CheckStats_num_failed(const CheckStats *self)
{
	return self->num_failed;
}

typedef struct {
	ColorOutput output;
	CheckStats *stats;
} Check;

INLINE bool Check_success(const Check *self)
{
	return self->stats->num_failed == 0;
}

Check Check_new(ColorOutput output);

void Check_free(const Check *self);

void Check_eq(Check *self, SourceLocation location, const void *actual,
	      const void *expected, bool (*eq)(const void *, const void *),
	      void (*print)(FILE *, const void *), const char *prefix);

void Check_errno(Check *self, SourceLocation location, int expected_errno);

void Check_print_summary(const Check *self);

__attribute__((noreturn)) void Check_exit(const Check *self);
