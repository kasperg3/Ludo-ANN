#include "dialog.h"
#include <QApplication>
#include "game.h"
#include <vector>
#include "ludo_player.h"
#include "ludo_player_random.h"
#include "positions_and_dice.h"
#include "ludo_player_ann.h"
#include <fstream>
#include "fann.h"



Q_DECLARE_METATYPE( positions_and_dice )

void log_training_data(game &g,  QApplication &a, ludo_player_ann &p1, ludo_player_ann &p2, ludo_player_ann &p3, ludo_player_ann &p4 ){

    int in_out_sets = 0;

    std::ofstream ofs ("/home/kasper/qtworkspace/Ludo/validate10Games.data", std::ofstream::out);


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
}

void train_network(){
        fann_type *calc_out;
        const unsigned int num_input = 56;
        const unsigned int num_output = 4;
        const unsigned int num_layers = 3;
        const unsigned int num_neurons_hidden = 30;
        const float desired_error = (const float) 0;
        const unsigned int max_epochs = 10000;
        const unsigned int epochs_between_reports = 10;
        const float init_range = 0.77;
        struct fann *ann;
        struct fann_train_data *data;

        unsigned int i = 0;
        unsigned int decimal_point;

        printf("Creating network.\n");
        ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);

        data = fann_read_train_from_file(TRAINING_DATA);

        fann_set_activation_steepness_hidden(ann, 1);
        fann_set_activation_steepness_output(ann, 1);

        fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
        fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

        fann_set_train_stop_function(ann, FANN_STOPFUNC_BIT);

        fann_set_training_algorithm(ann, FANN_TRAIN_BATCH);

        fann_randomize_weights(ann, -init_range, init_range);

        printf("Training network.\n");
        fann_train_on_data(ann, data, max_epochs, epochs_between_reports, desired_error);

        printf("Testing network. %f\n", fann_test_data(ann, data));

        for(i = 0; i < fann_length_train_data(data); i++)
        {
            calc_out = fann_run(ann, data->input[i]);
        }

        printf("Saving network.\n");

        fann_save(ann, TRAINED_NETWORK_PATH);

        printf("Cleaning up.\n");
        fann_destroy_train(data);
        fann_destroy(ann);

}
int test_network(){
    fann_type *calc_out;
        unsigned int i;
        int ret = 0;

        struct fann *ann;
        struct fann_train_data *data;

        printf("Creating network.\n");

        ann = fann_create_from_file(TRAINED_NETWORK_PATH);

        if(!ann)
        {
            printf("Error creating ann --- ABORTING.\n");
            return -1;
        }

        fann_print_connections(ann);
        fann_print_parameters(ann);

        data = fann_read_train_from_file(VALIDATION_DATA); //Validation data

        for(i = 0; i < fann_length_train_data(data); i++) {
                fann_reset_MSE(ann);
                calc_out = fann_test(ann, data->input[i], data->output[i]);
                std::cout << "validation " << i << ": " << (float) fann_abs(calc_out[0] - data->output[i][0]) << std::endl;
        }

        fann_reset_MSE(ann);
        std::cout << "Validation on whole set: " << fann_test_data(ann, data) << std::endl;

        printf("Cleaning up.\n");
        fann_destroy_train(data);
        fann_destroy(ann);

        return ret;
}


int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    qRegisterMetaType<positions_and_dice>();

    //instanciate the players here
    ludo_player_ann p1(4), p2(0), p3(1), p4(2);

    game g;
    g.setGameDelay(100); //if you want to see the game, set a delay

    // Add a GUI <-- remove the '/' to uncomment block
    Dialog w;
    QObject::connect(&g,SIGNAL(update_graphics(std::vector<int>)),&w,SLOT(update_graphics(std::vector<int>)));
    QObject::connect(&g,SIGNAL(set_color(int)),                   &w,SLOT(get_color(int)));
    QObject::connect(&g,SIGNAL(set_dice_result(int)),             &w,SLOT(get_dice_result(int)));
    QObject::connect(&g,SIGNAL(declare_winner(int)),              &w,SLOT(get_winner()));
    QObject::connect(&g,SIGNAL(close()),&a,SLOT(quit()));
    w.show();

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


    int winnerArray[4] = {0,0,0,0};
    for(int j = 0; j < 1000; ++j){
        g.start();
        a.exec();
        g.reset();
        int winner = g.get_winner();

        winnerArray[winner] += 1;

        while (a.closingDown());
        g.reset();
        if(g.wait());
    }

    std::cout << "wins:" <<  winnerArray[0] << ", "<<  winnerArray[1] << ", "<<  winnerArray[2] << ", "<<  winnerArray[3];

    //log_training_data(g,a,p1,p2,p3,p4);

    //train_network();
    //test_network();
    return 0;
}
