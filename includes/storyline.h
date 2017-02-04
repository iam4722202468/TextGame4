#ifndef STORYLINE_H
#define STORYLINE_H

#include <vector>
#include <string>
#include "option.h"

class Option;
class GameController;
class Command;

class Storyline
{
    private:
    GameController *currentGame;
    
    int getresponse(int options);
    
    std::string data;
    
    public:
    
    std::vector<Option*> options;
    std::vector<Command*> commands; //to be ran when option is chosen
    
    std::string storylineName;
    Storyline(std::string data, std::string storylineName, GameController* currentGame_);
    ~Storyline();
    std::string getOption(std::string userInput, std::vector<Option*> &chooseFrom);
    void displayOptions(std::vector<Option*> &chooseFrom, std::vector<std::string> &returnOptions);
};

#endif
