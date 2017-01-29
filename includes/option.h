#ifndef OPTION_H
#define OPTION_H

#include <string>
#include <vector>

#include "gameController.h"

class GameController;
class Command;

class Option {
    private:
    std::string data;
    
    GameController *currentGame;
    
    public:
    
    std::vector<Command*> commands;
    
    std::string optionTitle;
    int optionNumber;
    
    std::string condition;
    
    Option(std::string data, int optionNumber_, std::string optionTitle_, std::string condition_, GameController* currentGame_);
    ~Option();
    
};

#endif
