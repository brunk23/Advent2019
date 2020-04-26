#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "machine.h"

void
icm_out(Intcode *M) {
	vlong value = 0;

	value = readmem( M, M->ip + 1 );
	RUNNINGP;

	print("%lld\n", value);

	M->ip += 2;
}

void
icm_in(Intcode *M) {
	vlong value = 0;

	if( EOF == scanf("%d", &value) ) {
		M->state = ERRIN;
		return;
	}

	writemem( M, M->ip + 1, value );
	RUNNINGP;

	M->ip += 2;
}
