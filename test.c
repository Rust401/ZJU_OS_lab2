#include<unistd.h>
#include<sys/syscall.h>
#include<stdio.h>
#include<stdlib.h>
#define __NR_mysyscall 223

int main()
{
        syscall(__NR_mysyscall);
        system("dmesg");
        return 0;
}