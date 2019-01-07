#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

int
exec(char *path, char **argv)
{
  char *s, *last;
  int i, off;
  uint argc, sz, sp, ustack[3+MAXARG+1];
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pde_t *pgdir, *oldpgdir;

  begin_op();
  if((ip = namei(path)) == 0){
    end_op();
    return -1;
  }
  ilock(ip);
  pgdir = 0;

  // Check ELF header
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) < sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  if((pgdir = setupkvm()) == 0)
    goto bad;

  // Load program into memory.
  //sz = 0;
  sz=0;
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz)) == 0)
      goto bad;
    if(loaduvm(pgdir, (char*)ph.vaddr, ip, ph.off, ph.filesz) < 0)
      goto bad;
  }
  iunlockput(ip);
  end_op();
  ip = 0;

  sz = PGROUNDUP(sz);
  /*cprintf("In exec.c, to allocuvm sz, sz+2*PGSIZE : [%x %x]\n", sz, sz+2*PGSIZE);  
  if((sz = allocuvm(pgdir, sz, sz + 2*PGSIZE)) == 0)
          goto bad;
  clearpteu(pgdir, (char *)(sz - 2*PGSIZE));
  cprintf("In exec.c, finish allocuvm sz : [%x]\n", sz);
  sp = sz;*/
  
  uint s_top = KERNBASE - 2*PGSIZE;
  //cprintf("In exec.c, to allocuvm s_top, KERNBASE : [%x %x]\n", s_top, KERNBASE);  
  if((sp = allocuvm(pgdir, s_top, KERNBASE)) == 0)
      goto bad;
  //cprintf("In exec.c, finish allocuvm sz : [%x]\n", sz);
  //sp = sz;
  // Push argument strings, prepare rest of stack in ustack.
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
    //cprintf("In exec.c, pgdir, sp, argv, argc : [%x %x %s %d]\n",pgdir,sp,argv[argc],argc);
    if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
        goto bad;
    ustack[3+argc] = sp;
  }
  ustack[3+argc] = 0;
  ustack[0] = 0xffffffff;  // fake return PC
  ustack[1] = argc;
  ustack[2] = sp - (argc+1)*4;  // argv pointer
  //cprintf("In exec.c ustack : [%x %d %d %d]\n",ustack[0],ustack[1],ustack[2], ustack[3]);
  sp -= (3+argc+1) * 4;
  if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
    goto bad;
  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(proc->name, last, sizeof(proc->name));
  // Commit to the user image.
  oldpgdir = proc->pgdir;
  proc->s_top = s_top;
  proc->pgdir = pgdir;
  //proc->sz = KERNBASE - 2*PGSIZE;
  proc->sz = sz;
  proc->tf->eip = elf.entry;  // main
  proc->tf->esp = sp;
  //cprintf("In exec.c, old, pg, sz, eip, esp : [%x %x %x %x %x]\n", oldpgdir, proc->pgdir, proc->sz, proc->tf->eip, proc->tf->esp);
  switchuvm(proc);
  freevm(oldpgdir);
  return 0;

 bad:
  if(pgdir)
    freevm(pgdir);
  if(ip){
    iunlockput(ip);
    end_op();
  }
  return -1;
}
