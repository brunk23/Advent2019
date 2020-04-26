#include <u.h>
#include <libc.h>
#include "machine.h"

typedef struct iobuff iobuff;

struct iobuff {
	vlong	inp;
	int	status;
};

enum
{
	aread, unread
};


vlong omem[ WSIZE ];
iobuff iotable[ 5 ];
int maxsignal;
int bytes;
int who;
Intcode *M[5];

void copymem(vlong *from, vlong *to);
void swapints(int *from, int *to);
void permute(int *a, int start, int end);
void trial( int *a );

void
main(int argc, char *argv[])
{
	int i;
	int phase[5] = { 5, 6, 7, 8, 9 };

	for( i = 0; i < 5; i++ ) {
		if( !( M[i] = malloc( sizeof(Intcode) ) ) ) {
			exits("Could not create machine.");
		}
	}

	/*
	 * Get the initial state for the machine and save the memory
	 * so we don't need to reread the file from disk.
	 */
	init(M[0]);
	populate(M[0]);
	bytes = M[0]->highest;
	copymem( M[0]->mem, omem );

	maxsignal = 0;
	who = 0;

	permute( phase, 0, 4 );

	print("Max signal: %lld\n", maxsignal);
	exits(0);
}

void
trial(int *a)
{
	int i, j, running;

	for( i = 0; i < 5; i++ ) {
		for( j = 0; j < 5; j++ ) {
			init(M[j]);
			copymem( omem, M[j]->mem );
			M[j]->state = RUNNING;
			iotable[j].inp = a[j];
			iotable[j].status = unread;
		}
		running = RUNNING;
		while( running == RUNNING ) {
			running = STOPPED;
			for( j = 0; j < 5; j++ ) {
				if( M[j]->state == RUNNING ) {
					who = j;
					running = RUNNING;
					step(M[j]);
				}
			}
		}
	}

	if( iotable[ 0 ].inp > maxsignal ) {
		maxsignal = iotable[ 0 ].inp;

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
icm_out(Intcode *M)
{
	vlong value = 0;

	/*
	 * We can't output, the other program hasn't read the last input.
	 */
	if( iotable[ (who+1)%5 ].status == unread ) {
		print("Out blocked\n");
		return;
	}

	value = readmem( M, M->ip + 1 );
	RUNNINGP;

	iotable[ (who+1)%5 ].inp = value;
	iotable[ (who+1)%5 ].status = unread;

	M->ip += 2;
}

void
icm_in(Intcode *M)
{
	vlong value = 0;

	if( iotable[ who ].status == aread ) {
		return;
	}

	writemem( M, M->ip + 1, iotable[ who ].inp );
	iotable[who].status = aread;
	RUNNINGP;

	if( !who && M->highest != (WSIZE - 1) ) {
		iotable[who].status = unread;
		iotable[who].inp = 0;
		M->highest = WSIZE - 1;
	}

	M->ip += 2;
}
