#include <iostream>
#include <unistd.h>
#include <string>
#include "gameController.h"
#include "extras.h"

//for random numbers
#include <cstdlib>
#include <time.h>
#include <ctime>

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

int printOptions(int gameIndex, std::vector<GameContainer*> *mainGameVector)
{
    mainGameVector->at(gameIndex)->optionNumber = 0;
    
    if(mainGameVector->at(gameIndex)->game->getOptions(mainGameVector->at(gameIndex)->optionVector))
    {
        for(auto w : mainGameVector->at(gameIndex)->optionVector)
        {
            std::cout << w << " " << mainGameVector->at(gameIndex)->sessionKey << std::endl;
            //std::cout << w << std::endl;
            if(w.find("    ") == 0)
                mainGameVector->at(gameIndex)->optionNumber++;
        }
    }
    else
    {
        return -1;
    }
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
        
        /////
        //userInput += " moo|";
        /////
        
        // Input is from user
        /* Ends with |
         */
        if(userInput[userInput.size()-1] == '|')
        {
            gameNameIndex = userInput.find_last_of(" ");
            gameName = userInput.substr(gameNameIndex+1, userInput.size() - gameNameIndex - 2);
            userInput.erase(userInput.begin() + gameNameIndex, userInput.end());
            
            *gameIndex = findGamePlace(mainGameVector, gameName);
            if(*gameIndex >= 0)
                options = mainGameVector->at(*gameIndex)->optionNumber;
            
            try {
                chosenint = stoi(userInput);
            } catch(...) {
                chosenint = -1;
            }
            
            std::cout << "  " << gameName << std::endl;
            std::cin.clear();
            
            if(*gameIndex >= 0 && chosenint <= 0)
            {
                if(mainGameVector->at(*gameIndex)->game->getItems() == "")
                    std::cout << "You currently have no items " << gameName << std::endl;
                else
                    std::cout << "You have: " << mainGameVector->at(*gameIndex)->game->getItems() << " " << gameName << std::endl;
                std::cout << "Your health is currently " << mainGameVector->at(*gameIndex)->game->health << " " << gameName << std::endl;
                std::cout << "  " << gameName << std::endl;
            }
        }
        //Server command//
        /* NEW <sessionkey> <gameFile> <startlocation>
         * DELETE <sessionkey>
         * INFO <sessionkey>
         */
        else 
        {
            std::vector<std::string> splitText;
            stringsplit(' ', userInput, splitText);
            
            if(splitText.at(0) == "NEW" && splitText.size() == 4) {
                mainGameVector->push_back(new GameContainer);
                mainGameVector->at(mainGameVector->size()-1)->game = new GameController(splitText.at(1)); //moo
                mainGameVector->at(mainGameVector->size()-1)->sessionKey = splitText.at(1); //moo
                mainGameVector->at(mainGameVector->size()-1)->game->parseFile(splitText.at(2)); //game.txt
                mainGameVector->at(mainGameVector->size()-1)->game->storyline = splitText.at(3); //;setup;
                printOptions(mainGameVector->size()-1, mainGameVector);
                
                //std::cout << "CREATED " << splitText.at(1) << std::endl;
            } else if(splitText.at(0) == "DELETE" && splitText.size() == 2) {
                *gameIndex = findGamePlace(mainGameVector, splitText.at(1));
                
                if(*gameIndex != -1)
                {
                    delete mainGameVector->at(*gameIndex)->game;
                    delete mainGameVector->at(*gameIndex);
                    mainGameVector->erase(mainGameVector->begin() + *gameIndex);
                    //std::cout << "DELETED " << splitText.at(1) << std::endl;
                }
            } else if(splitText.at(0) == "INFO" && splitText.size() == 2) {
                //put stuff here
            }
        }
    }
    
    return std::to_string(chosenint-1);
}

int main()
{
    srand(time(NULL));
    
    std::vector<GameContainer*> mainGameVector;
    
    std::string userInput;
    int gameIndex;
    
    /*
    mainGameVector.push_back(new GameContainer);
    mainGameVector.at(mainGameVector.size()-1)->game = new GameController("moo"); //moo
    mainGameVector.at(mainGameVector.size()-1)->sessionKey = "moo"; //moo
    mainGameVector.at(mainGameVector.size()-1)->game->parseFile("game.txt"); //game.txt
    mainGameVector.at(mainGameVector.size()-1)->game->storyline = ";setup;"; //;setup;
    printOptions(mainGameVector.size()-1, &mainGameVector);
    */
    
    while(true)
    {
        usleep(100);
        
        userInput = getUserInput(&mainGameVector, &gameIndex);
        
        //sendInput returns false if game ends from inside an option. if printOptions returns -1, game end was found when parsing storyline
        if(!mainGameVector.at(gameIndex)->game->sendInput(userInput) || printOptions(gameIndex, &mainGameVector) == -1)
        {
            std::cout << "DEATH " << mainGameVector.at(gameIndex)->sessionKey << std::endl;
            delete mainGameVector.at(gameIndex)->game;
            delete mainGameVector.at(gameIndex);
            mainGameVector.erase(mainGameVector.begin() + gameIndex);
        }
    }
    
    return 0;
}
