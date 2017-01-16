//
//  MMU.c
//  
//
//  Created by Harry Su on 11/25/16.
//
//

#include <linux/syscalls.h>
#include <linux/types.h>
#include <asm/page.h>
#include <asm/linkage.h>
#include <asm/tlbflush.h>
#include <asm/uaccess.h>


/*
 * Function: get_pagetable()
 *
 * Description:
 *  Return a physical address that can be used to access the current page table.
 */
static inline unsigned char *
get_pagetable (void) {
    /* Value of the CR3 register */
    uintptr_t cr3;
    
    /* Get the page table value out of CR3 */
    __asm__ __volatile__ ("movq %%cr3, %0\n" : "=r" (cr3));
    
    /*
     * Shift the value over 12 bits.  The lower-order 12 bits of the page table
     * pointer are assumed to be zero, and so they are reserved or used by the
     * hardware.
     */
    return (unsigned char *)((((uintptr_t)cr3) & 0x000ffffffffff000u));
}

/*
 * Function: readPML4E(unsigned long vaddr, unsigned char *cr3)
 *
 * Description:
 *  Return Page Map Level 4
 */
unsigned long* readPML4E(unsigned long vaddr, unsigned char *cr3){
    /* linear address 47:39 */
    unsigned long pml4 = vaddr&0x0000ff8000000000u;
    /* physical address of pmle4
     * Bits 51:12 are from CR3
     * Bits 11:3 are bits 47:39 of the linear address
     * Bits 2:0 are all 0
     */
    return (unsigned long *)((pml4>>36)|(uintptr_t)cr3);
}

/*
 * Function: readPDPTE(unsigned long vaddr, unsigned long pml4e)
 *
 * Description:
 *  Return Page Directory Pointer Table
 */
unsigned long* readPDPTE(unsigned long vaddr, unsigned long pml4e){
    /* physical address of pdpte
     * Bits 51:12 are from the PMLE4
     * Bits 11:3 are bits 38:30 of the linear address
     * Bits 2:0 are all 0
     */
    return (unsigned long *)(((vaddr&0x0000007fc0000000u)>>27) | (pml4e&0x000ffffffffff000u));
}

/*
 * Function: readPDE(unsigned long vaddr, unsigned long pdpte)
 *
 * Description:
 *  Return Page Directory
 */
unsigned long* readPDE(unsigned long vaddr, unsigned long pdpte){
    /* physical address of pde
     * Bits 51:12 are from the PDPTE
     * Bits 11:3 are bits 29:21 of the linear address
     * Bits 2:0 are all 0
     */
    return (unsigned long *)(((vaddr&0x000000003fe00000u)>>18) | (pdpte&0x000ffffffffff000u));
}

/*
 * Function: readPTE(unsigned long vaddr, unsigned long pde)
 *
 * Description:
 *  Return Page Table
 */
unsigned long* readPTE(unsigned long vaddr, unsigned long pde){
    /* physical address of pte
     * Bits 51:12 are from the PDE
     * Bits 11:3 are bits 20:12 of the linear address
     * Bits 2:0 are all 0
     */
    return (unsigned long *)(((vaddr&0x00000000001ff000u)>>9) | (pde&0x000ffffffffff000u));
}

/*
 * Function: sys_readMMU(unsigned long vaddr, unsigned long* pte_u)
 *
 * Description:
 *  Store the value of the page table entry for virtual address vaddr into the memory pointed to by pte. 
 *  The system call should return 0 on success and -1 on error.
 */
