#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "buf.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

int sys_getCallCount(void) {
  struct proc *curproc = myproc();
  int sysCallNum;
  if(argint(0, &sysCallNum) < 0)
    return -1;
  if(sysCallNum > 0 && sysCallNum < 32) {
    return curproc->sysCallCounts[sysCallNum];
  }
  return -1;
}

int sys_myMemory(void) {
  pde_t* pageDirectory = myproc()->pgdir;
  int alloced = 0, accessible = 0, writable = 0;
  for(int i = 0; i < NPDENTRIES; i++) {
    if(pageDirectory[i] & PTE_P) {
      pte_t* pageTable = (pte_t*)(PTE_ADDR(pageDirectory[i]) + KERNBASE);
      for(int j = 0; j < NPTENTRIES; j++) {
        pte_t pte = pageTable[j];
        if(pte & PTE_P) {
          alloced++;
          if(pte & PTE_U) {
            accessible++;
            if(pte & PTE_W)
              writable++;
          }  
        }
      }
    }
  }
  cprintf("Current page usage: %d alloced, %d accessible, %d writable\n", alloced, accessible, writable);
  return 0;
}

int sys_getINode(void) {
  int device, iNum;
  struct dinode *inode;
  struct superblock sb;
  cprintf("Start\n");
  if(argint(0, &device) < 0)
    return -1;
  if(argint(1, &iNum) < 0)
    return -1;
  if(argptr(2, (char **)&inode, sizeof(struct dinode)) < 0)
    return -1;
  cprintf("Middle\n");
  readsb(device, &sb);

  struct buf *bp = bread(device, IBLOCK(iNum, sb));
  // *inode = *((struct dinode*)bp->data + iNum%IPB);
  memmove(inode, (struct dinode*)bp->data + iNum%IPB, sizeof(struct dinode));
  cprintf("End\n");
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
