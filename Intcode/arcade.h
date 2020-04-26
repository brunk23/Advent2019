enum
{
	/* Meaning of spot */
	ESPOT = 0, WALL, BLOCK, HPAD, BALL
};

enum
{
	/* OUTPUT STATES */
	WX, WY, WV
};

typedef struct AScreen AScreen;
typedef struct Arcade Arcade;

struct AScreen {
	int x_min;
	int x_max;
	int y_min;
	int y_max;
	int *view;
} ascreen;

struct Arcade {
	int state;
	int x;
	int y;
	int ball;
	int pad;
	int score;
} arcade;

void init_ascreen(void);
void resize_ascreen(int, int, int, int);
int *find_loc(int, int);
int get_val(int, int);
void put_val(int, int, int);
vlong arcade_read(void);
void print_ascreen(void);
int count_blocks(void);
