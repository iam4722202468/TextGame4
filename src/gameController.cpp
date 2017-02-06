#include <iostream>
#include <string>
#include <vector>
#include <fstream>

//for random numbers
#include <cstdlib>
#include <time.h>
#include <ctime>

#include <unistd.h>

#include "gameController.h"
#include "extras.h"
#include "calculate.h"

GameController::GameController(std::string sessionKey) :
    sessionKey(sessionKey)
{
    health = 100;
    gameClock = 0;
}

GameController::~GameController()
{
    for(auto i : storylines)
        delete i;
    
    storylines.clear();
}

int GameController::addStoryline(std::string storylineName, std::string storylineData)
{
    storylines.push_back(new Storyline(storylineData, storylineName, this));
    return 0;
}

bool GameController::sendInput(std::string input)
{
    std::string returnString;
    
    for(auto place : storylines)
        if(place->storylineName == storyline)
        {
            returnString = place->getOption(input, chooseFrom);
            if(returnString == "")
            {
                storyline[storyline.size()-1] = '.';
                storyline += std::to_string(stoi(input)+1) + ";";
                return 1;
            }
            else if(returnString[0] != ';')
            {
                std::cout << returnString << " " << sessionKey << std::endl;
                return 0;
            }
            else
            {
                storyline = returnString;
                return 1;
            }
        }
}

bool GameController::getOptions(std::vector<std::string> &toReturn)
{
    std::string returnString;
    
    for(auto place : storylines)
        if(place->storylineName == storyline)
        {
            returnString = doCommandBlock(place->commands);
            
            //returnString is from parsing only what is found in the storyline, not the options
            if(returnString == "")
            {
                place->displayOptions(chooseFrom, toReturn);
                
                //do macros
                for(int i = 0; i < toReturn.size(); ++i)
                    toReturn.at(i) = doMacros(toReturn.at(i));
                
                return 1;
            }
            else if(returnString[0] != ';')
            {
                std::cout << doMacros(returnString) << " " << sessionKey << std::endl;
                return 0;
            }
            else
            {
                storyline = returnString;
                toReturn.clear();
                
                //if a new storyline is found, change storylines and then parse the options for it
                return getOptions(toReturn);
            }
            
            break;
        }
    
    return 1;
}

std::string GameController::doCommandBlock(std::vector<Command*> &commands)
{
    std::string returnString = "";
    
    std::vector<Command*> checkedCommands;
    
    for(auto command : commands)
        if(checkCondition(command->condition))
            checkedCommands.push_back(command);
        
    for(auto place : checkedCommands)
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
    std::ifstream myfile("./games/" + fileName);
    
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
    {
        health += stoi(amount);
        std::cout << "+" << stoi(amount) << " health. (" << health << " now) " << sessionKey << std::endl;
        std::cout << "  " << sessionKey << std::endl;
    }
    else if(type == '-')
    {
        health -= stoi(amount);
        std::cout << "-" << stoi(amount) << " health. (" << health << " now) " << sessionKey << std::endl;
        std::cout << "  " << sessionKey << std::endl;
    }
    else if(type == '=')
    {
        health = stoi(amount);
        std::cout << "Health set to " << health << " " << sessionKey << std::endl;
        std::cout << "  " << sessionKey << std::endl;
    }
    
    if(health <= 0)
        return message;
    
    return "";
}

bool GameController::addVariable(std::string variableString)
{
    variableString = removeWhiteSpace(variableString, '\n');
    
    std::string variableName;
    std::string variableValue;
    bool found = false;
    
    for(int place = 0; place < variableString.length(); place++)
    {
        if(variableString[place] == '=')
            found = true;
        else if(found)
            variableValue += variableString[place];
        else
            variableName += variableString[place];
    }
    
    int variablePlace = -1;
    
    for(int place = 0; place < variables.size(); place++)
        if(variables.at(place) == variableName)
        {
            variablePlace = place;
            break;
        }
    
    if(variablePlace > -1)
        variablesValue.at(variablePlace) = variableValue;
    else
    {
        variables.push_back(variableName);
        variablesValue.push_back(variableValue);
    }
    
    return true;
}

