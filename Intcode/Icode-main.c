#include <u.h>
#include <libc.h>
#include "machine.h"

void main(int argc, char *argv[]) {
	init();

	populate();

	IM.state = RUNNING;
	while( IM.state == RUNNING ) {
		step();
	}

	if( IM.state != STOPPED || DEBUG ) {
		print_mem();
		print("\n\n");
		print("\n\n\tIP: %d\n\tState: %s\n\tMem[0]: %lld\n",
			IM.ip, print_state(), IM.mem[0]);

		print("\tHighest Address Used: %ld\n", IM.highest);
	}
	exits(0);
}
