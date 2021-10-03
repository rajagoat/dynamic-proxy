# Dynamic Proxy in C

## Overview
This is a dynamically configurable network proxy that uses the POSIX socket APIs to communicate.

## Usage
Note that *this can only be compiled and run on a Linux machine*.
Furthermore, the server host is set to be the CPSC servers at the University of Calgary. This will have to be changed if you are unable to access these servers.

To use this proxy, two files will need to be run in separate terminal windows:

`proxy.c` 

`blocker.c`

in this order. 

To compile and run, use the following commands:

### `gcc -o proxy proxy.c`

### `./proxy`

### `gcc -o blocker blocker.c`

### `./blocker`

in this order.

## NOTES 
The `keywords.txt` file is necessary in order to get expected results from the proxy and blocker.
