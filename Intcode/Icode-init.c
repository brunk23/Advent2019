#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "machine.h"

MemBlock *newblock(vlong blocknum) {
	MemBlock *b = 0;
	int i;

	print("Creating block: %lld\n", blocknum);
	b = malloc(sizeof(MemBlock));
	if( !b ) {
		print("FAILED TO GET MEMORY.\n");
		abort();
	}

	b->block = blocknum;
	b->next = NULL;
	for( i = 0; i < BSIZE; i++ ) {
		b->data[i] = 0;
	}
	return b;
}

MemBlock *addblock(vlong blocknum) {
	MemBlock *curr = IM.mem;

	while( curr->next ) {
		curr = curr->next;
	}

	curr->next = newblock( blocknum );

	return curr->next;
}
				

/*
 * Set machine up for initial run.
 */
int init(void) {
	int i;

	IM.mem = newblock(0);

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
	MemBlock *curr = IM.mem;
	vlong n;

	while( curr ) {
		print("\n");
		for( n = 0; n < BSIZE; n++ ) {
			if( n%10 == 0 ) {
				print("\n%lld-%lld:\t",curr->block,n);
			}
			print("%lld\t", curr->data[n]);
		}
		curr = curr->next;
	}
	return 0;
}

/*
 * Will read input in form: 1,2,3,4,5,6,0,32,2 ... and put it into memory
 * Returns number of integers read
 */
vlong populate(void) {
	vlong numbersRead = 0, block = 0, curr = 0, value = 0;
	FILE *fp;
	MemBlock *blk = IM.mem;

	fp = fopen("input", "r");

	if( !fp ) {
		return ERRFILE;
	}

	while( EOF != ( fscanf(fp, "%d,", &value ) ) ) {
		if( curr == BSIZE ) {
			block++;
			blk = addblock( block );
			curr = 0;	
		}
		blk->data[curr] = value;
		curr++;
		numbersRead++;
	}

	fclose(fp);
	return numbersRead;
}

/*
 * Returns pointer to block if address is valid, and NULL if not.
 */
MemBlock *valid(vlong addr) {
	MemBlock *curr = IM.mem;
	vlong blk = addr / BSIZE;

	if( addr < 0 ) {
		return NULL;
	}

	while( curr ) {
		if( curr->block == blk ) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
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
