#include<linux/module.h>
#include<linux/sched/signal.h>
#include<linux/pid_namespace.h>
#include<asm/io.h>
// #include <linux/mm_types.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <asm/pgtable.h>  // Optional, depends on specific use case
#include <linux/sched.h>  // For task_struct and process management

unsigned long virt2phys(struct mm_struct* mm, unsigned long vpage) {
  pgd_t* pgd;
  p4d_t* p4d;
  pud_t* pud;
  pmd_t* pmd;
  pte_t* pte;

  struct page* page;
  pgd = pgd_offset(mm, vpage);
  if (pgd_none(*pgd) || pgd_bad(*pgd))
    return 0;
  p4d = p4d_offset(pgd, vpage);
  if (p4d_none(*p4d) || p4d_bad(*p4d))
    return 0;
  pud = pud_offset(p4d, vpage);
  if (pud_none(*pud) || pud_bad(*pud))
    return 0;
  pmd = pmd_offset(pud, vpage);
  if (pmd_none(*pmd) || pmd_bad(*pmd))
    return 0;
  pte = pte_offset_map(pmd, vpage);
  if (!pte)
    return 0;
  page = pte_page(*pte);
  if (!page)
    return 0;

  unsigned long physical_page_addr = (unsigned long)page_to_phys(page);
  pte_unmap(pte);

  // handle unmapped page
  if (physical_page_addr == 70368744173568)
    return 0;

  return physical_page_addr;
}

void print_proc_stats(void) {
  struct task_struct* thechild;

  for_each_process(thechild) {
    pid_t pid = thechild->pid; // Process ID
    char* name = thechild->comm; // Process name

    // only show process IDs that are greater than 650
    if (pid > 650) {

      // check pages of the process
      struct vm_area_struct* vma = 0;
      unsigned long vpage;
      int total_page_count = 0;

      if (thechild->mm && thechild->mm->mmap) {
        for (vma = thechild->mm->mmap; vma; vma = vma->vm_next) {
          for (vpage = vma->vm_start; vpage < vma->vm_end; vpage += PAGE_SIZE) {
            unsigned long physical_page_addr = virt2phys(thechild->mm, vpage);

            // printk(KERN_INFO "virt2phys() returns %d", physical_page_addr);

            if (physical_page_addr != 0) {
              total_page_count++;
            }
            // else {
            //   printk(KERN_INFO "virt2phys() returned 0!! Page is NOT allocated");
            // }
          }
        }
        printk(KERN_INFO"PID: % ld, NAME : % s Total page count %d", pid, name, total_page_count);

        // for extra credit:
        // count non-contigious pages by checking all ISOLATED pages in memory (check addresses for this?)
        // count contigious pages by checking all pages next to at least ONE other page
      }
    }
  }
}



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

