#include "ludo_player_ann.h"
#include <random>

ludo_player_ann::ludo_player_ann():
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
}

int ludo_player_ann::make_decision(){
    if(dice_roll == 6){
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]<0){
                return i;
            }
        }
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
                return i;
            }
        }
    } else {
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
                return i;
            }
        }
        for(int i = 0; i < 4; ++i){ //maybe they are all locked in
            if(pos_start_of_turn[i]<0){
                return i;
            }
        }
    }
    return -1;
}

void ludo_player_ann::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
    int decision = make_decision();
    for(int i = 0; i < 4; i++)
    {
        input.push_back(is_on_home_stretch(pos_start_of_turn.at(i)));
        input.push_back(can_move_to_home_stretch(pos_start_of_turn.at(i),dice_roll));
        input.push_back(can_kill_enemy(dice_roll,i));
        input.push_back(move_star(pos_start_of_turn.at(i),dice_roll));
        input.push_back(can_complete(pos_start_of_turn.at(i),dice_roll));
        input.push_back(is_complete(pos_start_of_turn.at(i)));
        input.push_back(move_out_home(pos_start_of_turn.at(i),dice_roll));
        input.push_back(is_global_safe(i));
        input.push_back(move_global_safe(i,dice_roll));
        input.push_back(is_local_safe(pos_start_of_turn.at(i)));
        input.push_back(move_local_safe(pos_start_of_turn.at(i),dice_roll));
        input.push_back(is_enemy_start(pos_start_of_turn.at(i)));
        input.push_back(move_enemy_start(pos_start_of_turn.at(i),dice_roll));
        input.push_back(is_home(pos_start_of_turn.at(i)));
    }
    switch(decision)
    {
    case 0:
        output.push_back(1);
        output.push_back(0);
        output.push_back(0);
        output.push_back(0);
        break;

    case 1:
        output.push_back(0);
        output.push_back(1);
        output.push_back(0);
        output.push_back(0);
        break;

    case 2:
        output.push_back(0);
        output.push_back(0);
        output.push_back(1);
        output.push_back(0);
        break;

    case 3:
        output.push_back(0);
        output.push_back(0);
        output.push_back(0);
        output.push_back(1);
        break;
    }
    emit select_piece(decision);
}

void ludo_player_ann::post_game_analysis(std::vector<int> relative_pos){
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    emit turn_complete(game_complete);
}

bool ludo_player_ann::move_out_home(int pos, int dice)
{
    if(pos == -1 && dice == 6)
    {
        return true;
    }
    return false;
}
bool ludo_player_ann::is_globe(int pos)
{
    if(pos == 8 ||pos == 21 ||pos == 34 ||pos == 47)
        return true;
    return false;
}

bool ludo_player_ann::is_global_safe(int token_index, int dice)
{
    if(is_globe(pos_start_of_turn.at(token_index)+dice))
        return true;

    if(pos_start_of_turn.at(token_index) == 0)
        return true;

    for(int i = 0; i < 4; i++)
    {
        if(i != token_index)
        {
           if(pos_start_of_turn.at(token_index)+dice == pos_start_of_turn.at(i))
               return true;
        }
    }
    return false;
}

bool ludo_player_ann::move_global_safe(int token_index, int dice)
{
   if(is_global_safe(token_index,dice))
       return true;
   return false;
}

bool ludo_player_ann::is_star(int pos)
{
    if(pos == 5 || pos == 11 || pos == 18 || pos == 24 || pos == 31 || pos == 37 || pos == 44 || pos == 50)
        return true;
    return false;
}

int ludo_player_ann::star_jump(int pos,int dice)
{
    int jump = 0;
    if((pos + dice) == 5 || (pos + dice) == 18 || (pos + dice) == 31 || (pos + dice) == 44 ){
        jump = 6;
    }else if((pos + dice) == 11 || (pos + dice) == 24 || (pos + dice) == 37){
        jump=  7;
    }
    return (pos + dice + jump);
}

bool ludo_player_ann::move_star(int pos, int dice)
{
    if(is_star(pos+dice))
    {
        int next_star = star_jump(pos, dice);
        int occupancy = 0;
        for (int i = 0;i<pos_start_of_turn.size();i++)
        {
            if (pos_start_of_turn.at(i) == next_star)
                occupancy++;
        }
        if(occupancy < 2)
            return true;
    }
    return false;
}

bool ludo_player_ann::is_local_safe(int pos)
{
    bool on_star = is_star(pos);
    for(int i = 4; i < pos_start_of_turn.size(); i++)
    {
        if(pos - pos_start_of_turn.at(i) > 6)
            return true;
        if(on_star)
        {
            int jump = 0;
            if((pos) == 5 || (pos) == 18 || (pos) == 31 || (pos) == 44 ){
                jump = 6;
            }else if((pos) == 11 || (pos) == 24 || (pos) == 37){
                jump=  7;
            }
            if(pos - pos_start_of_turn.at(i) - jump > 6)
            {
                return true;
            }
        }
    }
    return false;
}

