-g = Debugging information with line.
-finstrument-functions = for entry and exit

Usage:
root@embsys-VirtualBox:~/github/intx/usrc/gcc_inst_func# gcc  -g -finstrument-functions  gcc_profiling.c
root@embsys-VirtualBox:~/github/intx/usrc/gcc_inst_func# ./a.out
Function Entry : 0x400696 0x7f03592f1830
Function Entry : 0x4005d6 0x4006ba
Inside foo!
Function Exit : 0x4005d6 0x4006ba
Function Entry : 0x400609 0x4006c4
Inside boo!
Function Exit : 0x400609 0x4006c4
Function Exit : 0x400696 0x7f03592f1830
root@embsys-VirtualBox:~/github/intx/usrc/gcc_inst_func# addr2line -e a.out 0x400609
/home/embsys/github/intx/usrc/gcc_inst_func/gcc_profiling.c:12

