# rw-kernel-module

A simple read and write kernel module.
All compilable code written in ldd.c

Run command: make
Load module: 'sudo insmod ldd.ko'
Read from 'my_driver': 'cat /proc/my_driver/'
Write to 'my_driver': 'echo "Test" > /proc/my_driver/"
    (Note buffer size of 98)
Remove module: 'sudo rmmod ldd'
