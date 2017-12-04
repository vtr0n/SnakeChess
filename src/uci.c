#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#include "move.h"
#include "board.h"
#include "algorithms.h"
#include "uci.h"
#include "hash.h"

extern Board position; // main.c

pthread_t thread; // for thread

int uci_work_status = 0;
int current_player = 1; // whose turn, 1 for white, 0 for black, it's not racy
int max_current_deep;

unsigned int count_nodes = 0;

MOVE out_move[2]; // save best move for out

// load board to engine
void fen_to_board(char *str) {

    for (int i = 0; i < 256; i++) // set border flag
        position[i] = BORDER;

    // empty cell
    for (int i = 0; i < 8; i++) {
        for (int j = 68 + i * 16; j < 76 + i * 16; j++)
            position[j] = 0;
    }

    int arr[64];
    for (int i = 0, k = 0, z = 0; str[i] != 0 && str[i] != ' '; i++, k++) {

        if (str[i] > '0' && str[i] < '9') {
            for (int j = 0; j < str[i] - '0'; j++, k++) {
                arr[k] = 0;
            }
            k--;
        }

        // black
        if (str[i] == 'r') {
            arr[k] = FIGURE_TYPE_ROOK | BLACK | IS_MOVE;
        }
        if (str[i] == 'n') {
            arr[k] = FIGURE_TYPE_KNIGHT | BLACK;
        }
        if (str[i] == 'b') {
            arr[k] = FIGURE_TYPE_BISHOP | BLACK;
        }
        if (str[i] == 'q') {
            arr[k] = FIGURE_TYPE_QUEEN | BLACK;
        }
        if (str[i] == 'k') {
            arr[k] = FIGURE_TYPE_KING | BLACK;
        }
        if (str[i] == 'p') {
            if (z == 1)
                arr[k] = FIGURE_TYPE_PAWN | BLACK;
            else
                arr[k] = FIGURE_TYPE_PAWN | BLACK | IS_MOVE;
        }

        // white
        if (str[i] == 'R') {
            arr[k] = FIGURE_TYPE_ROOK | WHITE | IS_MOVE;
        }
        if (str[i] == 'N') {
            arr[k] = FIGURE_TYPE_KNIGHT | WHITE;
        }
        if (str[i] == 'B') {
            arr[k] = FIGURE_TYPE_BISHOP | WHITE;
        }
        if (str[i] == 'Q') {
            arr[k] = FIGURE_TYPE_QUEEN | WHITE;
        }
        if (str[i] == 'K') {
            arr[k] = FIGURE_TYPE_KING | WHITE;
        }
        if (str[i] == 'P') {
            if (z == 6) // Если на исходной позиции
                arr[k] = FIGURE_TYPE_PAWN | WHITE;
            else
                arr[k] = FIGURE_TYPE_PAWN | WHITE | IS_MOVE;
        }

        if (str[i] == '/') {
            k--;
            z++;
        }
    }

    for (int i = 0, k = 0; i < 8; i++) {
        for (int j = 68 + i * 16; j < 76 + i * 16; j++, k++) {

            position[j] = arr[k];
        }
    }

    int i = 0;

    while (str[i] != ' ') {
        i++;
    }

    if (str[i + 1] == 'w') {
        current_player = 1;
    }
    else {
        current_player = 0;
    }


    i += 3;

    while (str[i] != ' ') {

        if (str[i] == 'K') {
            if (position[187] != 0)
                position[187] = FIGURE_TYPE_ROOK | WHITE;
        }

        if (str[i] == 'Q') {
            if (position[180] != 0)
                position[180] = FIGURE_TYPE_ROOK | WHITE;
        }

        if (str[i] == 'k') {
            if (position[75] != 0)
                position[75] = FIGURE_TYPE_ROOK | BLACK;
        }

        if (str[i] == 'q') {
            if (position[68] != 0)
                position[68] = FIGURE_TYPE_ROOK | BLACK;
        }

        i++;
    }

    i++;

    if (str[i] != '-') {

        int passed_pawn_coord = uci_to_coord(str[i], str[i + 1]);
        if (passed_pawn_coord < 112) {
            int cell = position[passed_pawn_coord + 16];
            position[passed_pawn_coord + 16] = cell | IS_PASSED_PAWN_UCI;
        }
        else {
            int cell = position[passed_pawn_coord - 16];
            position[passed_pawn_coord - 16] = cell | IS_PASSED_PAWN_UCI;
        }
    }
}

