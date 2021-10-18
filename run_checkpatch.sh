#!/bin/bash

INIT_COMMIT=$(git log --reverse --pretty=format:"%h" | head -n 1)
IGNORES=FILE_PATH_CHANGES,SPDX_LICENSE_TAG

HW4_IGNORES=$IGNORES,EXPORT_SYMBOL,ENOSYS,AVOID_EXTERNS
git diff $INIT_COMMIT linux/ user/module/ | linux/scripts/checkpatch.pl --ignore $HW4_IGNORES

# ENOSYS: Looks like ENOSYS is often misused in the kernel so checkpatch always
#         warns on seeing ENOSYS. Our use is correct, though!
#         (Nonexistent syscall was called)
# EXPORT_SYMBOL: Buggy for our use-case, let's just ignore this.
# AVOID_EXTERNS: Checkpatch will complain if you extern in a .c file instead of
#                a .h file. Given that supermom is so simple, we'll ignore this.

# I changed this to only run on files in `linux/`.
# It doesn't make sense to run it on other files.
# For example, it tries to run it on symlinks (like the `.clang-format` one).
# And it tries to run it on the `justfile`, which is in a language it doesn't understand.
