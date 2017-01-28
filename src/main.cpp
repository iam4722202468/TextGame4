#include <iostream>
#include <unistd.h>
#include <string>
#include "gameController.h"

std::string getUserInput(int options, GameController *currentGame)
{
    int chosenint = -1;
    std::string chosenintString;
    
    while (chosenint < 1 || chosenint > options)
    {
        usleep(500);
        if (!std::cin) 
        {
            std::cin.clear();
            std::cin.ignore(256, '\n');
        }
        std::getline(std::cin,chosenintString);
        
        try {
            chosenint = stoi(chosenintString);
        } catch(...) {
            chosenint = -1;
        }
        
        std::cin.clear();
        
        if(chosenint == 0)
        {
            if(currentGame->getItems() == "")
                std::cout << "You currently have no items\n";
            else
                std::cout << "You have: " << currentGame->getItems() << std::endl;
            std::cout << "Your health is currently " << currentGame->health << std::endl;
        }
    }
    
    return std::to_string(chosenint-1);
}

int main()
{
    GameController game("MOO");
    game.parseFile("game.txt");
    game.storyline = ";setup;";
    
    std::vector<std::string> optionVector;
    int optionNumber;
    std::string userInput;
    
    while(true)
    {
        optionNumber = 0;
        usleep(500);
        
        game.getOptions(optionVector);
        
        std::cout << std::endl;
        
        for(auto w : optionVector)
        {
            std::cout << w << std::endl;
            if(w.find("    ") == 0)
                optionNumber++;
        }
        
        std::cout << std::endl;
        
        userInput = getUserInput(optionNumber, &game);
        
        game.sendInput(userInput);
    }
    
    return 0;
}
