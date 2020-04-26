#include <u.h>
#include <libc.h>
#include "machine.h"
#include "arcade.h"


void
main(int argc, char *argv[])
{
	int blocks = 0;
	Intcode *M = nil;

	if( !( M = malloc( sizeof(Intcode) ) ) ) {
		exits("Could not create machine.");
	}

	init(M);
	init_ascreen();
	arcade.ball = 0;
	arcade.pad = 0;
	arcade.score = 0;

	populate(M);

	M->state = RUNNING;
	M->mem[0] = 2;
	while( M->state == RUNNING ) {
		step(M);
	}

	print("High Score: %d\n",arcade.score);
	if( ascreen.view ) {
		free( ascreen.view );
	}
	exits(0);
}
