#include <u.h>
#include <libc.h>
#include "machine.h"

int main(int argc, char *argv[]) {
	long len = 0, max = 0;
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

	if( DEBUG ) {
		print("\n\n\tIP: %d\n\tState: %s\n\tMem[0]: %lld\n",
			IM.ip, print_state(), IM.mem[0][0]);

		for(len = 0; len < WSIZE; len++ ) {
			if( IM.mem[len][0] == EMPTY ) {
				break;
			}
			if( IM.mem[len][0] > max ) {
				max = IM.mem[len][0];
			}
		}
		print("\tAddresses Used: %ld\n\tMax Address: %ld\n", len, max);
	}
	return 0;
}
