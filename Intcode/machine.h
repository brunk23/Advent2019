#define WSIZE	4000
#define MAXSTR	7000
#define EMPTY	-1

#define RUNNINGP if( IM.state != RUNNING ) { return; }

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

struct Intcode_machine {
	int ip;
	long base;
	int inst;
	int mode;
	int state;
	vlong mem[WSIZE][2];
} IM;

/* Functions in Icode-init.c */
int print_mem(void);
vlong populate(void);
int init(void);
vlong *valid(vlong);
void addpair(vlong, vlong);
char *print_state(void);

/* Functions in Icode-op.c */
void step(void);
void icm_add(void);
void icm_mult(void);
void icm_halt(void);
void icm_jt(void);
void icm_jf(void);
void icm_lt(void);
void icm_eq(void);
void icm_adjrb(void);
vlong readmem(long);
void writemem(long, vlong);

/* In specialized io file for project */
void icm_in(void);
void icm_out(void);
