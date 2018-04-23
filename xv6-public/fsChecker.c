#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "param.h"

void getAllocedINodes(int *table, int ninodes) {
	struct dinode inode = {0};
	for(int i = 1; i < ninodes; i++) {
		getINode(ROOTDEV, i, &inode);
		if(inode.type)
			table[i] = 1;
	}
}

void walkTree(int* table, char *path) {
	int fd;
	struct stat st = {0};
	struct dirent de = {0};

	if((fd = open(path, 0)) < 0) {
		printf(1,"failed to open %s\n", path);
		return;
	}
	if(fstat(fd, &st) < 0){
		close(fd);
		printf(1,"failed to stat %s\n", path);
		return;
	}

	table[st.ino] = 1;
	if(st.type == T_DIR) {
		char buf[256];
		int pathLength = strlen(path);
		memmove(buf, path, pathLength);
		buf[pathLength] = '/';

		while(read(fd, &de, sizeof(de)) == sizeof(de)) {
			if(de.inum == 0)
				continue;
			if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
				continue;
			memmove(buf + pathLength + 1, de.name, strlen(de.name));
			walkTree(table, buf);
			memset(buf + pathLength + 1, '\0', 256 - pathLength - 1);
		}
	}
	close(fd);
}

void getReachableINodes(int *table, int ninodes) {
	walkTree(table, "");
}

int main(int argc, char *argv[]) {
	struct superblock sb = {0};
	getSuperBlock(1, &sb);

	int *allocedINodes = (int *)malloc(sizeof(int) * sb.ninodes);
	getAllocedINodes(allocedINodes, sb.ninodes);
	int *reachableINodes = (int *)malloc(sizeof(int) * sb.ninodes);
	getReachableINodes(reachableINodes, sb.ninodes);

	for(int i = 1; i < sb.ninodes; i++) {
		if(allocedINodes[i] != reachableINodes[i]) {
			printf(1, "Error detected: ");
			printf(1, "i: %d   a: %d   r:%d\n", i, allocedINodes[i], reachableINodes[i]);
		}
	}
	exit();
}