#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "machine.h"

/*
 * Set machine up for initial run.
 */
int init(void) {
	int i = 0;

	/*
	 * Clear the address table, set all cells EMPTY
	 */
	for( i = 0; i < WSIZE; i++ ) {
		IM.mem[i][0] = EMPTY;
		IM.mem[i][1] = 0;
	}

	IM.ip = 0;
	IM.inst = 0;
	IM.mode = 0;
	IM.base = 0;

	return 0;
}

/*
 * Print memory range.
 */
int print_mem() {
	int i = 0;

	for( i = 0; i < WSIZE; i++ ) {
		if( IM.mem[i][0] == EMPTY ) {
			break;
		}
		print("%lld: %lld\n", IM.mem[i][0], IM.mem[i][1]);
	}

	return 0;
}

/*
 * Will read input in form: 1,2,3,4,5,6,0,32,2 ... and put it into memory
 * Returns number of integers read
 */
vlong populate(void) {
	vlong numbersRead = 0, value = 0;
	int sign = 0;
	long maxlen = 0, i, j;
	char *str = malloc(MAXSTR * sizeof(char));
	FILE *fp;

	if( !str ) {
		print("Can't get room for string reading\n");
		return ERRFILE;
	}

	fp = fopen("input", "r");

	if( !fp ) {
		return ERRFILE;
	}

	while( EOF != ( fscanf(fp, "%s,", str ) ) ) {
		maxlen = strlen(str);
		for( i = 0; i < maxlen; i++ ) {
			value = 0;
			sign = 1;
			for( j = i; j < maxlen; j++ ) {
				if( str[j] == ',' ) {
					break;
				}
				i++;
				if( value == 0 && str[j] == '-' ) {
					sign = -1;
					continue;
				}
				value *= 10;
				value += str[j] - '0';
			}
			value *= sign;
			IM.mem[ numbersRead ][0] = numbersRead;
			IM.mem[ numbersRead ][1] = value;
			numbersRead++;
			if( numbersRead == WSIZE ) {
				exits("Out of memory during populate!");
			}
		}
	}
	fclose(fp);
	free(str);
	return numbersRead;
}

/*
 * Returns pointer to data if address is valid, and NULL if not.
 */
vlong *valid(vlong addr) {
	int i = 0;

	if( addr < 0 ) {
		return NULL;
	}

	for( i = 0; i < WSIZE; i++ ) {
		if( IM.mem[i][0] == addr ) {
			return &IM.mem[i][1];
		}
		if( IM.mem[i][0] == EMPTY ) {
			break;
		}
	}
	return NULL;
}

void addpair( vlong addr, vlong value ) {
	int i = 0;

	for( i = 0; i < WSIZE; i++ ) {
		if( IM.mem[i][0] == addr ) {
			/* This should never happen */
			IM.mem[i][1] = value;
			return;
		}
		if( IM.mem[i][0] == EMPTY ) {
			IM.mem[i][0] = addr;
			IM.mem[i][1] = value;
			return;
		}
	}
	print("Out of memory in addpair\n");
	IM.state = ERRMEM;
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
		case ERRIN:
			return "ERRIN";
		default:
			break;
	}
	return "UNKNOWN STATE";
}
