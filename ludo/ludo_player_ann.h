#ifndef LUDO_PLAYER_ANN_H
#define LUDO_PLAYER_ANN_H
#include <QObject>
#include <iostream>
#include "positions_and_dice.h"

class ludo_player_ann : public QObject {
    Q_OBJECT
private:
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
    int dice_roll;
    int make_decision();
    std::vector<int> input;
    std::vector<int> output;
public:
    ludo_player_ann();

    //States
    bool is_on_home_stretch(int);
    bool can_move_to_home_stretch(int pos, int dice_roll);
    bool can_kill_enemy(int dice, int token_index);
    bool is_star(int pos);
    bool is_globe(int pos);
    bool is_killable(int pos);
    bool is_enemy_start(int pos);
    bool can_complete(int pos, int dice);
    bool is_complete(int pos);
    bool move_out_home(int pos, int dice);
    bool is_global_safe(int token_index, int dice = 0);
    bool move_global_safe(int pos, int dice);
    bool move_star(int pos, int dice);
    bool is_local_safe(int pos);
    int star_jump(int pos,int dice);
    bool move_local_safe(int pos, int dice);
    void print_inputs();
    std::vector<int> get_ivector();
    std::vector<int> get_ovector();
    bool move_enemy_start(int pos, int dice);
    bool is_home(int pos);
    void clear_vector();

signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_ANN_H
