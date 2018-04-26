#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "buf.h"
#include "param.h"

void printDInodeInfo(struct dinode dip){
  // print nlink, size, and addrs
  printf(1,"   fs links=%d\n",dip.nlink);
  printf(1,"   size=%d\n",dip.size);
  printf(1,"   data block addresses=");
  int i;
  for (i=0;i < (sizeof(dip.addrs)/sizeof(dip.addrs[0])); i++) {
      printf(1,"%d,",dip.addrs[i]);
  }
  printf(1,"\n");
}

void printDInode(int device, int iNum){
  struct dinode dip = {0};
  getINode(device, iNum, &dip);

  // print type information
  switch(dip.type){
    case 0:
      // printf(1,"free\n");
      break;
    case T_DIR:
      printf(1,"inode#=%d\n",iNum);
      printf(1,"   type=");
      printf(1,"directory\n");
      printDInodeInfo(dip);
      printf(1,"\n");
      break;
    case T_FILE:
      printf(1,"inode#=%d\n",iNum);
      printf(1,"   type=");
      printf(1,"file\n");
      printDInodeInfo(dip);
      printf(1,"\n");
      break;
    case T_DEV: // not required for resolving directory crash issues
      // printf(1,"device\n");
      // printf(1,"   major:%d",dip.major);
      // printf(1,"   minor:%d",dip.minor);
      break;
    default:  
      printf(1,"other/error type");
      printf(1,"\n");
  }
}

int main(int argc, char *argv[]) {
  int i;
  struct superblock sb = {0};
  getSuperBlock(1, &sb);
  for(i = 0; i < sb.ninodes; i++){
    // printf(1,"%d",i);
    // printf(1,"%d",NINODE);
    printDInode(ROOTDEV,i);
  }
  exit();
}

