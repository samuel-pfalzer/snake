#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
//getch() function implemented using unistd.h and termios.h
#include "getch.h"

#define FIELD_SIZE 20
#define PLAYER_SYMBOL 'S'
#define WALL_SYMBOL '#'
#define EMPTY_SYMBOL ' '
#define FOOD_SYMBOL 'A'
#define QUIT_SYMBOL 'q'
#define MOVE_UP 'w'
#define MOVE_LEFT 'a'
#define MOVE_DOWN 's'
#define MOVE_RIGHT 'd'
#define SPEED 150000

//function to handle user input
void* getch_loop(void* param) {
    char* next_action = (char*) param;
    while (*next_action != QUIT_SYMBOL) {
        *next_action = getch();
    }
}

int main() {
    //array field contains gameboard for printing
    char field[FIELD_SIZE][FIELD_SIZE];
    
    //initialize field with spaces and borders
    for (int y = 0; y < FIELD_SIZE; y++) {
        for (int x = 0; x < FIELD_SIZE; x++) {
            if (
                y == 0 ||
                y == FIELD_SIZE - 1 ||
                x == 0 ||
                x == FIELD_SIZE - 1
            ) {
                field[y][x] = WALL_SYMBOL;
            } else {
                field[y][x] = EMPTY_SYMBOL;
            }
        }
    }
    
    //struct for storing positions
    struct xyCoords {
        int x;
        int y;
    };
    
    //store current position of user
    struct xyCoords position;
    position.x = 1;
    position.y = 1;
    
    //store snake body and pointers to head and tail
    struct xyCoords player_body[(FIELD_SIZE - 2) * (FIELD_SIZE - 2)];
    player_body[0] = position;
    int player_head, player_tail;
    player_head = 0;
    player_tail = 0;
    
    //store position of next food item
    struct xyCoords food_position;
    
    //seed time for random number generation
    srand(time(NULL));
    
    //store next action (moving direction or quit)
    char next_action = MOVE_RIGHT;
    
    //store user score
    int score;
    score = 0;
    
    //wait three loops before placing next food item
    int looper;
    looper = 0;
    
    //thread to handle user input
    pthread_t getch_thread;    
    pthread_create(&getch_thread, NULL, getch_loop, (void*) &next_action);
    
    //main loop
    while (next_action != QUIT_SYMBOL) {
        
        //check for next action and change user position
        switch(next_action) {
            case MOVE_UP:
                if (position.y > 0) {
                    position.y -= 1;
                }
                break;
            case MOVE_LEFT:
                if (position.x > 0) {
                    position.x -= 1;
                }
                break;
            case MOVE_DOWN:
                if (position.y < FIELD_SIZE - 1) {
                    position.y += 1;
                }
                break;
            case MOVE_RIGHT:
                if (position.x < FIELD_SIZE - 1) {
                    position.x += 1;
                }
                break;
            default:
                break;
        }
        
        //perform checks for new position and take actions
        if (
            field[position.y][position.x] == WALL_SYMBOL
            || field[position.y][position.x] == PLAYER_SYMBOL
        ) {
            if (score == (FIELD_SIZE - 2) * (FIELD_SIZE - 2) - 1) {
                printf("\nCONGRATULATIONS, YOU WIN!\n");
            } else {
                printf("\nGAME OVER!\n");
            }
            sleep(1);
            break;
        } else if (field[position.y][position.x] == FOOD_SYMBOL) {
            score++;
            looper = 0;
            
            if (player_head < (FIELD_SIZE - 2) * (FIELD_SIZE - 2) - 1) {
                player_head++;
            } else {
                player_head = 0;
            }
            
            player_body[player_head] = position;
        } else if (field[position.y][position.x] == EMPTY_SYMBOL) {
            field[player_body[player_tail].y][player_body[player_tail].x] = EMPTY_SYMBOL;
            
            if (player_tail < (FIELD_SIZE - 2) * (FIELD_SIZE - 2) - 1) {
                player_tail++;
            } else {
                player_tail = 0;
            }
            
            if (player_head < (FIELD_SIZE - 2) * (FIELD_SIZE - 2) - 1) {
                player_head++;
            } else {
                player_head = 0;
            }
            
            player_body[player_head] = position;
        }
        
        //place user at new position
        field[position.y][position.x] = PLAYER_SYMBOL;
        
        //check if new food item needs to be generated
        if (looper == 0 || looper == 1 || looper == 2) {
            looper++;
        }
        
        //generate new food item
        if (looper == 3) {
            looper++;
            do {
                food_position.x = rand() % (FIELD_SIZE - 2) + 1;
                food_position.y = rand() % (FIELD_SIZE - 2) + 1;
            } while (field[food_position.y][food_position.x] == PLAYER_SYMBOL);
            field[food_position.y][food_position.x] = FOOD_SYMBOL;
        }   
        
        //clear console and print new field
        system("clear");        
        printf("Score: %d\n\n", score);
        for (int y = 0; y < FIELD_SIZE; y++) {
			for (int x = 0; x < FIELD_SIZE; x++) {
                if (field[y][x] == PLAYER_SYMBOL) {
					printf("\033[0;102m \033[0m");
				} else if (field[y][x] == FOOD_SYMBOL) {
					printf("\033[1;31mA\033[0m");
				} else if (field[y][x] == WALL_SYMBOL) {
					printf("\033[0;44m \033[0m");
				} else {
					printf(" ");
            	}
			}
            printf("\n");
        }
        
        //controls speed, SPEED smaller -> higher speed
        usleep(SPEED);
    }
    
    printf("Press q to exit\n");
    
    //wait for input thread to finish its task
    pthread_join(getch_thread, NULL);
    
    return 0;
}
