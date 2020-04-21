#define SIZE	1000
#define POSITION 0
#define IMMEDIATE 1
#define ADD	1
#define MULT	2
#define	IN	3
#define OUT	4
#define HALT	99
#define STOPPED	23
#define RUNNING 24
#define ERROP	25
#define ERRMEM	26
#define ERRMODE	27

struct Intcode_machine {
	int ip;
	int inst;
	int mode;
	int state;
	long long mem[SIZE];
} IM;

/* Functions in Icode-init.c */
int print_mem(int, int);
int populate(void);
int init(void);
int valid(int);
char *print_state(void);

/* Functions in Icode-op.c */
void step(void);
void icm_add(void);
void icm_mult(void);
void icm_halt(void);
long long readmem(int);
void writemem(int, long long);
