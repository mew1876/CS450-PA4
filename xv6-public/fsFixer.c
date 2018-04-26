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
	struct buf blockbuf = {0};
	for(int i = 1; i < sb->ninodes; i++) { // loops over inode numbers
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
				// printf(1,"BREADING %p\n", &blockbuf);
				// printf(1,"block %d\n", inode.addrs[NDIRECT]);
				bread(ROOTDEV, inode.addrs[NDIRECT], &blockbuf);
				// printf(1,"AFTER BREAD\nReceived pointer: %p\n", blockbuf);
				// if(blockbuf.data) {
					for(int j = 0; j < NINDIRECT; j++) { // loop over pointers in the indirect block
						addr = ((uint *)blockbuf.data)[j];
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

void recoverLostBlock(int device, int blockNo, struct superblock sb){
	struct dinode lostInode = {0};
	lostInode.type = T_DIR;
	struct dirent *lostDEDot = 0;
	struct dirent *lostDEDotDot = 0;
	struct buf blockbuf = {0};
	
	// recover first block of lost inode
	bread(ROOTDEV,blockNo,&blockbuf);
	lostDEDot = (struct dirent *)(&(blockbuf.data));
	lostDEDotDot =(struct dirent*)(&(blockbuf.data))+1;
	if(lostDEDot<0){
		printf(1,"\".\" dirent * error");
	}
	if(lostDEDotDot<0){
		printf(1,"\"..\" dirent * error");
	}
	// int parentINum = lostDEDotDot->inum;
	int lostINum = lostDEDot->inum;
	if(strcmp(".",lostDEDot->name)) {
		printf(1,"dirent wasn't \".\"");
	}
	lostInode.addrs[0] = blockNo;

	uint size = 0;
	short nlink = -2; // to discount . and ..
	// printf(1,"%p%d%d",&lostInode,parentINum,lostINum);
	for(int i = 0; i < BSIZE/sizeof(struct dirent); i++){ // loop over dirent entries
		// add to size
		size += sizeof(struct dirent);
		// TODO: check that the inode in dirents actually refer to the current block
			// THEN add to nlink
		nlink++;
	}
	lostInode.size = size;
	lostInode.nlink = nlink;
	lostInode.major = 0;
	lostInode.minor = 0;
	
	// write lostInode to disk
	struct buf newbuf = {0};
	bread(ROOTDEV,IBLOCK(lostINum,sb),&newbuf);
	struct dinode *dip = (struct dinode *)newbuf.data + lostINum%IPB;
	dip->type = lostInode.type;
	dip->major = lostInode.major;
	dip->minor = lostInode.minor;
	dip->nlink = lostInode.nlink;
	dip->size = lostInode.size;
	memmove(dip->addrs, lostInode.addrs, sizeof(lostInode.addrs));
	bwrite(&newbuf);
}

void getOrphanBlocks(int *table, int *reachable, struct superblock *sb) {
	struct buf blockbuf = {0};
	for(int b = 0; b < sb->size; b += BPB) {
		bread(ROOTDEV, (b/BPB + sb->bmapstart), &blockbuf);
		for(int bi = 0; bi < BPB && b + bi < sb->size; bi++) {
			int m = 1 << (bi % 8);
			// printf(1,"Block: %d Bitmap: %d Reachable: %d\n", b + bi, (blockbuf.data[bi/8] & m) != 0, reachable[b + bi]);
			if((blockbuf.data[bi/8] & m) != 0) {
				if(!reachable[b + bi] && b + bi > 58) {
					table[b + bi] = 1;
					// printf(1,"Error with block %d\n", b + bi);
				}
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

	for(int i = 1; i < sb.size; i++) {
		// printf(1,"i: %d, o: %d\n", i, orphanBlocks[i]);
		if(orphanBlocks[i]) {
			printf(1,"Error with block %d\n", i);
			recoverLostBlock(ROOTDEV, i, sb);
		}
	}

	free(reachableBlocks);
	free(orphanBlocks);
	exit();
}

// mkdir foo; mkdir foo/bar; directoryEraser 27; fsFixer;