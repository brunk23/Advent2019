#include <u.h>
#include <libc.h>
#include <stdio.h>
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
		case IN:
			icm_in();
			break;
		case OUT:
			icm_out();
			break;
		case LT:
			icm_lt();
			break;
		case EQ:
			icm_eq();
			break;
		case JT:
			icm_jt();
			break;
		case JF:
			icm_jf();
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

void icm_jt() {
	long long value = 0;

	value = readmem( IM.ip + 1 );
	RUNNINGP;

	if( value != 0 ) {
		value = readmem( IM.ip + 2 );
		RUNNINGP;
		if( valid(value) ) {
			IM.ip = value;
			return;
		} else {
			IM.state = ERRMEM;
			return;
		}
	}
	IM.ip += 3;
}

void icm_jf() {
	long long value = 0;

	value = readmem( IM.ip + 1 );
	RUNNINGP;

	if( value == 0 ) {
		value = readmem( IM.ip + 2 );
		RUNNINGP;
		if( valid(value) ) {
			IM.ip = value;
			return;
		} else {
			IM.state = ERRMEM;
			return;
		}
	}
	IM.ip += 3;
}

void icm_lt() {
	long long noun = 0, verb = 0;

	noun = readmem( IM.ip + 1 );
	RUNNINGP;

	verb = readmem( IM.ip + 2 );
	RUNNINGP;

	if( noun < verb ) {
		writemem( IM.ip + 3, 1 );
	} else {
		writemem( IM.ip + 3, 0 );
	}
	RUNNINGP;

	IM.ip += 4;
}

void icm_eq() {
	long long noun = 0, verb = 0;

	noun = readmem( IM.ip + 1 );
	RUNNINGP;

	verb = readmem( IM.ip + 2 );
	RUNNINGP;

	if( noun == verb ) {
		writemem( IM.ip + 3, 1 );
	} else {
		writemem( IM.ip + 3, 0 );
	}
	RUNNINGP;

	IM.ip += 4;
}

void icm_out() {
	long long value = 0;

	value = readmem( IM.ip + 1 );
	RUNNINGP;

	print("%d\n", value);

	IM.ip += 2;
}

void icm_in() {
	long long value = 0;

	if( EOF == scanf("%d", &value) ) {
		IM.state = ERRIN;
		return;
	}

	writemem( IM.ip + 1, value );
	RUNNINGP;

	IM.ip += 2;
}

void icm_add() {
	long long noun = 0, verb = 0;

	noun = readmem( IM.ip + 1 );
	RUNNINGP;

	verb = readmem( IM.ip + 2 );
	RUNNINGP;

	noun += verb;

	writemem( IM.ip + 3, noun);
	RUNNINGP;

	IM.ip += 4;
}

void icm_mult() {
	long long noun = 0, verb = 0;

	noun = readmem( IM.ip + 1 );
	RUNNINGP;

	verb = readmem( IM.ip + 2 );
	RUNNINGP;

	noun *= verb;

	writemem( IM.ip + 3, noun);
	RUNNINGP;

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
