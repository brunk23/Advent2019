#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <draw.h>
#include <cursor.h>
#include "machine.h"
#include "arcade.h"

void init_ascreen() {
	int i;

	i = initdraw(nil, nil, "Arcade Game");
	if( i == -1 ) {
		exits("Failed to init screen.\n");
	}
	ascreen.x_min = 0;
	ascreen.x_max = 0;
	ascreen.y_min = 0;
	ascreen.y_max = 0;
	ascreen.view = NULL;
	resize_ascreen( 0, 0, 39, 20 );
}

int count_blocks() {
	int x = 0, count = 0, size = 0;

	size = (ascreen.x_max - ascreen.x_min + 1) * (ascreen.y_max - ascreen.y_min + 1);

	for( x = 0; x < size; x++ ) {
		if( ascreen.view[x] == BLOCK ) {
			count++;
		}
	}
	return count;
}

void print_ascreen() {
	int x, y, v, fact;
	char c = ' ';

	fact = ascreen.x_max - ascreen.x_min + 1;

	for( y = ascreen.y_min; y <= ascreen.y_max; y++ ) {
		print("%d\t", y);
		for( x = ascreen.x_min; x <= ascreen.x_max; x++ ) {
			v = ascreen.view[ (y - ascreen.y_min) * fact + (x - ascreen.x_min) ];
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

void resize_ascreen( int xmin, int ymin, int xmax, int ymax) {
	int *newview = NULL;
	int x, y;
	int old_x_min = ascreen.x_min, old_y_min = ascreen.y_min;
	int old_x_max = ascreen.x_max, old_y_max = ascreen.y_max;
	ulong newsize = 0;

	newsize = (xmax - xmin + 1) * (ymax - ymin + 1) * sizeof(int);

	if( !(newview = malloc( newsize ) ) ) {
		print("Unable to get ascreensize %d:%d to %d:%d\n", xmin, ymin, xmax, ymax);
		exits("Failed to get be able to reset screen");
	}

	/* Zero out the new ascreen */
	for( y = ymin; y <= ymax; y++ ) {
		for( x = xmin; x <= xmax; x++ ) {
			newview[ (y - ymin) * (xmax - xmin + 1) + (x-xmin) ] = ESPOT;
		}
	}

	if( ascreen.view ) {
		for( y = old_y_min; y <= old_y_max; y++ ) {
			for( x = old_x_min; x <= old_x_max; x++ ) {
				newview[ (y - ymin) * (xmax - xmin + 1) + (x-xmin) ] =
				ascreen.view[ (y - old_y_min) * (old_x_max-old_x_min+1) + (x - old_x_min) ];
			}
		}
		free( ascreen.view );
	}

	ascreen.x_min = xmin;
	ascreen.y_min = ymin;
	ascreen.x_max = xmax;
	ascreen.y_max = ymax;
	ascreen.view = newview;
}

int *find_loc(int x, int y) {
	int xmin, ymin, xmax, ymax, resize = 0;
	if( x < ascreen.x_min ) {
		xmin = x;
		resize = 1;
	} else {
		xmin = ascreen.x_min;
	}
	if( x > ascreen.x_max ) {
		xmax = x;
		resize = 1;
	} else {
		xmax = ascreen.x_max;
	}
	if( y < ascreen.y_min ) {
		ymin = y;
		resize = 1;
	} else {
		ymin = ascreen.y_min;
	}
	if( y > ascreen.y_max ) {
		ymax = y;
		resize = 1;
	} else {
		ymax = ascreen.y_max;
	}
	if( resize == 1 ) {
		resize_ascreen( xmin, ymin, xmax, ymax );
	}
	return &ascreen.view[ (y - ymin) * (xmax - xmin + 1) + (x - xmin) ];
}

void put_val(int x, int y, int v) {
	char *wall = "#", *block = "X", *empty = " ", *ball = "*", *hpad = "-", *d = nil;
	Rectangle r;

	switch( v ) {
		case BALL:
			d = ball;
			break;
		case HPAD:
			d = hpad;
			break;
		case ESPOT:
			d = empty;
			break;
		case BLOCK:
			d = block;
			break;
		case WALL:
			d = wall;
			break;
		default:
			break;
	}
	r.min.x = 10 * x + screen->r.min.x;
	r.min.y = 16 * y + screen->r.min.y;
	r.max.x = r.min.x + 10;
	r.max.y = r.min.y + 16;
	draw(screen, r, display->white, 0 , (Point){0,0});
	string(screen, r.min, display->black, (Point){0,0},	display->defaultfont, d);
	flushimage(display,0);
	sleep(2);
	int *loc = find_loc(x, y);
	*loc = v;
}

int get_val(int x, int y ) {
	int *loc = find_loc(x, y);
	return *loc;
}

void icm_out() {
	vlong value = 0;
	Rectangle r;

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
			if( arcade.x == -1 && arcade.y == 0) {
				r.min.x = 8 + screen->r.min.x;
				r.min.y = 21 * 16 + screen->r.min.y;
				r.max.x = screen->r.max.x;
				r.max.y = r.min.y + 20;
				draw( screen, r, display->white, 0, (Point){0,0});
				string(screen, r.min, display->black, (Point){0,0},
					display->defaultfont, smprint("Score: %d", value));
				flushimage(display,0);
			} else {
				if( value == BALL ) {
					arcade.ball = arcade.x;
				}
				if( value == HPAD ) {
					arcade.pad = arcade.x;
				}
				put_val( arcade.x, arcade.y, value );
			}
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
	if( arcade.pad > (arcade.ball) ) {
		return -1;
	}
	if( arcade.pad < (arcade.ball) ) {
		return 1;
	}
	return 0;
}
