#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <string>
#include <vector>

#include "storyline.h"

class Storyline;

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
	
	GameController();
	
	PlayerInfo player;
	
	std::vector<Storyline*> storylines;
	
	std::string playerName;
	std::string maxInventory;
	
	int gameClock; //increases for every option chosen
	
	std::vector<std::string> items;
	std::vector<int> itemsAmount;
	
	std::vector<std::string> variables;
	std::vector<std::string> variablesValue;
	
	std::vector<std::string> includedFiles;
	
	std::string storyline;
	int health;
	
	bool parseFile(std::string fileName);
	int addStoryline(std::string storylineName, std::string storylineData); //return index of new storyline
	
	void start();
	
	bool parseLine(std::string line, std::string currentCondition, std::vector<Command*> &commands); //line parsing here so it can be accessed by options and storylines
	void addCommand(std::string command, std::string condition, std::string commandType, std::vector<Command*> &commands);
	
	std::string getItems();
	std::string doHealth(std::string command);
	
	bool addItem(std::string itemString);
	bool checkCondition(std::string condition);
	std::string doCommand(Command* command); //return storyline, otherwise return empty string
	std::string doCommandBlock(std::vector<Command*> &commands);
	
	int getItemAmount(std::string itemName);
	int getLife(std::string lolineedthis){ return health;}
	int getGameClock(std::string lolineedthis){ return gameClock;}
	
	bool checkConditionGeneric(int (GameController::*getValueFunction)(std::string toSend), std::string itemString);
};

#endif
