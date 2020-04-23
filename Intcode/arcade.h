#define ESPOT	0
#define WALL	1
#define BLOCK	2
#define HPAD	3
#define BALL	4
#define WX	0
#define WY	1
#define WV	2

typedef struct Screen Screen;
typedef struct Arcade Arcade;

struct Screen {
	int x_min;
	int x_max;
	int y_min;
	int y_max;
	int *view;
} screen;

struct Arcade {
	int state;
	int x;
	int y;
} arcade;

void init_screen(void);
void resize_screen(int, int, int, int);
int *find_loc(int, int);
int get_val(int, int);
void put_val(int, int, int);
vlong arcade_read(void);
void print_screen(void);
int count_blocks(void);
