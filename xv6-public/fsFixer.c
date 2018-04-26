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
	for(int i = 0; i < sb->ninodes; i++) { // loops over inode numbers
		getINode(ROOTDEV, i, &inode);
		if(inode.type) {
			// printf(1,"inode %d--------------------------\n", i, inode.type);
			//check direct links
			for(int j = 0; j <= NDIRECT; j++) { // loops over pointers in the block
				if(inode.addrs[j] == 0) {
					//break;
					table[inode.addrs[j]] = 0;
				}
				else{
					// printf(1, "marking1 %d\n", inode.addrs[j]);
					table[inode.addrs[j]] = 1;
				}
			}
			//check indirect links
			uint addr;
			if(inode.addrs[NDIRECT]) {
				// printf(1,"BREADING %p\n", &bp);
				// printf(1,"block %d\n", inode.addrs[NDIRECT]);
				bread(1, inode.addrs[NDIRECT], &bp);
				// printf(1,"AFTER BREAD\nReceived pointer: %p\n", bp);
				// if(bp.data) {
					for(int j = 0; j < NINDIRECT; j++) { // loop over pointers in the indirect block
						addr = ((uint *)bp.data)[j];
						// printf(1, "J: %d ADDR: %p", j , addr);
						if(addr == 0) {
							// break;
							table[addr] = 0;
						}
						else{
							// printf(1, "marking2 %d\n", addr);
							table[addr] = 1;
						}
					}
				// }
			}
		}
		// printf(1,"END %d\n", i);
	}
}

void getOrphanBlocks(int *table, int *reachable, struct superblock *sb) {
	struct buf bp = {0};
	for(int b = 0; b < sb->size; b += BPB) {
		bread(ROOTDEV, (b/BPB + sb->bmapstart), &bp);
		for(int bi = 0; bi < BPB && b + bi < sb->size; bi++) {
			int m = 1 << (bi % 8);
			printf(1,"Block: %d Bitmap: %d Reachable: %d\n", b + bi, (bp.data[bi/8] & m) != 0, reachable[b + bi]);
			if((bp−>data[bi/8] & m) != 0) {
				
			}
		}
	}
}

int main(int argc, char *argv[]) {
	struct superblock sb = {0};
	getSuperBlock(1, &sb);

	int *reachableBlocks = (int *)malloc(sizeof(int) * sb.size);
	memset(reachableBlocks, 0, sizeof(reachableBlocks));
	getINodeReachableBlocks(reachableBlocks, &sb);

	int *orphanBlocks = (int *)malloc(sizeof(int) * sb.size);
	memset(orphanBlocks, 0, sizeof(reachableBlocks));
	getOrphanBlocks(orphanBlocks, reachableBlocks, &sb);

	// for(int i = 1; i < sb.size; i++) {
	// 	printf(1,"Block: %d Reachable: %d\n", i, reachableBlocks[i]);
	// }


	exit();
}