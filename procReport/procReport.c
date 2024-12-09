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

/**
 * @author Nathan Hinthorne
 * @author Caleb Krauter
*/

/**
 * Checks if the mapping is valid for a given page and if the addressible space is valid for
 * the corresponding valid pageframe address.
 * @param mm a memory map struct pointer
 * @param vpage an unsigned long for the virtual page.
 * @return physical address or 0 if mapping is invalid
 *         or the physical address is invalid due to raeching max addressible space.
*/
unsigned long virt2phys(struct mm_struct* mm, unsigned long vpage) {
  // Physical page address.
  unsigned long physical_page_addr;
  // Page global directory pointer.
  pgd_t* pgd;
  // 4th page directory pointer.
  p4d_t* p4d;
  // Upper page directory pointer.
  pud_t* pud;
  // Middle page directory pointer.
  pmd_t* pmd;
  // Page Table Entry pointer.
  pte_t* pte;
  // Page pointer.
  struct page* page;

  // Offset of PGD.
  pgd = pgd_offset(mm, vpage);
  // Check if PGD is empty or is valid.
  if (pgd_none(*pgd) || pgd_bad(*pgd))
    // Return 0 if not.
    return 0;
  // Offset of P4D.
  p4d = p4d_offset(pgd, vpage);
  // Check if P4D is empty or is valid.
  if (p4d_none(*p4d) || p4d_bad(*p4d))
    // Return 0 if not.
    return 0;
  // Offset of PUD.
  pud = pud_offset(p4d, vpage);
  // Check if PUD is empty or is valid.
  if (pud_none(*pud) || pud_bad(*pud))
    // Return 0 if not.
    return 0;
  // Offset of PMD.
  pmd = pmd_offset(pud, vpage);
  // Check if PMD is empty or is valid.
  if (pmd_none(*pmd) || pmd_bad(*pmd))
    // Return 0 if not.
    return 0;
  // Offset of PTE.
  pte = pte_offset_map(pmd, vpage);
  // Check if PTE is invalid.
  if (!pte)
    // Return 0 if not.
    return 0;
  // Offset of PAGE.
  page = pte_page(*pte);
  // Check if PAGE is invalid.
  if (!page)
    // Return 0 if not.
    return 0;

  physical_page_addr = page_to_phys(page);

  // we don't need the PTE anymore, so unmap it from memory
  pte_unmap(pte);

  // If the max addressible space is used which is 64 TB for a x86-64 then return 0.
  if (physical_page_addr == 70368744173568)
    return 0;
  // Return the physical address.
  return physical_page_addr;
}

/**
 * Log info about each process
 * @param void
 * @return void
*/
void print_proc_stats(void) {
  // the current process we're working with
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

      // check if mm and mmap exist
      if (thechild->mm && thechild->mm->mmap) {

        // iterate over virtual memory areas
        for (vma = thechild->mm->mmap; vma; vma = vma->vm_next) {

          // iterate over each page
          for (vpage = vma->vm_start; vpage < vma->vm_end; vpage += PAGE_SIZE) {
            unsigned long physical_page_addr = virt2phys(thechild->mm, vpage);

            // if the PPA is 0, the page must not have been allocated (mapped to)
            if (physical_page_addr != 0) {
              total_page_count++;
            }
          }
        }

        // log process stats
        printk(KERN_INFO "%d, %s, %d", pid, name, total_page_count);
      }
    }
  }
}

/**
 * Reports status info about each process.
 * @param void
 * @return 0.
*/
int proc_init(void) {
  printk(KERN_INFO "PROCESS REPORT:\n");
  printk(KERN_INFO "proc_id, proc_name, total_pages\n");
  print_proc_stats();

  return 0;
}

/**
 * Perform operations to clean up module
 * @param void
 * @return void
*/
void proc_cleanup(void) {
  // Not in sample output, so excluded.
  // printk(KERN_INFO "helloModule: performing cleanup of module\n");
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);

