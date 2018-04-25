#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "buf.h"

void getINodeReachableBlocks(int *table, struct superblock *sb) {
	struct dinode inode = {0};
	struct buf bp = {0};
	for(int i = 1; i < sb->ninodes; i++) {
		getINode(ROOTDEV, i, &inode);
		if(inode.type) {
			printf(1,"inode %d, %d\n", i, inode.type);
			printf(1,"%d\n", inode.addrs[NDIRECT]);
			//check direct links
			for(int j = 0; j < NDIRECT; j++) {
				if(inode.addrs[j] == 0) {
					break;
				}
				table[inode.addrs[j]] = 1;
			}
			//check indirect links
			uint addr;
			if(inode.addrs[NDIRECT]) {
				printf(1,"BREADING %p\n", &bp);
				printf(1,"block %d\n", inode.addrs[NDIRECT]);
				bread(1, inode.addrs[NDIRECT], &bp);
				printf(1,"AFTER BREAD\n");
				for(int j = 0; j < NINDIRECT; j++) {
					printf(1,"   %d\n",j);
					addr = (uint)bp.data[j];
					if(addr == 0) {
						break;
					}
					table[addr] = 1;
				}
				// printf(1,"BRESLING %p\n", &bp);
				// brelse(&bp);
			}
		}
		printf(1,"END %d\n", i);
	}
}

int main(int argc, char *argv[]) {
	struct superblock sb = {0};
	getSuperBlock(1, &sb);

	int *reachableBlocks = (int *)malloc(sizeof(int) * sb.size);
	memset(reachableBlocks, 0, sizeof(reachableBlocks));
	getINodeReachableBlocks(reachableBlocks, &sb);
	exit();
}