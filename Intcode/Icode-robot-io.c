#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "machine.h"
#include "robot.h"

void
icm_out(Intcode *M) {
	vlong value = 0;

	value = readmem( M, M->ip + 1 );
	RUNNINGP;

	switch( robot.state ) {
		case PAINT:
			if( value == BLACK ) {
				robot.loc->val = BLACK;
			} else {
				robot.loc->val = WHITE;
				robot.loc->paint = 1;
			}
			robot.state = DIRECT;
			break;
		case DIRECT:
			if( value == RIGHT ) {
				robot.direction++;
				robot.direction %= 4;
			} else {
				if( robot.direction == UP ) {
					robot.direction = LEFT;
				} else {
					robot.direction--;
				}
			}
			robot_move();
			robot.state = PAINT;
			break;
		default:
			print("Bad state\n");
	}

	M->ip += 2;
}

void
icm_in(Intcode *M) {
	writemem( M, M->ip + 1, robot_read() );
	RUNNINGP;

	M->ip += 2;
}

vlong robot_read() {
	return robot.val;
}

void robot_move(void) {
	long i;
	switch( robot.direction ) {
		case UP:
			robot.y--;
			if( robot.y < robot.miny ) {
				robot.miny = robot.y;
			}
			break;
		case DOWN:
			robot.y++;
			if( robot.y > robot.maxy ) {
				robot.maxy = robot.y;
			}
			break;
		case LEFT:
			robot.x--;
			if( robot.x < robot.minx ) {
				robot.minx = robot.x;
			}
			break;
		case RIGHT:
			robot.x++;
			if( robot.x > robot.maxx ) {
				robot.maxx = robot.x;
			}
			break;
		default:
			print("Bad direction\n");
	}
	for( i = 0; i < HULLSIZE; i++ ) {
		if( hull[i].val == NEWSPOT ) {
			hull[i].x = robot.x;
			hull[i].y = robot.y;
			hull[i].val = BLACK;
			robot.val = BLACK;
			robot.loc = &hull[i];
			return;
		}
		if( hull[i].x == robot.x && hull[i].y == robot.y ) {
			robot.val = hull[i].val;
			robot.loc = &hull[i];
			return;
		}
	}
	print("Ran out of room on the hull!\n");
}
