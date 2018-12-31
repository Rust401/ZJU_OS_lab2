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