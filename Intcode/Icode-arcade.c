#include <u.h>
#include <libc.h>
#include "machine.h"
#include "arcade.h"


void
main(int argc, char *argv[])
{
	int blocks = 0;
	init();
	init_ascreen();
	arcade.ball = 0;
	arcade.pad = 0;

	if( populate() == ERRFILE ) {
		exits("Could not open file 'input' for reading.\n");
	}

	IM.state = RUNNING;
	IM.mem[0][1] = 2;
	while( IM.state == RUNNING ) {
		step();
	}

	if( ascreen.view ) {
		free( ascreen.view );
	}
	exits(0);
}
