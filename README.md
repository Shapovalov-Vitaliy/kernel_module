Linux module example
====================
This is the linux kernel module that saves the current time every minute in /tmp/current_time.txt in hh:mm format.

Use the following for build and run:  
>make  
>sudo insmod kmodule.ko

Use the following to remove:  
>sudo rmmod kmodule
