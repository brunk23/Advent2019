#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "machine.h"

void icm_out() {
	vlong value = 0;

	value = readmem( IM.ip + 1 );
	RUNNINGP;

	print("%lld\n", value);

	IM.ip += 2;
}

void icm_in() {
	vlong value = 0;

	if( EOF == scanf("%d", &value) ) {
		IM.state = ERRIN;
		return;
	}

	writemem( IM.ip + 1, value );
	RUNNINGP;

	IM.ip += 2;
}
