#include <u.h>
#include <libc.h>
#include "machine.h"

void step(void) {
	int value;

	if( valid( IM.ip ) ) {
		value = IM.mem[ IM.ip ];
	} else {
		IM.state = ERRMEM;
		return;
	}

	/* Split the instruction and mode flags */
	IM.inst = value%100;
	IM.mode = value/100;

	switch( IM.inst ) {
		case ADD:
			icm_add();
			break;
		case MULT:
			icm_mult();
			break;
		case HALT:
			icm_halt();
			break;
		default:
			IM.state = ERROP;
			break;
	}
}

void icm_halt() {
	IM.ip++;
	IM.state = STOPPED;
}

void icm_add() {
	long long noun = 0, verb = 0;

	noun = readmem( IM.ip + 1 );
	if( IM.state != RUNNING ) {
		return;
	}

	verb = readmem( IM.ip + 2 );
	if( IM.state != RUNNING ) {
		return;
	}

	noun += verb;

	writemem( IM.ip + 3, noun);
	if( IM.state != RUNNING ) {
		return;
	}

	IM.ip += 4;
}

void icm_mult() {
	long long noun = 0, verb = 0;

	noun = readmem( IM.ip + 1 );
	if( IM.state != RUNNING ) {
		return;
	}

	verb = readmem( IM.ip + 2 );
	if( IM.state != RUNNING ) {
		return;
	}

	noun *= verb;

	writemem( IM.ip + 3, noun);
	if( IM.state != RUNNING ) {
		return;
	}

	IM.ip += 4;
}

/*
 * Reads memory from addr using correct mode.
 * Returns: value
 * Warning: changes IM.mode
 */
long long readmem(int addr) {
	long long value = 0;

	switch( IM.mode%10 ) {
		case POSITION:
			if( valid(addr) ) {
				if( valid( IM.mem[ addr ] ) ) {
					value = IM.mem [ IM.mem[ addr ] ];
				} else {
					IM.state = ERRMEM;
				}
			} else {
				IM.state = ERRMEM;
			}
			break;
		case IMMEDIATE:
			if( valid(addr) ) {
				value = IM.mem[ addr ];
			} else {
				IM.state = ERRMEM;
			}
			break;
		default:
			IM.state = ERRMODE;
			break;
	}
	IM.mode /= 10;

	return value;
}

void writemem( int addr, long long value ) {
	switch( IM.mode%10 ) {
		case POSITION:
			if( valid( addr ) ) {
				if( valid( IM.mem[ addr ] ) ) {
					IM.mem[ IM.mem[ addr ] ] = value;
				} else {
					IM.state = ERRMEM;
				}
			} else {
				IM.state = ERRMEM;
			}
			break;
		case IMMEDIATE:   /* Fall through, immediate mode invalid */
		default:
			IM.state = ERRMODE;
	}
	IM.mode /= 10;
}
