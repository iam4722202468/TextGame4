#include <string>
#include <vector>
#include <iostream>
#include <unistd.h>

#include "storyline.h"
#include "extras.h"
#include "option.h"

Storyline::Storyline(std::string data_, std::string storylineName_, GameController *currentGame_):
    currentGame(currentGame_),
    data(data_),
    storylineName(storylineName_)
{
    std::vector<std::string> splitText;
    stringsplit('\n', data, splitText);
    
    std::string clearedText;
    
    bool inOption = false;
    std::string optionString = "";
    std::string optionTitle = "";
    std::string currentCondition = "";
    
    for(std::string lineText : splitText)
    {
        clearedText = removeWhiteSpace(lineText);
        
        if(clearedText.substr(0,5) == "text:")
            text = clearedText.erase(0,6);
        else if(clearedText.substr(0,7) == "option:")
        {
            if(inOption)
                options.push_back(new Option(optionString, options.size(), optionTitle, currentCondition, currentGame));
            
            optionString = "";
            optionTitle = removeWhiteSpace(lineText, ':').erase(0,8);
            inOption = true;
        }
        else if(clearedText.substr(0,9) == "endoption")
        {
            options.push_back(new Option(optionString, options.size(), optionTitle, currentCondition, currentGame));
            inOption = false;
        }
        else if(clearedText.substr(0,3) == "if:")
            currentCondition = clearedText.erase(0,3);
        else if(clearedText.substr(0,5) == "endif")
            currentCondition = "";
        else if(not inOption && clearedText != "")
            currentGame->parseLine(lineText, currentCondition, commands); //command ran when option is chosen
        else
            optionString += lineText + "\n";
    }
    
    return;
}

void Storyline::displayOptions(std::vector<Option*> &chooseFrom, std::vector<std::string> &returnOptions) //return amount of options
{
    chooseFrom.clear();
    returnOptions.clear();
    
    returnOptions.push_back(text);
    
    for(int counter = 0; counter < options.size(); counter++)
    {
        if(currentGame->checkCondition(options.at(counter)->condition))
        {
            returnOptions.push_back("    " + std::to_string(chooseFrom.size()+1) + ": " + options.at(counter)->optionTitle);
            chooseFrom.push_back(options.at(counter));
        }
    }
    
    return;
}

std::string Storyline::getOption(std::string userInput, std::vector<Option*> &chooseFrom)
{
    int chosenInt = -1;
    
    try {
        chosenInt = stoi(userInput);
    } catch(...) {
        chosenInt = -1;
    }
    
    return currentGame->doCommandBlock(chooseFrom.at(chosenInt)->commands); //do commands for option
}
