//
// Created by valera on 02.11.17.
//

#include <stdio.h>
#include <string.h>
#include "move.h"
#include "board.h"

extern Board position;
extern int FigureWhiteCoord256[16];
extern int FigureBlackCoord256[16];
extern int *KingWhitePointer;
extern int *KingBlackPointer;

extern MOVE moves[DEPTH][200]; //ходы фигурой

int KingMove[9] = {16, -16, 1, -1, 17, -17, 15, -15, 0};//ходы короля
int QueenMove[9] = {16, -16, 1, -1, 17, -17, 15, -15, 0};//ходы ферзя
int RookMove[5] = {16, -16, 1, -1, 0};//ходы ладьи
int BishopMove[5] = {17, -17, 15, -15, 0};//ходы слона
int KnightMove[9] = {32 + 1, 32 - 1, 16 + 2, 16 - 2, -(32 + 1), -(32 - 1), -(16 + 2), -(16 - 2), 0};//ходы короля

int current_move[DEPTH];


Board old_position[DEPTH];
int FigureWhiteCoord_old[DEPTH][16];
int FigureBlackCoord_old[DEPTH][16];

// Получаем все ходы
void generate_moves(int depth, int current_player) {

    current_move[depth] = 0;
    if (current_player) {
        for (int i = 0; i < 16; i++) {
            if (FigureWhiteCoord256[i] != 0) {
                get_moves(FigureWhiteCoord256[i], depth);
            }
        }
    } else {
        for (int i = 0; i < 16; i++) {
            if (FigureBlackCoord256[i] != 0) {
                get_moves(FigureBlackCoord256[i], depth);
            }
        }
    }

    return;

}

// Получаем ходы для каждой фигуры
// Сделать проверху на ходимость
void get_moves(int coord, int depth) {

    int n = 0;
    int cell = position[coord];

    int color = cell & MASK_COLOR;
    int opponent_color = color ^MASK_COLOR;
    int type = cell & MASK_TYPE;

    // король
    if (type == FIGURE_TYPE_KING) {

        n = 0;
        while (KingMove[n] != 0) {

            int new_coord = coord + KingMove[n]; // Смотрим клетку
            cell = position[new_coord];

            if (cell == CELL_EMPTY) {

                //printf("1");
                //if (!check_move_king(coord, view_coord)) // если ходит король, то надо смотреть, чтобы он ходил в незашахованную область
                add_move(depth, coord, new_coord, MOVE_TYPE_SIMPLY);
            }
            {

            }

            /*
            if ((cell & MASK_COLOR) == opponent_color)
                if(!king_is_checked(color))
                    add_move(depth, coord, view_coord, MOVE_TYPE_EAT);
            */

            n++;
        }
        // рокировки и прочее
    }
    // ферз
    if (type == FIGURE_TYPE_QUEEN) {

        n = 0;
        while (QueenMove[n] != 0) {

            int new_coord = coord + QueenMove[n];
            while (position[new_coord] == CELL_EMPTY)
            {
                add_move(depth, coord, new_coord, MOVE_TYPE_SIMPLY);
                new_coord += QueenMove[n];
            }
            {

            }
            n++;
        }
        return;
    }
    //--------------------------------------------------
    //ладья
    //--------------------------------------------------
    if (type == FIGURE_TYPE_ROOK) {

        n = 0;
        while (RookMove[n] != 0) {

            int new_coord = coord + RookMove[n];
            while (position[new_coord] == CELL_EMPTY)
            {
                add_move(depth, coord, new_coord, MOVE_TYPE_SIMPLY);
                new_coord += RookMove[n];
            }
            {

            }
            n++;
        }
        return;
    }
    //--------------------------------------------------
    //слон
    //--------------------------------------------------
    if (type == FIGURE_TYPE_BISHOP) {

        n = 0;
        while (BishopMove[n] != 0) {

            int new_coord = coord + BishopMove[n];
            while (position[new_coord] == CELL_EMPTY)
            {
                add_move(depth, coord, new_coord, MOVE_TYPE_SIMPLY);
                new_coord += BishopMove[n];
            }
            {

            }
            n++;
        }
        return;
    }
    //--------------------------------------------------
    //конь
    //--------------------------------------------------
    if (type == FIGURE_TYPE_KNIGHT) {

        n = 0;
        while (KnightMove[n] != 0) {

            int new_coord = coord + KnightMove[n];
            cell = position[new_coord];
            if (cell == CELL_EMPTY)//можно ходить в эту клетку
            {
                add_move(depth, coord, new_coord, MOVE_TYPE_SIMPLY);
            }
            else {


            }
            n++;
        }
        return;
    }

    if (type==FIGURE_TYPE_PAWN)
    {

        if (color==WHITE)//если это белая пешка
        {
            //ход пешкой на одну клетку вперёд
            if (position[coord+16]==CELL_EMPTY)
            {
                // проверить 8 ряд и первый двойной ход
                add_move(depth, coord, coord+16, MOVE_TYPE_SIMPLY);
            }
            //проверим, можно ли есть
            cell=position[coord-1+16];
            if ((cell&MASK_COLOR)==BLACK)
            {
                // 8 ряд
                add_move(depth, coord, coord-1+16, MOVE_TYPE_SIMPLY);
            }
            cell=position[coord+1+16];
            if ((cell&MASK_COLOR)==BLACK)
            {
                // 8 ряд
                add_move(depth, coord, coord+1+16, MOVE_TYPE_SIMPLY);
            }
        }
        else//если это чёрная пешка
        {
            //ход пешкой на одну клетку вперёд
            if (position[coord-16]==CELL_EMPTY)
            {
                // проверить 8 ряд и первый двойной ход
                add_move(depth, coord, coord-16, MOVE_TYPE_SIMPLY);
            }
            //проверим, можно ли есть
            cell=position[coord-1-16];
            if ((cell&MASK_COLOR)==BLACK)
            {
                // 8 ряд
                add_move(depth, coord, coord-1-16, MOVE_TYPE_SIMPLY);
            }
            cell=position[coord+1-16];
            if ((cell&MASK_COLOR)==BLACK)
            {
                // 8 ряд
                add_move(depth, coord, coord+1-16, MOVE_TYPE_SIMPLY);
            }
        }

        //проверим, можем ли мы взять проходную пешку
        {

        }

    }

}

