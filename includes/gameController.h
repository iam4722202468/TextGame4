#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <string>
#include <vector>

#include "storyline.h"
#include "option.h"

class Storyline;
class Option;

struct PlayerInfo
{
    int health;
    std::string name;
};

class Command
{
    public:
    
    std::string command;
    std::string condition;
    std::string commandType;
    
    Command(std::string command, std::string condition, std::string commandType): command(command), condition(condition), commandType(commandType) {}
};

class GameController
{
    public:
    
    GameController(std::string sessionKey);
    ~GameController();
    
    PlayerInfo player;
    
    std::vector<Storyline*> storylines;
    
    std::string playerName;
    std::string maxInventory;
    
    int gameClock; //increases for every option chosen
    std::string sessionKey;
    
    std::vector<std::string> items;
    std::vector<int> itemsAmount;
    
    std::vector<std::string> variables;
    std::vector<std::string> variablesValue;
    
    std::vector<std::string> includedFiles;
    std::vector<Option*> chooseFrom;
    
    std::string storyline;
    
    int health;
    
    bool parseFile(std::string fileName);
    int addStoryline(std::string storylineName, std::string storylineData); //return index of new storyline
    
    bool sendInput(std::string input);
    void getOptions(std::vector<std::string> &toReturn);
    
    bool parseLine(std::string line, std::string currentCondition, std::vector<Command*> &commands); //line parsing here so it can be accessed by options and storylines
    void addCommand(std::string command, std::string condition, std::string commandType, std::vector<Command*> &commands);
    
    std::string getItems();
    std::string doHealth(std::string command);
    
    bool addItem(std::string itemString);
    bool addVariable(std::string variableString);
    
    bool checkCondition(std::string condition);
    
    bool checkVariable(std::string variableCondition);
    
    std::string doCommand(Command* command); //return storyline or game ending, otherwise return empty string
    std::string doCommandBlock(std::vector<Command*> &commands);
    
    std::string doMacros(std::string macroString);
    
    int getItemAmount(std::string itemName);
    std::string getVariableValue(std::string variable);
    
    int getLife(std::string lolineedthis){ return health;}
    int getGameClock(std::string lolineedthis){ return gameClock;}
    
    bool checkConditionGeneric(int (GameController::*getValueFunction)(std::string toSend), std::string itemString);
};

#endif
