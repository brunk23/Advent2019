#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "machine.h"
#include "arcade.h"

void init_screen() {
	screen.x_min = 0;
	screen.x_max = 0;
	screen.y_min = 0;
	screen.y_max = 0;
	screen.view = NULL;
	resize_screen( 0, 0, 10, 10 );
}

int count_blocks() {
	int x = 0, count = 0, size = 0;

	size = (screen.x_max - screen.x_min + 1) * (screen.y_max - screen.y_min + 1);

	for( x = 0; x < size; x++ ) {
		if( screen.view[x] == BLOCK ) {
			count++;
		}
	}
	return count;
}

void print_screen() {
	int x, y, v, fact;
	char c = ' ';

	fact = screen.x_max - screen.x_min + 1;

	for( y = screen.y_min; y <= screen.y_max; y++ ) {
		print("%d\t", y);
		for( x = screen.x_min; x <= screen.x_max; x++ ) {
			v = screen.view[ (y - screen.y_min) * fact + (x - screen.x_min) ];
			switch( v ) {
				case ESPOT:
					c = ' ';
					break;
				case WALL:
					c = '#';
					break;
				case BLOCK:
					c = 'X';
					break;
				case HPAD:
					c = '_';
					break;
				case BALL:
					c = '*';
					break;
				default:
					c = '?';
					break;
			}
			print("%c", c);
		}
		print("\n");
	}
}

void resize_screen( int xmin, int ymin, int xmax, int ymax) {
	int *newview = NULL;
	int x, y;
	int old_x_min = screen.x_min, old_y_min = screen.y_min;
	int old_x_max = screen.x_max, old_y_max = screen.y_max;
	ulong newsize = 0;

	newsize = (xmax - xmin + 1) * (ymax - ymin + 1) * sizeof(int);

	if( !(newview = malloc( newsize ) ) ) {
		print("Unable to get screensize %d:%d to %d:%d\n", xmin, ymin, xmax, ymax);
		abort();
	}

	/* Zero out the new screen */
	for( y = ymin; y <= ymax; y++ ) {
		for( x = xmin; x <= xmax; x++ ) {
			newview[ (y - ymin) * (xmax - xmin + 1) + (x-xmin) ] = ESPOT;
		}
	}

	if( screen.view ) {
		for( y = old_y_min; y <= old_y_max; y++ ) {
			for( x = old_x_min; x <= old_x_max; x++ ) {
				newview[ (y - ymin) * (xmax - xmin + 1) + (x-xmin) ] =
				screen.view[ (y - old_y_min) * (old_x_max-old_x_min+1) + (x - old_x_min) ];
			}
		}
		free( screen.view );
	}

	screen.x_min = xmin;
	screen.y_min = ymin;
	screen.x_max = xmax;
	screen.y_max = ymax;
	screen.view = newview;
}

int *find_loc(int x, int y) {
	int xmin, ymin, xmax, ymax, resize = 0;
	if( x < screen.x_min ) {
		xmin = x;
		resize = 1;
	} else {
		xmin = screen.x_min;
	}
	if( x > screen.x_max ) {
		xmax = x;
		resize = 1;
	} else {
		xmax = screen.x_max;
	}
	if( y < screen.y_min ) {
		ymin = y;
		resize = 1;
	} else {
		ymin = screen.y_min;
	}
	if( y > screen.y_max ) {
		ymax = y;
		resize = 1;
	} else {
		ymax = screen.y_max;
	}
	if( resize == 1 ) {
		resize_screen( xmin, ymin, xmax, ymax );
	}
	return &screen.view[ (y - ymin) * (xmax - xmin + 1) + (x - xmin) ];
}

void put_val(int x, int y, int v) {
	int *loc = find_loc(x, y);
	*loc = v;
}

int get_val(int x, int y ) {
	int *loc = find_loc(x, y);
	return *loc;
}

void icm_out() {
	vlong value = 0;

	value = readmem( IM.ip + 1 );
	RUNNINGP;

	switch( arcade.state ) {
		case WX:
			arcade.x = value;
			arcade.state = WY;
			break;
		case WY:
			arcade.y = value;
			arcade.state = WV;
			break;
		case WV:
			put_val( arcade.x, arcade.y, value );
			arcade.state = WX;
			break;
		default:
			print("Bad state\n");
	}

	IM.ip += 2;
}

void icm_in() {
	writemem( IM.ip + 1, arcade_read() );
	RUNNINGP;

	IM.ip += 2;
}

vlong arcade_read() {
	return 0;
}