// change coord of the form a1-h8 to form 0-255
int uci_to_coord(char a, char b) {

    int coord = 180;
    coord = coord + a - 'a'; // добавили по иксу
    coord = coord + ('1' - b) * 16; // добавили по y

    return coord;
}

// change coord of the form 0-255 to form a1-h8
void get_position(int position, char *str) {

    position = position - 64;

    int x = position % 16 - 4;
    int y = 8 - position / 16;

    str[0] = x + 'a';
    str[1] = y + '0';
}

// change MOVE to form a1-h8
void move_to_uci(MOVE move, char *out) {

    // получаем координату в нашеим виде e2
    int first = move.current_position;
    int second = move.next_position;

    char first_char[3];
    char second_char[3];

    get_position(first, first_char);

    get_position(second, second_char);

    if (move.MoveType == MOVE_TYPE_SIMPLY || move.MoveType == MOVE_TYPE_EAT ||
        move.MoveType == MOVE_TYPE_CASTLING_O_O || move.MoveType == MOVE_TYPE_CASTLING_O_O_0) {

        out[0] = first_char[0];
        out[1] = first_char[1];
        out[2] = second_char[0];
        out[3] = second_char[1];
        out[4] = 0;
    }

    if (move.MoveType == MOVE_TYPE_CONVERSION) {

        char new_fig;

        if (move.NewFigureType == FIGURE_TYPE_QUEEN)
            new_fig = 'q';

        if (move.NewFigureType == FIGURE_TYPE_ROOK)
            new_fig = 'r';

        if (move.NewFigureType == FIGURE_TYPE_BISHOP)
            new_fig = 'b';

        if (move.NewFigureType == FIGURE_TYPE_KNIGHT)
            new_fig = 'n';

        out[0] = first_char[0];
        out[1] = first_char[1];
        out[2] = second_char[0];
        out[3] = second_char[1];
        out[4] = new_fig;
        out[5] = 0;
    }
}

// thread to infinite analyze
void *start() {

    time_t time1, time2;

    max_current_deep = 1;
    uci_work_status = 1;
    while (max_current_deep < DEPTH) {

        char best_move[10], old_best_move[10];

        count_nodes = 0;
        hash_init();
        move_init();

        time1 = clock();

        //int score = minimax(max_current_deep, current_player);
        int score = alpha_beta(-INF, INF, max_current_deep, 0, current_player, 0);

        time2 = clock();

        int time_def = (int) ((time2 - time1) / 1000);

        if (time_def == 0)
            time_def = 1;

        int nodes_by_sec = (int) (count_nodes / (time_def * 1000));


        move_to_uci(out_move[0], best_move);

        if (score != -INF && score != INF) {

            printf("info depth %d nodes %d score cp %d time %d pv %s\n", max_current_deep, count_nodes, score, time_def,
                   best_move);

            memcpy(old_best_move, best_move, 10 * sizeof(char));
        }
        else {

            printf("bestmove %s\n", old_best_move);
            max_current_deep = DEPTH;
        }

        fflush(stdout);
        max_current_deep++;
    }
}

// listen GUI
void uci_listen() {

    char input[100];

    while (fgets(input, 90, stdin)) {

        if (strstr(input, "uci")) {

            printf("id name Demo_engine\n");
            printf("id author XXX\n");
            printf("uciok\n");
            printf("position startpos\n");
        }

        if (strstr(input, "quit") || strstr(input, "exit")) {

            uci_work_status = 0;
            printf("quit\n");
            return;
        }

        if (strstr(input, "isready")) {

            printf("readyok\n");
        }

        if (strstr(input, "go inf")) {

            pthread_create(&thread, NULL, start, NULL);
        }

        if (strstr(input, "go movetime")) {

            int max_move_time = atoi(&input[11]);

            if(!max_move_time) {

                printf("Incorrect max time\n");
                return;
            }

            int time_diff = 0;

            time_t time1, time2;

            pthread_create(&thread, NULL, start, NULL);
            time1 = clock();


            while (time_diff < max_move_time) {

                time2 = clock();
                time_diff = (int) ((time2 - time1) / 1000);
            }

            uci_work_status = 0;
        }

        if (strstr(input, "position fen ")) {

            uci_work_status = 0;

            char changed[100];
            strcpy(changed, &input[13]);

            fen_to_board(changed);
            board_print2(position);

        }

        if (strstr(input, "stop")) {

            uci_work_status = 0;
        }

        fflush(stdout);
    }
}