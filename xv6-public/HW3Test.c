#include "types.h"
#include "user.h"

int main(void) {
	int size = 500000;
	int bufSize = 800;
	char *buf;
	char *bufs [bufSize];
	int i = 0;

	printf(1,"start\n");
	myMemory();

	printf(1,"single small int malloc\n");
	buf = (char *) malloc(sizeof(int));
	bufs[i] = buf;
	i++;
	myMemory();

	printf(1,"10 int mallocs\n");
	int j;
	for(j=0; j < 10; j++){
		buf = (char *) malloc(sizeof(int));
		bufs[i] = buf;
		i++;
	}
	myMemory();


	printf(1,"10 char mallocs\n");
	int k;
	for(k=0; k < 10; k++){
		buf = (char *) malloc(sizeof(char));
		bufs[i] = buf;
		i++;
	}
	myMemory();

	printf(1,"10 float mallocs\n");
	int l;
	for(l=0; l < 10; l++){
		buf = (char *) malloc(sizeof(float));
		bufs[i] = buf;
		i++;
	}
	myMemory();

	// i is some value already, so it skips over small malloc spots in bufs!
	printf(1,"malloc char*[%d] until vm runs out of space\n",size);
	while( ((buf) = (char *)malloc(size))){
		buf[0] = size;
		bufs[i] = buf;
		i++;
		if(i >= bufSize){
			printf(1,"ran out of bufs room");
		}
	}
	myMemory();

	printf(1,"free everything\n");
	int z;
	for(z = 0; z<i; z++){
		free(bufs[z]);
	}
	// printf(1,"z=%d",z);
	myMemory();
	exit();
}