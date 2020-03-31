#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define FIELD_WIDTH 40
#define FIELD_HEIGHT 20
#define FIELD_SIZE (FIELD_WIDTH * FIELD_HEIGHT)
#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4
#define STARTING_LENGTH 3
#define IR_BUFFER_SIZE 128
#define POINT (FIELD_SIZE + 1)
#define POINT_DELAY 10
#define INIT_LOOP_DELAY 500

int field[FIELD_WIDTH][FIELD_HEIGHT];
int direction;
int snake_length;
int delay;

void init();
void runGameLoop();
void drawBorders();
void drawField();
void goToXY(int column, int line);
int moveSnake();
int readKeyboard();
void generatePoint();

int main() {
	
	init();
	runGameLoop();

	return 0;
} // end main

void init() {

    time_t t;
    srand((unsigned)time(&t));

    delay = INIT_LOOP_DELAY;
    snake_length = STARTING_LENGTH;
	drawBorders();
	direction = WEST;
	int centre_x = FIELD_WIDTH / 2;
	int centre_y = FIELD_HEIGHT / 2;

	for (int i = 0; i < STARTING_LENGTH; i++) {
		field[centre_x + i][centre_y] = i + 1;
	} // end for
	
} // end init

void runGameLoop() {
    
    int counter = POINT_DELAY;
    while (readKeyboard()) {
        if (counter == POINT_DELAY) {
            counter = 0;
            generatePoint();
        }
        else {
            counter++;
        }
        if (moveSnake()) {
            goToXY(43, 0);
            printf("game over");
            break;
        } // end if
        drawField();
        Sleep(delay);
    } // end game cycle
} // end runGameLoop

void goToXY(int column, int line) {

    COORD coord;
    coord.X = column;
    coord.Y = line;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hConsole, coord);

} // end goToXY

void drawBorders() {

	for (int i = 0; i < FIELD_HEIGHT + 2; i++) {
		goToXY(0, i);
		printf("%c", 186);
		goToXY(FIELD_WIDTH + 1, i);
		printf("%c", 186);
	} // end for

	for (int i = 0; i < FIELD_WIDTH + 1; i++) {
		goToXY(i, FIELD_HEIGHT + 1);
		printf("%c", 205);
		goToXY(i, 0);
		printf("%c", 205);
	} // end for

	goToXY(0, FIELD_HEIGHT + 1);
	printf("%c", 200);
	goToXY(FIELD_WIDTH + 1, FIELD_HEIGHT + 1);
	printf("%c", 188);
    goToXY(0, 0);
    printf("%c", 201);
    goToXY(FIELD_WIDTH + 1, 0);
    printf("%c", 187);

} // end drawBorders

void drawField() {
    CHAR_INFO ch_buffer[FIELD_SIZE];

    // ch_buffer[0].Char.UnicodeChar = '^';
    // ch_buffer[0].Attributes = 0x0F;
    
    for (int i = 0; i < FIELD_HEIGHT; i++) {
        for (int j = 0; j < FIELD_WIDTH; j++) {
            int position = i * FIELD_WIDTH + j;
            if (field[j][i] == POINT) {
                ch_buffer[position].Attributes = 0x0f;
                ch_buffer[position].Char.UnicodeChar = '#';
            }
            else if (field[j][i]) {
                ch_buffer[position].Attributes = 0x0f;
                ch_buffer[position].Char.UnicodeChar = '*';
            }
            else {
                ch_buffer[position].Attributes = 0x0f;
                ch_buffer[position].Char.UnicodeChar = ' ';
            } // end if
        } // end for
    } // end for
    

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD field_size = { .X = FIELD_WIDTH, .Y = FIELD_HEIGHT };
    COORD corner = { .X = 0, .Y = 0 };
    SMALL_RECT rectangle = {
        .Top = 1,
        // .Left = FIELD_WIDTH * 2 + 10,
        .Left = 1,
        .Bottom = FIELD_HEIGHT,
        // .Right = FIELD_WIDTH * 4 + 9
        .Right = FIELD_WIDTH + 1
    };
    WriteConsoleOutput(hConsole,
        ch_buffer,
        field_size,
        corner,
        &rectangle);
    // goToXY(1, FIELD_WIDTH + 1);
    // printf("SCORE: %d", score);
} // end drawField

