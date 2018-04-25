#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "param.h"

// // made a syscall for permissions
// void dirInodeEraser(int dev, int type){
// 	int inum;
// 	struct buf *bp;
// 	struct dinode *dip;

// 	bp = bread(dev, IBLOCK(inum, sb));
// 	dip = (struct dinode*)bp−>data + inum%IPB;
// 	if(dip->type != T_DIR){
// 		panic("attempted to erase inode not a directory");
// 	}
// 	memset(dip, 0, sizeof(*dip));
// 	brelse(bp);
// }

int
main(int argc, char *argv[])
{
  dirErase(ROOTDEV,atoi(argv[1])); // I don't know if this is right
  exit();
}
