#include "dialog.h"
#include <QApplication>
#include "game.h"
#include <vector>
#include "ludo_player.h"
#include "ludo_player_random.h"
#include "positions_and_dice.h"
#include "ludo_player_ann.h"
#include <fstream>

Q_DECLARE_METATYPE( positions_and_dice )

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    qRegisterMetaType<positions_and_dice>();

    //instanciate the players here
    ludo_player_ann p1, p2(0), p3(1), p4(2);
//    ludo_player p1;
//    ludo_player_random p2, p3, p4;

    game g;
    g.setGameDelay(0); //if you want to see the game, set a delay

    // Add a GUI <-- remove the '/' to uncomment block
//    Dialog w;
//    QObject::connect(&g,SIGNAL(update_graphics(std::vector<int>)),&w,SLOT(update_graphics(std::vector<int>)));
//    QObject::connect(&g,SIGNAL(set_color(int)),                   &w,SLOT(get_color(int)));
//    QObject::connect(&g,SIGNAL(set_dice_result(int)),             &w,SLOT(get_dice_result(int)));
//    QObject::connect(&g,SIGNAL(declare_winner(int)),              &w,SLOT(get_winner()));
//    QObject::connect(&g,SIGNAL(close()),&a,SLOT(quit()));
//    w.show();


    QObject::connect(&g,SIGNAL(close()),&a,SLOT(quit()));

    //set up for each player
    QObject::connect(&g, SIGNAL(player1_start(positions_and_dice)),&p1,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p1,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player1_end(std::vector<int>)),    &p1,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p1,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    QObject::connect(&g, SIGNAL(player2_start(positions_and_dice)),&p2,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p2,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player2_end(std::vector<int>)),    &p2,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p2,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    QObject::connect(&g, SIGNAL(player3_start(positions_and_dice)),&p3,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p3,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player3_end(std::vector<int>)),    &p3,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p3,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    QObject::connect(&g, SIGNAL(player4_start(positions_and_dice)),&p4,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p4,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player4_end(std::vector<int>)),    &p4,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p4,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    int in_out_sets = 0;

    std::ofstream ofs ("/home/kasper/qtworkspace/Ludo/test.txt", std::ofstream::out);


    for(int j = 0; j < 10; ++j){
        int counter = 0;
        g.start();
        a.exec();
        g.reset();
        int winner = g.get_winner();
        switch(winner)
        {
        case 0:
            for(int k = 0; k < p1.get_ivector().size()/56; k++){
                //std::cout << "k:" << k << std::endl; //Debug
                for(int i = 0; i < 56; i++){
                    ofs << p1.get_ivector().at(i+counter*56) << " ";
                }
                ofs << "\n";
                for(int i = 0; i < 4; i++){
                    ofs << p1.get_ovector().at(i+counter*4) << " ";
                }
                ofs << "\n";
                counter++;
            }
            in_out_sets += p1.get_ivector().size()/56;
            break;
        case 1:
            for(int k = 0; k < p2.get_ivector().size()/56; k++){
                //std::cout << "k:" << k << std::endl; //Debug
                for(int i = 0; i < 56; i++){
                    ofs << p2.get_ivector().at(i+counter*56) << " ";
                }
                ofs << "\n";
                for(int i = 0; i < 4; i++){
                    ofs << p2.get_ovector().at(i+counter*4) << " ";
                }
                ofs << "\n";
                counter++;
            }
            in_out_sets += p2.get_ivector().size()/56;
            break;
        case 2:
            for(int k = 0; k < p3.get_ivector().size()/56; k++){
                //std::cout << "k:" << k << std::endl; //Debug
                for(int i = 0; i < 56; i++){
                    ofs << p3.get_ivector().at(i+counter*56) << " ";
                }
                ofs << "\n";
                for(int i = 0; i < 4; i++){
                    ofs << p3.get_ovector().at(i+counter*4) << " ";
                }
                ofs << "\n";
                counter++;
            }
            in_out_sets += p3.get_ivector().size()/56;
            break;
        case 3:
            for(int k = 0; k < p4.get_ivector().size()/56; k++){
                //std::cout << "k:" << k << std::endl; //Debug
                for(int i = 0; i < 56; i++){
                    ofs << p4.get_ivector().at(i+counter*56) << " ";
                }
                ofs << "\n";
                for(int i = 0; i < 4; i++){
                    ofs << p4.get_ovector().at(i+counter*4) << " ";
                }
                ofs << "\n";
                counter++;
            }
            in_out_sets += p4.get_ivector().size()/56;
            break;
        }

        p1.clear_vector();
        p2.clear_vector();
        p3.clear_vector();
        p4.clear_vector();

        while (a.closingDown());
        g.reset();
        if(g.wait());
    }
    ofs << in_out_sets << " 56 4";
    ofs.close();

    return 0;
}
