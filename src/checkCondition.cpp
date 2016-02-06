#include <iostream>
#include <string>
#include <vector>

#include "gameController.h"
#include "extras.h"

int GameController::getItemAmount(std::string itemName)
{
	for(int place = 0; place < items.size(); place++)
		if(itemName == items.at(place))
			return itemsAmount.at(place);
	return 0;
}

bool GameController::checkConditionGeneric(int (GameController::*getValueFunction)(std::string toSend), std::string itemString)
{
	std::string operations[] = {"<=", ">=", "==", "!=", "<", ">"};
	
	std::string name = "";
	std::string value = "";
	std::string currentOperation = "";
	
	for(int findOperation = 0; findOperation < itemString.length(); findOperation++)
	{
		for(std::string operation : operations)
			if(itemString.substr(findOperation, operation.length()) == operation && currentOperation == "")
			{
				currentOperation = operation;
				findOperation += operation.length();
			}
	
	if(currentOperation == "")
		name += itemString[findOperation];
	else
		value += itemString[findOperation];
	}
	
	if(currentOperation == ">" && (this->*getValueFunction)(name) > stoi(value))
		return true;
	else if(currentOperation == "!=" && (this->*getValueFunction)(name) != stoi(value))
		return true;
	else if(currentOperation == "==" && (this->*getValueFunction)(name) == stoi(value))
		return true;
	else if(currentOperation == "<" && (this->*getValueFunction)(name) < stoi(value))
		return true;
	else if(currentOperation == "<=" && (this->*getValueFunction)(name) <= stoi(value))
		return true;
	else if(currentOperation == ">=" && (this->*getValueFunction)(name) >= stoi(value))
		return true;
	return false;
}

bool GameController::checkCondition(std::string condition)
{
	condition = removeWhiteSpace(condition, '\n');
	
	std::string checkFor[] = {"item", "life", "gameclock"};
	
	if(condition == "")
		return true;
	else
	{
		for(int place = 0; place < condition.length(); place++)
			for(std::string searchPlace : checkFor)
				if(condition.substr(place, searchPlace.length()) == searchPlace)
				{
					std::string itemString = ""; //contains specific info to test
					
					for(int findEnd = place+searchPlace.length()+1; findEnd < condition.length(); findEnd++)
					{
						if(condition[findEnd] != ')')
							itemString += condition[findEnd];
						else
							break;
					}
					if(searchPlace == "item") //add 0 and 1's here
						if(checkConditionGeneric(&GameController::getItemAmount, itemString))
						{
							return true;
						}
					if(searchPlace == "life")
						if(checkConditionGeneric(&GameController::getLife, itemString))
						{
							return true;
						}
					if(searchPlace == "gameclock")
						if(checkConditionGeneric(&GameController::getGameClock, itemString))
						{
							return true;
						}
				}
		
		return false;
	}
}