// надо хранить общий счетчик ховод что ли, хз
void add_move(int depth, int current_coord, int new_coord, MOVE_TYPE type) {

    // проверить, чтобы не было шаха королю
    if (is_legal_move(current_coord, new_coord)) {

        moves[depth][current_move[depth]].current_position = current_coord;
        moves[depth][current_move[depth]].next_position = new_coord;

        moves[depth][current_move[depth]].MoveType = type;
        current_move[depth]++;
    }
}

//
// Если ход верный, нет шаха королю. Наверное, можно как-то ставить бит шаха, подумай
// не мат ли это или пат. Опять же, надо где-то проверить
int is_legal_move(int coord1, int coord2) {

    int cell = position[coord1];
    int cell2 = position[coord2];

    position[coord1] = 0;
    position[coord2] = cell;

    // опять же менять массив со всеми фигурами надо

    int color = cell & MASK_COLOR;
    int opponent_color = color ^MASK_COLOR;

    if(color == WHITE) {
        FigureWhiteCoord256[0] = coord2;
    }
    else {
        FigureBlackCoord256[0] = coord2;
    }

    if (king_is_checked(color)) {

        position[coord1] = cell;
        position[coord2] = cell2;

        if(color == WHITE) {
            FigureWhiteCoord256[0] = coord1;
        }
        else {
            FigureBlackCoord256[0] = coord1;
        }

        return 0;
    } else {

        position[coord1] = cell;
        position[coord2] = cell2;

        if(color == WHITE) {
            FigureWhiteCoord256[0] = coord1;
        }
        else {
            FigureBlackCoord256[0] = coord1;
        }

        return 1;
    }
}

// проверяем зашахован ли в текущей пощиции король
int king_is_checked(int color) {

    if (color == WHITE) //проверка шаха для белого короля
    {
        for (int n = 0; n < 16; n++) {

            int coord = FigureBlackCoord256[n];
            if (coord == 0)
                continue;
            if (check_king(coord))
                return 1; //фигура шахует белого короля
        }
        return 0;
    } else //проверка шаха для чёрного короля
    {
        for (int n = 0; n < 16; n++) {

            int coord = FigureWhiteCoord256[n];
            if (coord == 0)
                continue;//фигуры нет
            if (check_king(coord))
                return 1; //фигура шахует белого короля
        }
        return 0;
    }
}

