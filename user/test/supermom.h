#pragma once

#include "prelude.h"

#include <sys/types.h>

#ifndef __NR_supermom
#define __NR_supermom 500
#endif

int supermom(pid_t pid, uid_t *uid);
