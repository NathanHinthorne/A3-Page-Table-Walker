#include<linux/module.h>
#include<linux/sched/signal.h>

#include<linux/pid_namespace.h>
#include<asm/io.h>


void print_proc_stats(void) {
  struct task_struct* thechild;
  for_each_process(thechild) {
    pid_t pid = thechild->pid;
    char* name = thechild->comm;
    // only show process IDs that are greater than 650
    if (pid > 650) {
      printk(KERN_INFO "PID: %ld, %s", pid, name);
    }
  }
}


// void page_display(void) {
//   struct vm_area_struct* vma = 0;
//   unsigned long vpage;
//   if (task->mm && task->mm->mmap)
//     for (vma = task->mm->mmap; vma; vma = vma->vm_next)
//       for (vpage = vma->vm_start; vpage < vma->vm_end; vpage += PAGE_SIZE)
//         unsigned long physical_page_addr = virt2phys(task->mm, vpage);
// }

int proc_init(void) {
  printk(KERN_INFO "helloModule: kernel module initialized\n");

  printk(KERN_INFO "PROCESS REPORT:\n");
  printk(KERN_INFO "proc_id, proc_name, contig_pages, noncontig_pages, total_pages\n");
  print_proc_stats();

  return 0;
}


void proc_cleanup(void) {
  printk(KERN_INFO "helloModule: performing cleanup of module\n");
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);

