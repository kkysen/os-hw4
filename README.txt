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
specifically the `test_supermom` executable,
which will print out which tests failed and how many passed.

### part4
TODO
