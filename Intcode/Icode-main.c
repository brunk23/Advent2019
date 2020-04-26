#include <u.h>
#include <libc.h>
#include "machine.h"

void
main(int argc, char *argv[]) {
	Intcode *M = nil;

	if( !( M = malloc( sizeof(Intcode) ) ) ) {
		exits("Can't create machine.");
	}

	init(M);
	populate(M);

	M->state = RUNNING;
	while( M->state == RUNNING ) {
		step(M);
	}

	if( M->state != STOPPED || DEBUG ) {
		print_mem(M);
		print("\n\n");
		print("\n\n\tIP: %d\n\tState: %s\n\tMem[0]: %lld\n",
			M->ip, print_state(M), M->mem[0]);

		print("\tHighest Address Used: %ld\n", M->highest);
	}
	exits(0);
}