asmlinkage long sys_readMMU(unsigned long vaddr, unsigned long* pte_u){
    
    unsigned char *cr3 = get_pagetable();
    unsigned long *_pml4e,*_pdpte, *_pde, *_pte;
    unsigned long pml4e, pdpte, pde, pte;
    
    _pml4e = readPML4E(vaddr,cr3);
    /* pml4 entries */
    pml4e = *((unsigned long *)(__va(_pml4e)));
    
    if((pml4e&0x0000000000000001u) == 0x0000000000000000u){
        printk("pml4e not present\n");
        return -1;
    }
    
    
    _pdpte = readPDPTE(vaddr,pml4e);
     /* pdpt entries */
    pdpte = *((unsigned long *)(__va(_pdpte)));
    

    /* check PDPTE present bit */
    if((pdpte&0x0000000000000001u)==0x0000000000000000u){
        printk("pdpte not present\n");
        return -1;
    }
    
     /* check PDPTE's PS flag(bit 7) */
    if((pdpte&0x0000000000000080u)==0x0000000000000080u){
        if(copy_to_user(pte_u,&pdpte,sizeof(unsigned long))){
            printk("copy_to_user error\n");
            return -1;
        }
        return 0;
    }
    
    
    /* pd entries */
    _pde = readPDE(vaddr,pdpte);
    pde = *((unsigned long *)(__va(_pde)));
    /* check PDE present bit */
    if((pde&0x0000000000000001u)==0x0000000000000000u){
        printk("pde not present\n");
        return -1;
    }
    
    /* check PDE's PS flag(bit 7) */
    if((pde&0x0000000000000080u)==0x0000000000000080u){
        if(copy_to_user(pte_u,&pde,sizeof(unsigned long))){
            printk("copy_to_user error\n");
            return -1;
        }
        return 0;
    }
    

    
    /* pt entries */
    _pte = readPTE(vaddr,pde);
    pte = *((unsigned long *)(__va(_pte)));
    
    /* check PTE present bit */
    if((pte&0x0000000000000001u)==0x0000000000000000u){
        printk("pte not present\n");
        return -1;
    }
    
    if(copy_to_user(pte_u,&pte,sizeof(unsigned long))){
        printk("copy_to_user error\n");
        return -1;
    }
    
    return 0;
    
}
/*
 * Function: sys_writeMMU(unsigned long vaddr, unsigned long pte_u)
 *
 * Description:
 *  Store the value of the page table entry value pte into the page table entry for virtual address vaddr. 
 *  The system call should return 0 on success and -1 on error.
 */
asmlinkage long sys_writeMMU(unsigned long vaddr, unsigned long pte_u){
    
    unsigned char *cr3 = get_pagetable();
    unsigned long *_pml4e,*_pdpte, *_pde, *_pte;
    unsigned long pml4e, pdpte, pde, pte;
    
    _pml4e = readPML4E(vaddr,cr3);
    /* pml4 entries */
    pml4e = *((unsigned long *)(__va(_pml4e)));
    
    if((pml4e&0x0000000000000001u) == 0x0000000000000000u){
        printk("pml4e not present\n");
        return -1;
    }
    
    _pdpte = readPDPTE(vaddr,pml4e);
    /* pdpt entries */
    pdpte = *((unsigned long *)(__va(_pdpte)));
    
    /* check PDPTE present bit */
    if((pdpte&0x0000000000000001u)==0x0000000000000000u){
        printk("pdpte not present\n");
        return -1;
    }
    
    /* check PDPTE's PS flag(bit 7)*/
    if((pdpte&0x0000000000000080u)==0x0000000000000080u){
        *((unsigned long *)(__va(_pml4e))) = pte_u;
        flush_tlb_all();
        return 0;
    }
    /* pd entries */
    _pde = readPDE(vaddr,pdpte);
    pde = *((unsigned long *)(__va(_pde)));
    
    /* check PDE present bit */
    if((pde&0x0000000000000001u)==0x0000000000000000u){
        printk("pde not present\n");
        return -1;
    }
    
    /* check PDE's PS flag(bit 7)*/
    if((pde&0x0000000000000080u)==0x0000000000000080u){
        *((unsigned long *)(__va(_pde))) = pte_u;
        flush_tlb_all();
        return 0;
    }
    /* pt entries */
    _pte = readPTE(vaddr,pde);
    pte = *((unsigned long *)(__va(_pte)));
    
    /* check PTE present bit */
    if((pte&0x0000000000000001u)==0x0000000000000000u){
        printk("pte not present\n");
        return -1;
    }
    
    
    *((unsigned long *)(__va(_pte))) = pte_u;
    flush_tlb_all();
    return 0;
}