int moveSnake() {

    int result = 0;

    // int temp_field[FIELD_WIDTH][FIELD_HEIGHT];
    // int* temp_field = malloc(FIELD_SIZE * sizeof(int));
    int (*temp_field)[FIELD_HEIGHT] = malloc(sizeof(int[FIELD_WIDTH][FIELD_HEIGHT]));
    
    for (int i = 0; i < FIELD_WIDTH; i++) {
        for (int j = 0; j < FIELD_HEIGHT; j++) {
            temp_field[i][j] = 0;
        } // end for
    } // end for

    for (int i = 0; i < FIELD_WIDTH && !result; i++) {
        for (int j = 0; j < FIELD_HEIGHT; j++) {
            if (field[i][j] == 1) {
                // head processing
                temp_field[i][j] = field[i][j] + 1;
                int new_x = i;
                int new_y = j;
                if (direction == NORTH && j >= 0) {
                    new_y = j - 1;
                } else if (direction == WEST && i >= 0) {
                    new_x = i - 1;
                } else if (direction == SOUTH) {
                    new_y = j + 1;
                } else if (direction == EAST) {
                    new_x = i + 1;
                } // end if

                if (new_x < 0 || new_y < 0 || new_x > FIELD_WIDTH - 1 || new_y > FIELD_HEIGHT - 1) {
                    // border collision processing
                    result = 1;
                    break;
                } // end if
                if (field[new_x][new_y] > 0 && field[new_x][new_y] < snake_length) {
                    // snake collision processing
                    result = 1;
                    break;
                } // end if
                if (field[new_x][new_y] == POINT) {
                    field[new_x][new_y] = 0;
                    snake_length++;
                    delay = delay * 0.95;
                }
                temp_field[new_x][new_y] = 1;
            } else if (field[i][j] > 1 && field[i][j] < snake_length) {
                temp_field[i][j] = field[i][j] + 1;
            } else if (field[i][j] == snake_length) {
                temp_field[i][j] = 0;
            } // end if
        } // end for
    } // end for

    for (int i = 0; i < FIELD_WIDTH; i++) {
        for (int j = 0; j < FIELD_HEIGHT; j++) {
            if (field[i][j] == POINT) {
                continue;
            } // end if
            field[i][j] = temp_field[i][j];
        } // end for
    } // end for

    free(temp_field);

    return result;

} // end moveSnake

int readKeyboard() {
    // changes value of direction and returns 0 if esc is pressed

    int result = 1;

    HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD ir_in_buf[IR_BUFFER_SIZE];
    int cnumread;
    int event_num;
    int temp_direction = 0;
    GetNumberOfConsoleInputEvents(hstdin, &event_num);
    if (event_num) {
        ReadConsoleInput(hstdin, ir_in_buf, IR_BUFFER_SIZE, &cnumread);
        goToXY(0, 26);
        printf("%d", cnumread);
        for (int i = 0; i < cnumread && result; i++) {
            INPUT_RECORD event = ir_in_buf[i];
            if (event.EventType == KEY_EVENT) {
                KEY_EVENT_RECORD key_event = event.Event.KeyEvent;
                if (!key_event.bKeyDown) {
                    continue;
                } // end if
                WORD virtual_key_code = key_event.wVirtualKeyCode;
                goToXY(0, 27);
                if (virtual_key_code == VK_LEFT && direction != EAST) {
                    temp_direction = WEST;
                    printf("wst");
                }
                else if (virtual_key_code == VK_RIGHT && direction != WEST) {
                    temp_direction = EAST;
                    printf("est");
                }
                else if (virtual_key_code == VK_DOWN && direction != NORTH) {
                    temp_direction = SOUTH;
                    printf("sth");
                }
                else if (virtual_key_code == VK_UP && direction != SOUTH) {
                    temp_direction = NORTH;
                    printf("nrt");
                }
                else if (virtual_key_code == VK_SPACE) {
                    printf("spc");
                    //TODO game speedup with spacebar
                }
                else if (virtual_key_code == VK_ESCAPE) {
                    result = 0;
                    printf("esc");
                } // end if
            } // end if
        } // end for
        if (temp_direction) {
            direction = temp_direction;
        } // end if
    } // end if

    return result;

} // end readKeyboard

void generatePoint() {
    int rand_x;
    int rand_y;
    do {
        rand_x = rand() % FIELD_WIDTH;
        rand_y = rand() % FIELD_HEIGHT;
    } while (field[rand_x][rand_y]);
    field[rand_x][rand_y] = POINT;
    int temp = 1 + 1;
} // end generatePoint