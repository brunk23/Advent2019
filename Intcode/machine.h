#define WSIZE	4000
#define MAXSTR	7000
#define EMPTY	-1

#define RUNNINGP if( M->state != RUNNING ) { return; }

typedef struct Intcode Intcode;

enum
{
	/* Addressing modes */
	POSITION = 0, IMMEDIATE, RELATIVE
};

enum
{
	/* The instruction set */
	ADD = 1, MULT, IN, OUT, JT, JF, LT, EQ, ADJRB,
	HALT = 99
};

enum
{
	/* Program States */
	STOPPED = 23, RUNNING, ERROP, ERRMEM, ERRMODE, ERRIN,
	ERRFILE = -1,
	DEBUG = 0
};

struct Intcode {
	int ip;
	long base;
	int inst;
	int mode;
	int state;
	int highest;
	vlong mem[WSIZE];
};

/* Functions in Icode-init.c */
void print_mem(Intcode *);
void populate(Intcode *);
void init(Intcode *);
int valid(int);
char *print_state(Intcode *);

/* Functions in Icode-op.c */
void step(Intcode *);
void icm_add(Intcode *);
void icm_mult(Intcode *);
void icm_halt(Intcode *);
void icm_jt(Intcode *);
void icm_jf(Intcode *);
void icm_lt(Intcode *);
void icm_eq(Intcode *);
void icm_adjrb(Intcode *);
vlong readmem(Intcode *, int);
void writemem(Intcode *,int, vlong);

/* In specialized io file for project */
void icm_in(Intcode *);
void icm_out(Intcode *);
