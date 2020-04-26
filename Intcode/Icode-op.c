#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "machine.h"

void
step(Intcode *M) {
	vlong value, *curr;

	if( valid( M->ip ) ) {
		value = M->mem[ M->ip ];
	} else {
		print("\nError setting M->ip to %ld\n", M->ip);
		M->state = ERRMEM;
		return;
	}

	/* Split the instruction and mode flags */
	M->inst = value%100;
	M->mode = value/100;

	switch( M->inst ) {
		case ADD:
			icm_add(M);
			break;
		case MULT:
			icm_mult(M);
			break;
		case IN:
			icm_in(M);
			break;
		case OUT:
			icm_out(M);
			break;
		case LT:
			icm_lt(M);
			break;
		case EQ:
			icm_eq(M);
			break;
		case JT:
			icm_jt(M);
			break;
		case JF:
			icm_jf(M);
			break;
		case ADJRB:
			icm_adjrb(M);
			break;
		case HALT:
			icm_halt(M);
			break;
		default:
			M->state = ERROP;
			break;
	}
}

void
icm_halt(Intcode *M) {
	M->ip++;
	M->state = STOPPED;
}

void
icm_adjrb(Intcode *M) {
	long value = 0;

	value = readmem( M, M->ip + 1 );
	RUNNINGP;

	M->base += value;
	M->ip += 2;
}

void
icm_jt(Intcode *M) {
	vlong value = 0;

	value = readmem( M, M->ip + 1 );
	RUNNINGP;

	if( value != 0 ) {
		value = readmem( M, M->ip + 2 );
		RUNNINGP;
		if( valid(value) ) {
			M->ip = value;
			return;
		} else {
			print("\nError setting M->ip to %ld\n", value);
			M->state = ERRMEM;
			return;
		}
	}
	M->ip += 3;
}

void
icm_jf(Intcode *M) {
	vlong value = 0;

	value = readmem( M, M->ip + 1 );
	RUNNINGP;

	if( value == 0 ) {
		value = readmem( M, M->ip + 2 );
		RUNNINGP;
		if( valid(value) ) {
			M->ip = value;
			return;
		} else {
			print("\nError setting M->ip to %ld\n", value);
			M->state = ERRMEM;
			return;
		}
	}
	M->ip += 3;
}

void
icm_lt(Intcode *M) {
	vlong noun = 0, verb = 0;

	noun = readmem( M, M->ip + 1 );
	RUNNINGP;

	verb = readmem( M, M->ip + 2 );
	RUNNINGP;

	if( noun < verb ) {
		writemem( M, M->ip + 3, 1 );
	} else {
		writemem( M, M->ip + 3, 0 );
	}
	RUNNINGP;

	M->ip += 4;
}

void
icm_eq(Intcode *M) {
	vlong noun = 0, verb = 0;

	noun = readmem( M, M->ip + 1 );
	RUNNINGP;

	verb = readmem( M, M->ip + 2 );
	RUNNINGP;

	if( noun == verb ) {
		writemem( M, M->ip + 3, 1 );
	} else {
		writemem( M, M->ip + 3, 0 );
	}
	RUNNINGP;

	M->ip += 4;
}

void
icm_add(Intcode *M) {
	vlong noun = 0, verb = 0;

	noun = readmem( M, M->ip + 1 );
	RUNNINGP;

	verb = readmem( M, M->ip + 2 );
	RUNNINGP;

	noun += verb;

	writemem( M, M->ip + 3, noun);
	RUNNINGP;

	M->ip += 4;
}

void
icm_mult(Intcode *M) {
	vlong noun = 0, verb = 0;

	noun = readmem( M, M->ip + 1 );
	RUNNINGP;

	verb = readmem( M, M->ip + 2 );
	RUNNINGP;

	noun *= verb;

	writemem( M, M->ip + 3, noun);
	RUNNINGP;

	M->ip += 4;
}

/*
 * Reads memory from addr using correct mode.
 * Returns: value
 * Warning: changes IM.mode
 */
vlong
readmem(Intcode * M, int addr) {
	vlong value = 0;
	long reladdr = 0;

	switch( M->mode%10 ) {
		case POSITION:
			if( valid(addr) ) {
				value = M->mem[ addr ];
				if( valid(value) ) {
					value = M->mem[ value ];
				} else {
					M->state = ERRMEM;
				}
			} else {
				M->state = ERRMEM;
			}
			break;
		case RELATIVE:
			if( (valid(addr)) ) {
				reladdr = M->mem[ addr ] + M->base;
			} else {
				reladdr = M->base;
			}
			if( valid(reladdr) ) {
				value = M->mem[ reladdr ];
			} else {
				M->state = ERRMEM;
			}
			break;
		case IMMEDIATE:
			if( valid(addr) ) {
				value = M->mem[ addr ];
			} else {
				M->state = ERRMEM;
			}
			break;
		default:
			M->state = ERRMODE;
			break;
	}
	M->mode /= 10;

	return value;
}

void
writemem(Intcode *M, int addr, vlong value) {
	long reladdr = 0;

	switch( M->mode%10 ) {
		case POSITION:
			if( valid( addr ) ) {
				reladdr = M->mem[ addr ];
			} else {
				M->state = ERRMEM;
			}
			if( valid( reladdr ) ) {
				M->mem[ reladdr ] = value;
			} else {
				M->state = ERRMEM;
			}
			break;
		case RELATIVE:
			if( valid( addr ) ) {
				reladdr = M->mem[ addr ] + M->base;
			} else {
				M->state = ERRMEM;
			}
			if( valid( reladdr ) ) {
				M->mem[ reladdr ] = value;
			} else {
				M->state = ERRMEM;
			}
			break;
		case IMMEDIATE:   /* Fall through, immediate mode invalid */
		default:
			M->state = ERRMODE;
	}
	if( addr > M->highest ) {
		M->highest = addr;
	}
	if( reladdr > M->highest ) {
		M->highest = reladdr;
	}
	M->mode /= 10;
}
