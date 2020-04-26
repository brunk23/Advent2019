#include <u.h>
#include <libc.h>
#include "machine.h"
#include "robot.h"

int main(int argc, char *argv[]) {
	long max = 0, len = 0, x, y;
	Intcode *M = nil;

	if( !( M = malloc( sizeof(Intcode) ) ) ) {
		exits("Could not create machine.");
	}

	robot.direction = UP;
	robot.state = PAINT;
	robot.x = 0;
	robot.y = 0;
	robot.minx = 0;
	robot.maxx = 0;
	robot.miny = 0;
	robot.maxy = 0;
	robot.val = WHITE;
	robot.loc = &hull[0];

	for( len = 0; len < HULLSIZE; len++ ) {
		hull[len].x = 0;
		hull[len].y = 0;
		hull[len].paint = 0;
		hull[len].val = NEWSPOT;
	}

	hull[0].val = WHITE;

	init(M);
	populate(M);

	M->state = RUNNING;
	while( M->state == RUNNING ) {
		step(M);
	}

	if( M->state != STOPPED ) {
		print_mem(M);
		print("\n\n");
	}

	if( DEBUG ) {
		print("Min x: %d\nMax x: %d\n", robot.minx, robot.maxx);
		print("Min y: %d\nMax y: %d\n", robot.miny, robot.maxy);
	}

	max = 0;
	for( len = 0; len < HULLSIZE; len++ ) {
		if( hull[len].val == NEWSPOT ) {
			break;
		}
		if( hull[len].paint == 1 ) {
			max++;
		}
	}
	print("Painted at least %d spots\n", max);

	for( y = robot.miny; y <= robot.maxy; y++ ) {
		for( x = robot.minx; x <= robot.maxx; x++ ) {
			for( len = 0; len < HULLSIZE; len++ ) {
				if( hull[len].val == NEWSPOT ) {
					print(" ");
					break;
				}
				if( hull[len].x == x && hull[len].y == y ) {
					if( hull[len].val == BLACK ) {
						print(" ");
					} else {
						print("@");
					}
					break;
				}
			}
		}
		print("\n");
	}

	if( DEBUG ) {
		print("\n\n\tIP: %d\n\tState: %s\n\tMem[0]: %lld\n",
			M->ip, print_state(M), M->mem[0]);

		print("\tMax Address: %ld\n", M->highest);
	}
	return 0;
}
