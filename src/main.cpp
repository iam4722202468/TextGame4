#include <iostream>
#include <unistd.h>
#include <string>
#include "gameController.h"

struct GameContainer
{
    int optionNumber;
    std::string sessionKey;
    GameController *game;
    std::vector<std::string> optionVector;
};

int findGamePlace(std::vector<GameContainer*> *mainGameVector, std::string gameName)
{
    for(int i = 0; i < mainGameVector->size(); ++i)
        if(mainGameVector->at(i)->sessionKey == gameName)
            return i;
            
    return -1;
}

std::string getUserInput(std::vector<GameContainer*> *mainGameVector, int *gameIndex)
{
    int chosenint = -1;
    std::string userInput;
    std::size_t gameNameIndex;
    std::string gameName;
    int options = -1;
    
    while (chosenint < 1 || chosenint > options || gameIndex < 0)
    {
        usleep(500);
        if (!std::cin) 
        {
            std::cin.clear();
            std::cin.ignore(256, '\n');
        }
        
        std::getline(std::cin,userInput);
        
        //userInput += " MOO";
        
        gameNameIndex = userInput.find_last_of(" ");
        gameName = userInput.substr(gameNameIndex+1, userInput.size());
        userInput.erase(userInput.begin() + gameNameIndex, userInput.end());
        
        *gameIndex = findGamePlace(mainGameVector, gameName);
        if(*gameIndex >= 0)
            options = mainGameVector->at(*gameIndex)->optionNumber;
        
        try {
            chosenint = stoi(userInput);
        } catch(...) {
            chosenint = -1;
        }
        
        std::cin.clear();
        
        if(*gameIndex >= 0 && chosenint <= 0)
        {
            if(mainGameVector->at(*gameIndex)->game->getItems() == "")
                std::cout << "You currently have no items\n";
            else
                std::cout << "You have: " << mainGameVector->at(*gameIndex)->game->getItems() << std::endl;
            std::cout << "Your health is currently " << mainGameVector->at(*gameIndex)->game->health << std::endl;
        }
    }
    
    return std::to_string(chosenint-1);
}

int printOptions(int gameIndex, std::vector<GameContainer*> *mainGameVector)
{
    mainGameVector->at(gameIndex)->optionNumber = 0;
    mainGameVector->at(gameIndex)->game->getOptions(mainGameVector->at(gameIndex)->optionVector);
    
    for(auto w : mainGameVector->at(gameIndex)->optionVector)
    {
        std::cout << w << std::endl;
        if(w.find("    ") == 0)
            mainGameVector->at(gameIndex)->optionNumber++;
    }
}

int main()
{
    std::vector<GameContainer*> mainGameVector;
    
    mainGameVector.push_back(new GameContainer);
    mainGameVector.at(0)->game = new GameController();
    mainGameVector.at(0)->sessionKey = "moo";
    mainGameVector.at(0)->game->parseFile("game.txt");
    mainGameVector.at(0)->game->storyline = ";setup;";
    printOptions(0, &mainGameVector);
    
    std::string userInput;
    int gameIndex;
    
    while(true)
    {
        usleep(100);
        
        //make closing games work
        
        userInput = getUserInput(&mainGameVector, &gameIndex);
        mainGameVector.at(gameIndex)->game->sendInput(userInput);
        printOptions(gameIndex, &mainGameVector);
    }
    
    return 0;
}