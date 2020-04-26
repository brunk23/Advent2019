#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "machine.h"

void step(void) {
	vlong value, *curr;

	if( valid( IM.ip ) ) {
		value = IM.mem[ IM.ip ];
	} else {
		print("\nError setting IM.ip to %ld\n", IM.ip);
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
		case ADJRB:
			icm_adjrb();
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

void icm_adjrb() {
	long value = 0;

	value = readmem( IM.ip + 1 );
	RUNNINGP;

	IM.base += value;
	IM.ip += 2;
}

void icm_jt() {
	vlong value = 0;

	value = readmem( IM.ip + 1 );
	RUNNINGP;

	if( value != 0 ) {
		value = readmem( IM.ip + 2 );
		RUNNINGP;
		if( valid(value) ) {
			IM.ip = value;
			return;
		} else {
			print("\nError setting IM.ip to %ld\n", value);
			IM.state = ERRMEM;
			return;
		}
	}
	IM.ip += 3;
}

void icm_jf() {
	vlong value = 0;

	value = readmem( IM.ip + 1 );
	RUNNINGP;

	if( value == 0 ) {
		value = readmem( IM.ip + 2 );
		RUNNINGP;
		if( valid(value) ) {
			IM.ip = value;
			return;
		} else {
			print("\nError setting IM.ip to %ld\n", value);
			IM.state = ERRMEM;
			return;
		}
	}
	IM.ip += 3;
}

void icm_lt() {
	vlong noun = 0, verb = 0;

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
	vlong noun = 0, verb = 0;

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

void icm_add() {
	vlong noun = 0, verb = 0;

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
	vlong noun = 0, verb = 0;

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
vlong readmem(int addr) {
	vlong value = 0;
	long reladdr = 0;

	switch( IM.mode%10 ) {
		case POSITION:
			if( valid(addr) ) {
				value = IM.mem[ addr ];
				if( valid(value) ) {
					value = IM.mem[ value ];
				} else {
					IM.state = ERRMEM;
				}
			} else {
				IM.state = ERRMEM;
			}
			break;
		case RELATIVE:
			if( (valid(addr)) ) {
				reladdr = IM.mem[ addr ] + IM.base;
			} else {
				reladdr = IM.base;
			}
			if( valid(reladdr) ) {
				value = IM.mem[ reladdr ];
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

void writemem(int addr, vlong value) {
	long reladdr = 0;

	switch( IM.mode%10 ) {
		case POSITION:
			if( valid( addr ) ) {
				reladdr = IM.mem[ addr ];
			} else {
				IM.state = ERRMEM;
			}
			if( valid( reladdr ) ) {
				IM.mem[ reladdr ] = value;
			} else {
				IM.state = ERRMEM;
			}
			break;
		case RELATIVE:
			if( valid( addr ) ) {
				reladdr = IM.mem[ addr ] + IM.base;
			} else {
				IM.state = ERRMEM;
			}
			if( valid( reladdr ) ) {
				IM.mem[ reladdr ] = value;
			} else {
				IM.state = ERRMEM;
			}
			break;
		case IMMEDIATE:   /* Fall through, immediate mode invalid */
		default:
			IM.state = ERRMODE;
	}
	if( addr > IM.highest ) {
		IM.highest = addr;
	}
	if( reladdr > IM.highest ) {
		IM.highest = reladdr;
	}
	IM.mode /= 10;
}
