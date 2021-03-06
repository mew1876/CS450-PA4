#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char*
fmtname(char *path)
{
	static char buf[DIRSIZ+1];
	char *p;

	// Find first character after last slash.
	for(p=path+strlen(path); p >= path && *p != '/'; p--)
		;
	p++;

	// Return blank-padded name.
	if(strlen(p) >= DIRSIZ)
		return p;
	memmove(buf, p, strlen(p));
	memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
	return buf;
}

void
dw(char *path, char *indentation)
{
	char buf[512], *p;
	char indentBuf[50];
	int fd;
	struct dirent de;
	struct stat st;

	if((fd = open(path, 0)) < 0){
		printf(2, "dw: cannot open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0){
		printf(2, "dw: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch(st.type){
	case T_FILE:
		printf(1, "%s%s %d %d %d\n", indentation, fmtname(path), st.type, st.ino, st.size);
		break;

	case T_DIR:
		if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
			printf(1, "dw: path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf+strlen(buf);
		*p++ = '/';
		while(read(fd, &de, sizeof(de)) == sizeof(de)){
			if(de.inum == 0)
				continue;
			memmove(p, de.name, DIRSIZ);
			p[DIRSIZ] = 0;
			if(stat(buf, &st) < 0){
				printf(1, "dw: cannot stat %s\n", buf);
				continue;
			}
			printf(1, "%s%s %d %d %d\n", indentation, fmtname(buf), st.type, st.ino, st.size);
			strcpy(indentBuf, indentation);
			char *i = indentBuf + strlen(indentBuf);
			*i++ = ' ';
			*i++ = '\0';
			if(st.type == 1 && strcmp(fmtname(buf), ".             ") != 0 && strcmp(fmtname(buf), "..            ") != 0) {
				dw(buf, indentBuf);
			}
		}
		break;
	}
	close(fd);
}

int
main(int argc, char *argv[])
{
	int i;

	if(argc < 2){
		dw(".", "");
		exit();
	}
	for(i=1; i<argc; i++)
		dw(argv[i], "");
	exit();
}
