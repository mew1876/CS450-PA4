#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "buf.h"

int main(int argc, char *argv[]) {
	struct dinode inode = {0};
  struct superblock sb;
  getSuperBlock(1, &sb);
	for(int i = 1; i < sb.ninodes; i++) {
    getINode(1, i, &inode);
    if(inode.type) {
      printf(1, "Inode %d is allocated\n", i);
    }
  }
  exit();
}

