#include <iostream>
#include <string>
#include <vector>
#include <fstream>

//for random numbers
#include <cstdlib>
#include <time.h>

#include <unistd.h>

#include "gameController.h"
#include "extras.h"

GameController::GameController()
{
	health = 100;
	gameClock = 0;
}

int GameController::addStoryline(std::string storylineName, std::string storylineData)
{
	storylines.push_back(new Storyline(storylineData, storylineName, this));
	return 0;
}

void GameController::start()
{
	std::string returnString;
	
	while(true)
	{
		usleep(500);
		for(auto place : storylines)
			if(place->storylineName == storyline)
			{
				returnString = place->getOption();
				
				if(returnString[0] != ';')
				{
					std::cout << returnString << std::endl;
					return;
				}
				else
					storyline = returnString;
			}
	}
}

std::string GameController::doCommandBlock(std::vector<Command*> &commands)
{
	std::string returnString = "";
	for(auto place : commands)
	{
		returnString = doCommand(place);
		if(returnString != "")
			return returnString;
	}

	return returnString;
}

bool GameController::parseFile(std::string fileName) //return false on error
{
	includedFiles.push_back(fileName);
	
	int lineNumber = 0;
	std::string line;
	std::ifstream myfile(fileName);
	
	bool inScenario = false;
	
	std::string scenarioName;
	std::string scenarioInfo;
	
	if (myfile.is_open())
		while(getline(myfile,line))
		{
			lineNumber++;
			line = removeWhiteSpace(line);
			
			if(line[0] == ';')
			{
				inScenario = true;
				scenarioInfo = "";
				scenarioName = removeWhiteSpace(line,'\n');
			}
			else if(inScenario)
				scenarioInfo += line + "\n";
			
			if(inScenario && removeWhiteSpace(line, '\n') == "end")
			{
				addStoryline(scenarioName,scenarioInfo);
				inScenario = false;
			}
			
		}
	
	return true;
}

std::string doRand(std::string line)
{
	std::vector<std::string> parts;
	stringsplit('|', line, parts);

	if(removeWhiteSpace(parts.at(parts.size()-1)) == "")
		parts.pop_back();

	if(parts.size() == 0)
		return "";
		
	int randnum = rand() % parts.size();
	
	return parts.at(randnum);
}

std::string GameController::doHealth(std::string command)
{
	std::string message = "";
	char type;
	std::string amount = "";
	
	bool inMessage = false;
	
	type = command[0];
	
	for(int place = 1; place < command.length(); place++)
	{
		if(command[place] == '|')
			inMessage = true;
		else if(not inMessage)
			amount += command[place];
		else
			message += command[place];
	}
	
	if(type == '+')
		health += stoi(amount);
	else if(type == '-')
		health -= stoi(amount);
	
	if(health <= 0)
		return message;
	
	return "";
}

std::string GameController::doCommand(Command* command)
{
	if(checkCondition(command->condition))
	{
		if(command->commandType == "item")
			addItem(command->command);
		else if(command->commandType == "endgame")
			return command->command;
		else if(command->commandType == "goto")
			return command->command;
		else if(command->commandType == "rand")
			return doRand(command->command);
		else if(command->commandType == "health")
			return doHealth(command->command);
		else if(command->commandType == "include")
			parseFile(command->command);
		else if(command->commandType == "info")
			std::cout << command->command << std::endl;
		//else if(command->commandType == "var")
			//shit is about to get real
	}
	
	return "";
}

bool GameController::addItem(std::string itemString)
{
	bool foundPlace = false;
	
	char operation = ' ';
	char operationChars[] = {'+', '-', '/', '*'};
	
	std::string itemName;
	std::string amount;
	
	//parse item name and amount
	
	for(int place = 0; place < itemString.length(); place++)
	{
		if(operation == ' ')
		{
			for(char checkOperation : operationChars)
				if(itemString[place] == checkOperation)
					operation = checkOperation;
					
			if(operation == ' ')
				itemName += itemString[place];
		}
		else
			amount += itemString[place];
	}
	
	for(int place = 0; place < items.size(); place++)
	{
		if(itemName == items.at(place))
		{
			switch(operation)
			{
				case '+':
					itemsAmount.at(place) += std::stoi(amount);
				break;
				case '-':
					itemsAmount.at(place) -= std::stoi(amount);
				break;
				case '/':
					itemsAmount.at(place) /= std::stoi(amount);
				break;
				case '*':
					itemsAmount.at(place) *= std::stoi(amount);
				break;
			}
			
			foundPlace = true;
			break;
		}
	}
	
	if(not foundPlace)
	{
		items.push_back(itemName);
		itemsAmount.push_back(std::stoi(amount));
	}
	
	return true;
}

bool GameController::parseLine(std::string lineText, std::string currentCondition, std::vector<Command*> &commands)
{
	std::string clearedText = removeWhiteSpace(lineText);
	
	std::string commandList[] = {"item:", "endgame:", "goto:", "use:", "rand:", "health:", "var:", "include:", "info:"};
	
	bool hasParsed = false;
	
	for(std::string command : commandList)
		if(clearedText.substr(0,command.size()) == command)
		{
			addCommand(clearedText.erase(0,command.size()+1), currentCondition, command.substr(0, command.size()-1), commands);
			hasParsed = true;
		}
	return hasParsed;
}

std::string GameController::getItems()
{
	std::string returnString = "";
	
	std::string addAnd = "and ";
	
	if(items.size() == 1)
		addAnd = "";
	
	for(int place = 0; place < items.size(); place++)
	{
		if(itemsAmount.at(place) > 1 || itemsAmount.at(place) == 0) {
			if(place == items.size()-1)
				returnString += addAnd + std::to_string(itemsAmount.at(place)) + " " + items.at(place) + "s.";
			else
				returnString += std::to_string(itemsAmount.at(place)) + " " + items.at(place) + "s, ";
		} else if(itemsAmount.at(place) == 0) {
			if(place == items.size()-1)
				returnString += addAnd + std::to_string(itemsAmount.at(place)) + " " + items.at(place) + ".";
			else
				returnString += std::to_string(itemsAmount.at(place)) + " " + items.at(place) + ", ";
		}
	}
	
	return returnString;
}

void GameController::addCommand(std::string command, std::string condition, std::string commandType, std::vector<Command*> &commands)
{
	commands.push_back(new Command(command, condition, commandType));
	return;
}
