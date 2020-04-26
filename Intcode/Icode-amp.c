#include <u.h>
#include <libc.h>
#include "machine.h"

enum
{
	signalp = 0, phaseset, signal, output
};

vlong omem[ WSIZE ];
int iotable[ 4 ];
int maxsignal;
int bytes;
Intcode *M;

void copymem(vlong *from, vlong *to);
void swapints(int *from, int *to);
void permute(int *a, int start, int end);
void trial( int *a );

void
main(int argc, char *argv[])
{
	int i;
	int phase[5] = { 4, 3, 2, 1, 0 };

	if( !( M = malloc( sizeof(Intcode) ) ) ) {
		exits("Could not create machine.");
	}

	/*
	 * Get the initial state for the machine and save the memory
	 * so we don't need to reread the file from disk.
	 */
	init(M);
	populate(M);
	bytes = M->highest;
	copymem( M->mem, omem );

	maxsignal = 0;

	permute( phase, 0, 4 );

	if( M->state != STOPPED || DEBUG ) {
		print_mem(M);
		print("\n\n");
		print("\n\n\tIP: %d\n\tState: %s\n\tMem[0]: %lld\n",
			M->ip, print_state(M), M->mem[0]);

		print("\tHighest Address Used: %ld\n", M->highest);
	}

	print("Max signal: %lld\n", maxsignal);
	exits(0);
}

void
trial(int *a)
{
	int i;

	iotable[ signal ] = 0;

	for( i = 0; i < 5; i++ ) {
		iotable[ signalp ] = 0;
		iotable[ phaseset ] = a[ i ];
		init(M);
		copymem( omem, M->mem );
		M->state = RUNNING;
		while( M->state == RUNNING ) {
			step(M);
		}
		iotable[ signal ] = iotable[ output ];
	}

	if( iotable[ output ] > maxsignal ) {
		maxsignal = iotable[ output ];

		for( i = 0; i < 5; i++ ) {
			print("%d ", a[i]);
		}
		print("\n");
	}
}

/*
 * A helper function to reset the memory
 */
void
copymem(vlong *from, vlong *to)
{
	int i;

	for( i = 0; i < WSIZE; i++ ) {
		to[i] = from[i];
	}
}

/*
 * Helper function for swapping the array.
 */
void
swapints(int *from, int *to)
{
	int tmp = *to;
	*to = *from;
	*from = tmp;
}

void
permute(int *a, int start, int end)
{
	int i;

	if( start == end ) {
		trial(a);
		return;
	}

	for( i = start; i <= end; i++ ) {
		swapints( &a[i], &a[start] );
		permute(a, start+1, end);
		swapints( &a[i], &a[start] );
	}
}

void
icm_out()
{
	vlong value = 0;

	value = readmem( M, M->ip + 1 );
	RUNNINGP;

	iotable[ output ] = value;

	M->ip += 2;
}

void
icm_in()
{
	vlong value = 0;

	writemem( M, M->ip + 1, iotable[ iotable[ signalp ] + 1 ] );
	RUNNINGP;

	iotable[ signalp ]++;
	M->ip += 2;
}
