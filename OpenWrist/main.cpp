// Main.cpp : Defines the entry point for the console application.
//

#include <iostream> // 	For io on command line
#include <conio.h> // 	Allows use of _getch() if desired
#include <fstream> // 	For writing to file
#include <string> // 	Allows use of strings?
#include "util.h"	
#include "Clock.h"
#include "Input.h"

int Response() {

    int options = 21;
    int current = 0;

    while (!mel::Input::is_key_pressed(mel::Input::Return)) {
        mel::print("Low", false);

        for (int i = 0; i < options; i++) {
            if (i == current) {
                mel::print("[x]", false);
            }

            else {
                mel::print("[ ]", false);
            }

        }
        mel::print("High");
        mel::Clock::wait_for(0.1);
        if (mel::Input::is_key_pressed(mel::Input::Left)) {
            if (current > 0) {
                current -= 1;
            }
        }

        else if (mel::Input::is_key_pressed(mel::Input::Right)) {
            if (current < 20) {
                current += 1;
            }
        }
        system("cls");

    }
    return current;
} 

// Use this as the argument "const std::string& Subjnum"
int main()
{
    int Mental_Demand;
    int Physical_Demand;
    int Temporal_Demand;
    int Performance;
    int Effort;
    int Frustration;
    std::string Subjnum;

    std::cout << "Please enter your subject number as a 2 digit number (e.g. 02, 12, etc) <= ";
    std::cin >> Subjnum;


    std::ofstream myfile;
    myfile.open("NASATLX_Subj_" + Subjnum + ".txt");

    std::cout << "Welcome to the NASA Task Load Index (TLX). This method assesses work load on six 7 - point scales.\n";
    std::cout << "Increments of high, medium and low estimates for each point result in 21 gradations on the scales.\n\n";

    std::cout << "Please respond to every question using the scale provided. 1 is very low, and 21 is very high. \n\n";

    std::cout << "Question 1: How mentally demanding was the task? <= ";
    //std::cin >> Mental_Demand;
    Mental_Demand = Response();
    myfile << "1 \t" + std::to_string(Mental_Demand) + "\n";

    std::cout << "How physically demanding was the task? <= ";
    //std::cin >> Physical_Demand;
    Physical_Demand = Response();
    myfile << "2 \t" + std::to_string(Physical_Demand) + "\n";

    std::cout << "How hurried or rushed was the pace of the task? <= ";
    //std::cin >> Temporal_Demand;
    Temporal_Demand = Response();
    myfile << "3 \t" + std::to_string(Temporal_Demand) + "\n";

    std::cout << "How successful were you in accomplishing what you were asked to do? <= ";
    //std::cin >> Performance;
    Performance = Response();
    myfile << "4 \t" + std::to_string(Performance) + "\n";

    std::cout << "How hard did you have to work to accomplish your level of performance? <= ";
    //std::cin >> Effort;
    Effort = Response();
    myfile << "5 \t" + std::to_string(Effort) + "\n";

    std::cout << "How insecure, discouraged, irritated, stressed, and annoyed were you? <= ";
    //std::cin >> Frustration;
    Frustration = Response();
    myfile << "6 \t" + std::to_string(Frustration) + "\n";

    myfile.close();

    return 0;
}//




