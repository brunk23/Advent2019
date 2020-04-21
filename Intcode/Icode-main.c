#include <u.h>
#include <libc.h>
#include "machine.h"

int main(int argc, char *argv[]) {
	init();

	if( populate() == ERRFILE ) {
		print("Could not open file 'input' for reading.\n");
		return 1;
	}

	IM.state = RUNNING;
	while( IM.state == RUNNING ) {
		step();
	}

	print("IP: %d\nState: %s\nMem[0]: %d\n",IM.ip, print_state(), IM.mem[0]);
	return 0;
}
