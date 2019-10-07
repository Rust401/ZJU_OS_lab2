# Operating System(2018-2019Fall&Winter)LAB2

```shell
Project Name:   Add System Call
Student Name:   Hu.Zhaodong
Email       :   zhaodonghu94@zju.edu.cn
phone       :   15700080428
Date        :   2018.9-2018.12
```
## TARGET
* Rebuild the kernel of linux.
* Learn the linux system call. Understand and master the **frame**, **user interface**, **parameters passing** and the **enter/return** of the the linux system call. Read the source code of the linux kernel and add a easy syscall. Further understand the process of linux operating system handle the system call. Learn about the page fault and know the construct and features of `task_struct`.

## CONTENT
Add a system call with no parameters pass-in in the current operating system. The funtion of this syscall is to **statistic the totoal page fault count in the OS and the current processes' page fault and each process's dirty page number**. Strictly speaking, the page fault count here is just the times to invoke ``do_page_fault()`.  
**Main content:**
* Rebuild the kernel
* Add the name of new syscall
* Use a c lib to package
* Add syscall number
* Add the entry in the syscall table
* Modify the struct and the function of the kernel about statistic the page fault infomation of process
* `sys_mysyscall` implement
* Write a user mode test program

## STEPS
1) Download kernel source code from the mirror [aliyun](http://mirrors.aliyun.com/linux-kernel/). Download two files `linux_4.6.tar.xz` and `patch-4.6.xz`.
2) Depoly the kernel source code
    ```cpp
    //update apt-get
    apt-get update
    //install environment
    apt-get install kernel-package libncurses5-dev
    //Decompression the kernel
    xz -d linux-4.6.tar.xz
    //Take apart the kernel file
    tar xvf linux-4.6.tar
    //cp patch and kernel file to /usr/src and enter /usr/src
    cp  linux-4.6  /usr/src –rf
    cp  patch-4.6.xz  /usr/src
    cd  /usr/src
    //patch the kernel
    xz -d patch-4.6.xz | patch -p1
    //link the file name to a symbol
    ln  -s  /usr/src/linux-4.6/  linux
3) config the kernl
    ```cpp
    //enter kernel file
    cd  /usr/src/linux
    //remove all the config and .o
    make mrproper
    //copy the original kernel config to the new kernel
    cp ../linux-headers-4.4.0-31-generic/.config  .
    //config the kernel then load->OK->Save->OK->EXIT->EXIT
    make menuconfig

    ```
