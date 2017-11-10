//
// Created by valera on 10.11.17.
//

#ifndef CHESS_UCI_H
#define CHESS_UCI_H

void fen_to_board(char* str);
void move_to_uci(MOVE move, char* out);
char* get_position(int position, char* str);
void uci_listen();
int get_current_payer(char *str);
void start();

#endif //CHESS_UCI_H