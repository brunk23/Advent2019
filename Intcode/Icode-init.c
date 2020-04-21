#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "machine.h"

/*
 * Set machine up for initial run.
 */
int init(void) {
	int i;

	for( i = 0; i < SIZE; i++ ) {
		IM.mem[i] = 0;
	}

	IM.ip = 0;
	IM.inst = 0;
	IM.mode = 0;

	return 0;
}

/*
 * Print memory range.
 */
int print_mem(int start, int end) {
	int n;

	for( n = 0; (n+start) <= end; n++ ) {
		if( n%10 == 0 ) {
			print("\n%d:\t",n+start);
		}
		print("%d\t", IM.mem[n+start]);
	}

	return 0;
}

/*
 * Will read input in form: 1,2,3,4,5,6,0,32,2 ... and put it into memory
 * Returns number of integers read
 */
int populate(void) {
	int numbersRead = 0;

	while( EOF != ( scanf("%d,", &IM.mem[ numbersRead++ ] ) ) ) {
		if( numbersRead == SIZE ) {
			print("MEMORY FULL\n\tPopulate Halted\n\n");
			return numbersRead;
		}
	}

	return numbersRead;
}

/*
 * Returns 1 if address is valid, and 0 if not.
 */
int valid(int addr) {
	if( addr >=0 && addr < SIZE ) {
		return 1;
	}
	return 0;
}

char *print_state() {
	switch( IM.state ) {
		case RUNNING:
			return "RUNNING";
		case STOPPED:
			return "STOPPED";
		case ERRMEM:
			return "ERRMEM";
		case ERRMODE:
			return "ERRMODE";
		case ERROP:
			return "ERROP";
		default:
			break;
	}
	return "UNKNOWN STATE";
}
