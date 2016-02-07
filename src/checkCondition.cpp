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

std::string parseConditionBoolean(std::string condition)
{
	int inBracket;
	std::string bracketCondition;
	
	for(int place = 0; place < condition.length(); place++)
	{
		if(condition[place] == '(')
			inBracket = place;
		else if(condition[place] == ')')
		{ 
			bracketCondition = condition.substr(inBracket+1, place-inBracket-1);
			condition.erase(inBracket, place-inBracket+1);
			condition.insert(inBracket, parseConditionBoolean(bracketCondition));
			place -= place-inBracket+1 - inBracket;
		}
	}
	
	bool conditionBoolean;
	bool didAction;
	
	std::cout << "-> " << condition << std::endl;
	
	for(int place = 0; place < condition.length(); place++)
	{
		didAction = false;
		if(condition[place] == '&')
		{
			conditionBoolean = condition[place-1]-48 && condition[place+1]-48;
			didAction = true;
		}
		else if(condition[place] == '|')
		{
			conditionBoolean = condition[place-1]-48 || condition[place+1]-48;
			didAction = true;
		}
		
		if(didAction)
		{
			condition.erase(place-1, 3);
			
			if(conditionBoolean)
				condition.insert(place-1, "1");
			else
				condition.insert(place-1, "0");
		}
	}
	std::cout << condition << std::endl;
	return condition;
}

bool GameController::checkCondition(std::string condition)
{
	condition = removeWhiteSpace(condition, '\n');
	std::string checkFor[] = {"item", "life", "gameclock"};
	
	bool reply; //if it doesn't reply try emailing them to make sure they got the text
	
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
					
					reply = false;
					condition = condition.erase(place, itemString.length()+searchPlace.length()+2);
					
					if(searchPlace == "item") //add 0 and 1's here
						reply = checkConditionGeneric(&GameController::getItemAmount, itemString);
					if(searchPlace == "life")
						reply = checkConditionGeneric(&GameController::getLife, itemString);
					if(searchPlace == "gameclock")
						reply = checkConditionGeneric(&GameController::getGameClock, itemString);
					
					if(reply)
						condition.insert(place, "1");
					else
						condition.insert(place, "0");
				}
				
		return parseConditionBoolean(condition)[0]-48;
	}
}