4) Add syscall number
    ```
    /usr/include/asm-generic/unistd.h 
    /usr/src/linux/include/uapi/asm-generic/unistd.h
    ```
    Find the 223 and do the modifiy below:
    ```
    --  #define __NR3264_fadvise64 223
    --__SC_COMP(__NR3264_fadvise64, sys_fadvise64_64, compat_sys_fadvise64_64)

    ++  #define __NR_mysyscall 223
    ++  __SYSCALL(__NR_mysyscall, sys_mysyscall)
    ```

    ![fig1](https://github.com/Rust401/ZJU_OS_lab2/blob/master/Screenshot%20from%202018-12-31%2019-38-26.png?raw=true)
5) Modify the syscall table
    ```
    arch/x86/entry/syscalls/syscall_32.tbl
    ```
    ![fig2](https://github.com/Rust401/ZJU_OS_lab2/blob/master/Screenshot%20from%202018-12-31%2019-42-06.png?raw=true)
6) Modify the kernel code to statistic the page fault count of system and the current process  
    ```
    /usr/src/linux/include/linux/mm.h
    ++ extern unsigned  long  pfcount;
    ```
    ![fig3](https://github.com/Rust401/ZJU_OS_lab2/blob/master/Screenshot%20from%202018-12-31%2019-46-46.png?raw=true)
    ```
    /usr/src/linux/include/linux/sched.h
    ++ unsigned  long  pf;
    ```
    ![fig4](https://github.com/Rust401/ZJU_OS_lab2/blob/master/Screenshot%20from%202018-12-31%2019-49-27.png?raw=true)  
    Set the pf in `task_struct` to `0`
    ```
    /usr/src/linux/kernel/fork.c
    ++ tsk->pf=0;
    ```
    ![fig5](https://github.com/Rust401/ZJU_OS_lab2/blob/master/Screenshot%20from%202018-12-31%2019-55-50.png?raw=true)  
    ```
    /usr/src/linux/arch/x86/mm/fault.c
       …
    ++ unsigned long pfcount;

    __do_page_fault(struct pt_regs *regs, unsigned long error_code)
   {
   	    …
    ++ pfcount++;
    ++ current->pf++;
       …
   }

    ```  
    ![fig6](https://github.com/Rust401/ZJU_OS_lab2/blob/master/Screenshot%20from%202018-12-31%2020-05-34.png?raw=true)  

    ![fig7](https://github.com/Rust401/ZJU_OS_lab2/blob/master/Screenshot%20from%202018-12-31%2020-05-49.png?raw=true)
7) Implement the sys_mysyscall
    ```
    /usr/src/linux/kernel/sys.c
    ```
    ```c
    extern unsigned long pfcount;
    asmlinkage int sys_mysyscall(void)
    {
        struct task_struct* p=NULL;
        printk("@@@@syscall 223 start!\n");
        printk("@Total page fault: %lu times\n",pfcount);
        printk("@Current process's page fault: %lu times\n",current->pf);
        for(p=&init_task;(p=next_task(p))!=&init_task;)
        {
                printk("@------------------------------------------------------------\n");
                printk("@PID: %d\n",p->pid);
                printk("@NAME: %s\n",p->comm);
                printk("@DIRTY PAGES: %d pages\n",p->nr_dirtied);

        }
        printk("!!!!syscall 223 end!");
        return 0;
    }
    ```
    ![fig8](https://github.com/Rust401/ZJU_OS_lab2/blob/master/Screenshot%20from%202018-12-31%2020-11-48.png?raw=true)
8) Compile and reboot the kernel
    ```cpp
    //compile kernel
    sudo make bzImage –j2
    //compile module
    sudo make modules –j2
    //install module
    sudo make modules_install
    //install kernel
    sudo make install
    //modify grub
    sudo mkinitramfs  4.6.0  -o  /boot/initrd.img-4.6.0
    sudo update-grub2
    //reboot
    sudo reboot
    ```
9) Write a user mode program
    ```cpp
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
    ```
    **usage:**
    ```
    gcc -o test test.c
    ./test
    ```  
    **output:**
    ![fig9](https://github.com/Rust401/ZJU_OS_lab2/blob/master/Screenshot%20from%202018-12-31%2023-08-54.png?raw=true)
## Answer the problem
1) Run the your own test program and record the **current process' page fault** the the **system page fault**. The output page fault count is the concept of the page fault in out operating system concept?
    ```
    Times               Total_page_fault                current_page_fault
    1                   777702                          64
    2                   1682329                         64
    3                   1876058                         65
    4                   2134360                         64
    5                   2185915                         62
    6                   2437081                         66
    7                   2669816                         63
    ```
    **Answer:**  
    In our program the page fault count is actully the time the function `do_page_fault` be invoked. But the invoke of `do_page_falut` may due the reason **access violation**. So the page fault count in this program is **not less than** the truth page fault count.
2) Except the kernel modify, is there any other way to add or modify a syscall?  
**Answer:**  
We could use the **kernel module** to modify the funciton address in `sys_call_table` to our own funtion address. We should find the address of sys_call_table and remove **write protection**. There is 2 way to remove the write protection: **modify the 16th bit in CR0** or **set the virtual address page entry's attributes**. The more detail please visit my [Github](https://github.com/Rust401/ZJU_OS_lab2).
3) For a operating system, is the syscall safe?  
**Answer:**  
* If some syscalls are modified, then the clib rely on them may lose their orignal function and the application may not run as we design.
* Generally, modify the syscall need to re-compile the kernel. And if we make some mistake during the modifing of the kernel the complie may fail. Or even compile finished but the logic in the program have problem. That may do some subtle damage to the kernel.
* If we add some system call with the syscall number not used instead of modify the syscall on the original syscall function, it is more safe.

## Remark
* While modified the kernel, check the path carefully because the linux kernel source file will have many files with same or similar file name. And the Hierarchical structure may be confusing for us beginer.
* Before we add our declaration or definiton, read the source file first and be sure to put the new item into the right position. In this lab, I put the `tsk->pf=0` into the `/usr/src/linux/kernel/fork.c` after the `tsk->stack=ti;`. There is a code `err=arch_dup_task_struct(tsk,orig);` which do the opetation to copy the orignal task image to the new task. So after that copy behavior, we could modified the item in `tsk` such as `tsk->pf=0`. Or the operation will be cover by the `arch_dup_task`.
* The 32bit Ubuntu will use the differet syscall table to the 64bit ubuntu. In `/usr/src/linux/arch/x86/entry/syscalls/syscall_32.tbl` is the table for 32bit. If we just modify the call number in it, out 64 bit machine will not know the number we define. To avoid that problem, use the 32bie ubuntu.
* There is still a bug that in `my_syscall` funtion. I can't output the last output code `printk("!!!!syscall 223 end!");` before `return 0;`. It's just not in the `/var/log/kern.log`. If we do `./test` once more. The "!!!!syscall 223 end!" will print with the new page fault imformation. This bug is wait to be fixed.
* For more detail, visit my [Github](https://github.com/Rust401/ZJU_OS_lab2).





    
