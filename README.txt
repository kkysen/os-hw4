Khyber Sen
ks3343
hw4

### part1
Took a long time but I loaded it and it worked.

### part2
Much faster than part1 and it worked.

### part3
This part is working.
One thing I added that wasn't explicitly specified was that `-EFAULT` is returned when `copy_to_user` fails,
which is what the documentation for `copy_to_user` suggests.
Running `make -C user/test/ run` runs tests for all the possible configurations specified (except the `EFAULT` one),
and will test as root and as the current user.

##### Usermode Tests
I put the usermode test code in `user/test/`.
Running `make run` there should run the test (requires `sudo`),
specifically the `supermom.test` executable,
which will print out which tests failed and how many passed,
and also print the expected output that will be printed to the kernel log.

`just test` tests all of this if you have `just` installed,
including comparing the kernel log output to its expected value.

### part4
This part is working.
On init, it sets the supermom handler using the exported `set_supermom_handler` function,
and then sets it back to `NULL` on exit (the stub returns `-ENOSYS` when the handler is `NULL`).

##### Usermode Tests
The `supermom.test` executable now sets different expected errors
based on if the supermom module is loaded (checked by reading `/proc/modules`).
So all the tests should pass whether or not the supermom module is loaded.

`just test-mod` works `just test` between a few repetitions of `insmod` and `rmmod` `supermom.ko`.

### part5
TODO
