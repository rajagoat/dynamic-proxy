Author: Athul Rajagopal

To use this proxy, two files will need to be run in separate terminal windows:
    - proxy.c
    - blocker.c
in this order. 

To compile, use the following commands:
    gcc -o proxy proxy.c
    ./proxy
    gcc -o blocker blocker.c
    ./blocker
in this order.

NOTE: The keywords.txt file is necessary in order to get expected results from the proxy and blocker.

The proxy is fully functional as a censorship proxy. The blocker allows for dynamically configuring the blocked keywords, and has a
user interface which should be intuitive to use.

Testing was primarily done at home, through SSH into the Linux machines at the University.