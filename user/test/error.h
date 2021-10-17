#pragma once

#include "prelude.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

INLINE int errno_reset()
{
	int error = errno;
	errno = 0;
	return error;
}

const char *errno_name(int error);
