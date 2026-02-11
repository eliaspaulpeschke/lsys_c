packcc lsystem.peg
gcc -g -O0 -c lsys_parse_util.c
gcc -g -O0 -c lsystem.c
gcc -g -O0 -c lsystem2.c
gcc lsystem.o lsystem2.o lsys_parse_util.o -o tst
rm *.o
