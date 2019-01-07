#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;
uint req;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);
  
  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(proc->killed)
      exit();
    proc->tf = tf;
    syscall();
    if(proc->killed)
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpu->id == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpu->id, tf->cs, tf->eip);
    lapiceoi();
    break;   
    //stack backward
  case T_PGFLT:
    //cprintf("[[[%x %x]]]\n",  proc->s_top, rcr2());
    if((rcr2() < proc->s_top) && rcr2() > (proc->s_top - PGSIZE) && rcr2() < KERNBASE) {
    //if((rcr2() < proc->s_top && rcr2() < KERNBASE)) {
        if(allocuvm(proc->pgdir, proc->s_top-PGSIZE, proc->s_top) == 0) {
            proc->killed = 1;
            break;
        }
        else {
            proc->s_top = proc->s_top - PGSIZE;
            break;
        }
    }
    /*if(proc->tf->esp <= proc->s_top) {
        req = ((proc->s_top - proc->tf->esp)/PGSIZE) +1;
        cprintf("필요 갯수 : %d\n",req);
        for(i=0;i<req;i++) {
            allocuvm(proc->pgdir, proc->s_top-PGSIZE, proc->s_top);
            proc->s_top = proc->s_top - PGSIZE;
        }
    }
    else {
        cprintf("aa : proc->s_top, proc->tf->esp, ebp rcr : [%x %x %x %x]\n", proc->s_top, proc->tf->esp, proc->tf->ebp, rcr2());
        proc->killed = 1;
    }
    break;*/
    /*
    cprintf("proc->s_top, proc->tf->esp, ebp rcr : [%x %x %x %x]\n", proc->s_top, proc->tf->esp, proc->tf->ebp, rcr2());
    cprintf("proc->s_top 기준 : %x %x %x\n", proc->s_top-proc->tf->esp, proc->s_top-proc->tf->ebp, proc->s_top-rcr2());
    cprintf("proc->tf->esp 기준 : %x %x %x\n", proc->tf->esp-proc->s_top, proc->tf->esp-proc->tf->ebp, proc->tf->esp-rcr2());
    cprintf("proc->tf->ebp 기준 : %x %x %x\n", proc->tf->ebp-proc->s_top, proc->tf->ebp-proc->tf->esp, proc->tf->ebp-rcr2());
    cprintf("rcr2() 기준 : %x %x %x\n", rcr2()-proc->s_top, rcr2()-proc->tf->esp, rcr2()-proc->tf->ebp);
    cprintf("하하하하하하하\n");
    proc->killed = 1;
    break;
    */
    //PAGEBREAK: 13
  default:
    if(proc == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpu->id, tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            proc->pid, proc->name, tf->trapno, tf->err, cpu->id, tf->eip, 
            rcr2());
    proc->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running 
  // until it gets to the regular system call return.)
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(proc && proc->state == RUNNING && tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit();
}
