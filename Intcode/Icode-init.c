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
		IM.mem[i] = 0;
	}

	IM.ip = 0;
	IM.inst = 0;
	IM.mode = 0;
	IM.base = 0;
	IM.highest = 0;

	return 0;
}

/*
 * Print memory range.
 */
int print_mem() {
	int i = 0;

	for( i = 0; i < IM.highest; i++ ) {
		print("%04d: %lld\n", i, IM.mem[i]);
	}

	return 0;
}

/*
 * Will read input in form: 1,2,3,4,5,6,0,32,2 ... and put it into memory
 * Returns number of integers read
 */
int populate(void) {
	vlong value = 0;
	int sign = 0, numbersRead = 0;
	long maxlen = 0, i, j;
	char *str = malloc(MAXSTR * sizeof(char));
	FILE *fp;

	if( !str ) {
		exits("Can't get room for string reading");
	}

	fp = fopen("input", "r");

	if( !fp ) {
		exits("Can't open file 'input' for reading");
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
			IM.mem[ numbersRead ] = value;
			numbersRead++;
			if( numbersRead == WSIZE ) {
				exits("Out of memory during populate!");
			}
		}
	}
	fclose(fp);
	free(str);
	IM.highest = numbersRead;
	return numbersRead;
}

/*
 * Returns 1 address is valid, and 0 if not.
 */
int valid(int addr) {
	int i = 0;

	if( addr < 0 || addr >= WSIZE ) {
		return 0;
	}

	return 1;
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
