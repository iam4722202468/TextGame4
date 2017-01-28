#include <string>
#include <vector>
#include <iostream>

#include "option.h"
#include "extras.h"

Option::Option(std::string data, int optionNumber_, std::string optionTitle_, std::string condition_, GameController *currentGame_) :
    optionNumber(optionNumber_),
    optionTitle(optionTitle_),
    condition(condition_),
    currentGame(currentGame_)
{
    std::vector<std::string> splitText;
    stringsplit('\n', data, splitText);
    
    std::string clearedText;
    std::string currentCondition = "";
    
    for(std::string lineText : splitText)
    {
        clearedText = removeWhiteSpace(lineText);
        
        if(clearedText.substr(0,3) == "if:")
            currentCondition = clearedText.erase(0,3);
        else if(clearedText.substr(0,5) == "endif")
            currentCondition = "";
        else if(clearedText != "")
            currentGame->parseLine(lineText, currentCondition, commands);
    }
    
    return;
}