// сама реализцая проверки на шах
int check_king(int coord) {

    int n = 0;
    int cell = position[coord];

    int color = cell & MASK_COLOR;
    int opponent_color = color ^MASK_COLOR;
    int type = cell & MASK_TYPE;

    int check_type;
    // Король
    if (type == FIGURE_TYPE_KING) {

        n = 0;
        while (KingMove[n] != 0) {

            int new_coord = coord + KingMove[n]; // Смотрим клетку
            cell = position[new_coord];

            if (cell != CELL_EMPTY) {

                check_type = cell & (MASK_TYPE);
                if (check_type == FIGURE_TYPE_KING) {
                    return 1;
                }
            }
            n++;
        }
        return 0;
    }
    // королева
    if (type == FIGURE_TYPE_QUEEN) {

        printf("sdf");
        n = 0;
        while (QueenMove[n] != 0) {

            int new_coord = coord + QueenMove[n]; // Смотрим клетку

            while (position[new_coord] == CELL_EMPTY) { // пока не уперлись в фигуру
                new_coord += QueenMove[n];
            }

            cell = position[new_coord];
            check_type = cell & (MASK_TYPE);

            if (check_type == FIGURE_TYPE_KING) {
                return 1;
            }

            n++;
        }
        return 0;
    }

    // ладья
    if (type == FIGURE_TYPE_ROOK) {

        n = 0;
        while (RookMove[n] != 0) {

            int new_coord = coord + RookMove[n];

            while (position[new_coord] == CELL_EMPTY) { // пока не уперлись в фигуру
                new_coord += RookMove[n];
            }

            cell = position[new_coord];
            check_type = cell & (MASK_TYPE);

            if (check_type == FIGURE_TYPE_KING) {
                return 1;
            }
        }
        return 0;
    }

    // слон
    if (type == FIGURE_TYPE_BISHOP) {
        n = 0;
        while (BishopMove[n] != 0) {

            int new_coord = coord + BishopMove[n];
            while (position[new_coord] == CELL_EMPTY) { // пока не уперлись в фигуру
                new_coord += BishopMove[n];
            }

            cell = position[new_coord];
            check_type = cell & (MASK_TYPE);

            if (check_type == FIGURE_TYPE_KING) {
                return 1;
            }

            n++;
        }
        return 0;
    }

    //конь
    if (type == FIGURE_TYPE_KNIGHT) {
        n = 0;
        while (KnightMove[n] != 0) {

            int new_coord = coord + KnightMove[n];
            cell = position[new_coord];

            check_type = cell & (MASK_TYPE);
            if (check_type == FIGURE_TYPE_KING) {
                return 1;
            }

            n++;
        }
        return 0;
    }
    //пешка
    if (type == FIGURE_TYPE_PAWN) {

        if (color == WHITE) { //если это белая пешка
            //проверим, можно ли есть

            cell = position[coord - 1 + 16]; // поле слева от пешки
            check_type = cell & (MASK_TYPE);
            if (check_type == FIGURE_TYPE_KING) {
                return 1;
            }

            cell = position[coord + 1 + 16]; // поле справа от пешки
            check_type = cell & (MASK_TYPE);
            if (check_type == FIGURE_TYPE_KING) {
                return 1;
            }
        } else {
            //проверим, можно ли есть
            cell = position[coord - 1 - 16]; // поле слева от пешки
            check_type = cell & (MASK_TYPE);
            if (check_type == FIGURE_TYPE_KING) {
                return 1;
            }

            cell = position[coord + 1 - 16]; // поле справа от пешки
            check_type = cell & (MASK_TYPE);
            if (check_type == FIGURE_TYPE_KING) {
                return 1;
            }
        }
        return 0;
    }
    return 0;
}

void make_move(MOVE move, int depth) { // делаем ход

    //printf("dsfs");
    memcpy(old_position[depth], position, 200 * sizeof(int)); // скопировали старую позицию !!! переделай

    if (move.MoveType == MOVE_TYPE_SIMPLY) {

        int cell = position[move.current_position];
        int type = cell & MASK_TYPE;
        int color = cell & MASK_COLOR;

            if (color == WHITE) {

                // переделать. много пешек может быть и хз что с ними делать 2 коня
                FigureWhiteCoord256[type - 1] = move.next_position;
            } else {

                FigureBlackCoord256[type - 1] = move.next_position;
            }

        // перемещаем клетку
        position[move.current_position] = 0;
        position[move.next_position] = cell;
    }
}

void rollback_move(MOVE move, int depth) {

    if (move.MoveType == MOVE_TYPE_SIMPLY) {

        int cell = position[move.next_position];
        int color = cell & MASK_COLOR;
        int type = cell & MASK_TYPE;

        if (color == WHITE) {

            FigureWhiteCoord256[type - 1] = move.current_position;
        } else {

            FigureBlackCoord256[type - 1] = move.current_position;
        }

    }

    // могут быть рокировки и пешки на 8

    memcpy(position, old_position[depth], 200 * sizeof(int)); // копируем старую позицию !!! переделай
}