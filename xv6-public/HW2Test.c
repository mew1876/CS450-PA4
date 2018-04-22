#include "types.h"
#include "user.h"
#include "syscall.h"

int main(void) {
	printf(1, "Hello from xv6!");
	write(1, "This was written with the write() syscall\n", 45);

	printf(1, "Write something and hit enter!\n");
	char buf1[100];
	read(1, buf1, 50);
	printf(1, "Read in: %s\n", buf1);

	char buf2[100];
	strcpy(buf2, "This was put into the buffer with strcpy()\n");
	printf(1, "Copied into buffer: %s\n", buf2);

	if(fork() == 0) {
		printf(1, "Child process used write() %d times.\n", getCallCount(SYS_write));
	}
	else {
		wait();
		printf(1, "Parent process used write() %d times.\n", getCallCount(SYS_write));
		printf(1, "Parent process used write() %d times.\n", getCallCount(SYS_wait));
	}
	exit();
}