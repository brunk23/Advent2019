#include <u.h>
#include <libc.h>
#include "machine.h"

int main(int argc, char *argv[]) {
	MemBlock *curr = 0;

	init();

	if( populate() == ERRFILE ) {
		print("Could not open file 'input' for reading.\n");
		return 1;
	}

	IM.state = RUNNING;
	while( IM.state == RUNNING ) {
		step();
	}

	if( IM.state != STOPPED ) {
		print_mem();
		print("\n\n");
	}

	print("IP: %d\nState: %s\nMem[0]: %lld\n",IM.ip, print_state(), IM.mem->data[0]);

	/* Free the memory tree */
	while( IM.mem ) {
		curr = IM.mem->next;
		free(IM.mem);
		IM.mem = curr;
	}		
	return 0;
}
