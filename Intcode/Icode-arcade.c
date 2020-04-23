#include <u.h>
#include <libc.h>
#include "machine.h"
#include "arcade.h"


int main(int argc, char *argv[]) {
	int blocks = 0;
	init();
	init_screen();

	if( populate() == ERRFILE ) {
		print("Could not open file 'input' for reading.\n");
		return 1;
	}

	IM.state = RUNNING;
	while( IM.state == RUNNING ) {
		step();
	}

	print_screen();
	blocks = count_blocks();

	print("There were %d blocks on the screen.\n", blocks);

	if( screen.view ) {
		free( screen.view );
	}
	return 0;
}