std::string GameController::doMacros(std::string macroString)
{
    std::string commandList[] = {"$item(", "$storyline", "$health", "$var(", "$calc(", "$int("};
    int endBracket, inBracket, outBracket;
    
    int value;
    std::string valuestr;
    
    for(int place = 0; place < macroString.length(); place++)
        for(std::string command : commandList)
            if(macroString.substr(place,command.size()) == command)
            {                
                endBracket = -1;
                inBracket = 0;
                outBracket = 0;
                
                if(command[command.length()-1] == '(')
                    for(int subPlace = place; subPlace < macroString.length(); subPlace++)
                    {
                        if(macroString[subPlace] == ')')
                            outBracket++;
                        else if(macroString[subPlace] == '(')
                            inBracket++;
                        if(inBracket == outBracket && inBracket != 0)
                        {
                            endBracket = subPlace;
                            break;
                        }
                    }
                
                if(command == "$item(")
                {
                    value = getItemAmount(doMacros(macroString.substr(place+command.length(), endBracket-place-command.length())));
                    macroString.erase(place, endBracket-place+1);
                    macroString.insert(place, std::to_string(value));
                }
                else if(command == "$storyline")
                {
                    macroString.erase(place, command.length());
                    macroString.insert(place, storyline);
                }
                else if(command == "$health")
                {
                    macroString.erase(place, command.length());
                    macroString.insert(place, std::to_string(health));
                }
                else if(command == "$var(")
                {
                    valuestr = getVariableValue(doMacros(macroString.substr(place+command.length(), endBracket-place-command.length())));
                    macroString.erase(place, endBracket-place+1);
                    macroString.insert(place, valuestr);
                }
                else if(command == "$calc(")
                {
                    valuestr = solve(doMacros(macroString.substr(place+command.length(), endBracket-place-command.length())), 0);
                    macroString.erase(place, endBracket-place+1);
                    macroString.insert(place, valuestr);
                }
                else if(command == "$int(")
                {
                    value = stoi(doMacros(macroString.substr(place+command.length(), endBracket-place-command.length())));
                    macroString.erase(place, endBracket-place+1);
                    macroString.insert(place, std::to_string(value));
                }
                
                    
            }
    
    return macroString;
}

std::string GameController::doCommand(Command* command)
{
    
    std::string macroCommand;
    
    macroCommand = doMacros(command->command);
        
    if(command->commandType == "item")
        addItem(macroCommand);
    else if(command->commandType == "endgame")
        return macroCommand;
    else if(command->commandType == "goto")
        return macroCommand;
    else if(command->commandType == "rand")
        return doRand(macroCommand);
    else if(command->commandType == "health")
        return doHealth(macroCommand);
    else if(command->commandType == "include")
        parseFile(macroCommand);
    else if(command->commandType == "info")
        std::cout << macroCommand << " " << sessionKey << std::endl;
    else if(command->commandType == "var")
        addVariable(macroCommand);

    return "";
}

bool GameController::addItem(std::string itemString)
{
    bool foundPlace = false;
    
    char operation = ' ';
    char operationChars[] = {'+', '-', '/', '*', '='};
    
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
    
    int intAmount = std::stoi(amount);
    
    for(int place = 0; place < items.size(); place++)
    {
        if(itemName == items.at(place))
        {
            switch(operation)
            {
                case '+':
                {
                    itemsAmount.at(place) += intAmount;
                    std::cout << itemName << " + " << amount << " (" << itemsAmount.at(place) << " now) " << sessionKey << std::endl;
                    std::cout << "  " << sessionKey << std::endl;
                }
                break;
                case '-':
                {
                    itemsAmount.at(place) -= intAmount;
                    std::cout << itemName << " - " << amount << " (" << itemsAmount.at(place) << " now) " << sessionKey << std::endl;
                    std::cout << "  " << sessionKey << std::endl;
                }
                break;
                case '/':
                    itemsAmount.at(place) /= intAmount;
                break;
                case '*':
                    itemsAmount.at(place) *= intAmount;
                break;
                case '=':
                    itemsAmount.at(place) = intAmount;
                break;
            }
            
            foundPlace = true;
            break;
        }
    }
    
    if(not foundPlace)
    {
        std::cout << itemName << " + " << amount << " (" << amount << " now) " << sessionKey << std::endl;
        std::cout << "  " << sessionKey << std::endl;
        
        items.push_back(itemName);
        itemsAmount.push_back(intAmount);
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
        } else if(itemsAmount.at(place) == 1) {
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