bool ludo_player_ann::move_local_safe(int pos, int dice)
{
    if(is_local_safe(pos+dice))
        return true;

    return false;
}

bool ludo_player_ann::is_on_home_stretch(int pos){
    if(pos > 50 && pos != 99){
        return true;
    }else{
        return false;
    }
}

bool ludo_player_ann::can_move_to_home_stretch(int pos, int dice_roll){
    if(is_on_home_stretch(pos+dice_roll) && pos < 51){
        return true;
    }
    return false;
}


bool ludo_player_ann::is_enemy_start(int pos){
    if(pos == 13 || pos == 26 || pos == 39){
        return true;
    }
    return false;
}

bool ludo_player_ann::is_killable(int pos){
    if(is_globe(pos)){//Is on globe
        return false;
    }
    if(is_on_home_stretch(pos)){//Is inhomestretch
        return false;
    }
    int samePos = 0;
    for(int i = 4; i < pos_start_of_turn.size() ; i++){ //check if two players are on the same spot
       for(int j = 4; j < pos_start_of_turn.size(); j++){
           if(pos_start_of_turn[i] == pos_start_of_turn[j] && i != j && pos_start_of_turn[j] != -1 && pos_start_of_turn[i] != -1){
               samePos++;
           }
       }
       if(samePos != 0){ //If there were multiple tokens on one spot
           return false;
       }
    }
    if(is_enemy_start(pos))//Check if it is a enemy player start
        return false;

    return true;
}

bool ludo_player_ann::can_kill_enemy(int dice, int token_index){
    int pos = pos_start_of_turn.at(token_index);
    if (pos == -1 || pos == 99 || is_on_home_stretch(pos) && pos+dice > 50)
        return false;

    int jump = 0;

    for(int i = 4; i < pos_start_of_turn.size(); i++){ //Normal Kill
        if((pos_start_of_turn[token_index] + dice) == pos_start_of_turn[i]){
            if(is_killable(pos_start_of_turn[i]))
                return true;
        }
        if(is_star(pos_start_of_turn[token_index] + dice)){ //Star kill
            if((pos + dice) == 5 || (pos + dice) == 18 || (pos + dice) == 31 || (pos + dice) == 44 ){
                jump = 6;
            }
            if((pos + dice) == 11 || (pos + dice) == 24 || (pos + dice) == 37){
                jump=  7;
            }

            if((pos_start_of_turn[token_index] + dice + jump) == pos_start_of_turn[i]){
                return true;
            }
        }
    }
    return false;
}

bool ludo_player_ann::can_complete(int pos, int dice){
    if((pos + dice) == 57 || (pos + dice == 50)){
        return true;
    }

    return false;
}

bool ludo_player_ann::is_complete(int pos){
    if(pos == 99){
        return true;
    }
    return false;
}

void ludo_player_ann::print_inputs()
{
    for(int i = 0; i < 1; i++)
    {
        std::cout << "Sjerne? " << is_star(pos_start_of_turn.at(i)) << std::endl;
        std::cout << " Globus? " << is_globe(pos_start_of_turn.at(i)) << std::endl;
        std::cout << " Kan komme ud? " << move_out_home(pos_start_of_turn.at(i), dice_roll) << std::endl;
        std::cout << " lokalt sikker? " << is_local_safe(pos_start_of_turn.at(i)) << std::endl;
        std::cout << " bliv lokalt sikker " << move_local_safe(pos_start_of_turn.at(i), dice_roll) << std::endl;
        std::cout << " slå fjende ihjel? " << can_kill_enemy(dice_roll, i) << std::endl;
        std::cout << " globalt sikker? " << is_global_safe(i) << std::endl;
        std::cout << " bliv globalt sikker " << move_global_safe(i, dice_roll) << std::endl;
        std::cout << " til stjerne? " << move_star(pos_start_of_turn.at(i), dice_roll) << std::endl;
    }

}

std::vector<int> ludo_player_ann::get_ivector()
{
    return input;
}

std::vector<int> ludo_player_ann::get_ovector()
{
    return output;
}

bool ludo_player_ann::move_enemy_start(int pos, int dice){
    if(is_enemy_start(pos+dice)){
        return true;
    }
    return false;
}

bool ludo_player_ann::is_home(int pos){
    if(pos == -1)
        return true;
    return false;
}

void ludo_player_ann::clear_vector()
{
    input.clear();
    output.clear();
}

int ludo_player_ann::make_aggressive_decision(){

}
