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

int Storyline::displayOptions(std::vector<Option*> &chooseFrom) //return amount of options
{
	std::cout << std::endl << text << std::endl;
	
	for(int counter = 0; counter < options.size(); counter++)
	{
		if(currentGame->checkCondition(options.at(counter)->condition))
		{
			std::cout << "	" << chooseFrom.size()+1 << ": " << options.at(counter)->optionTitle << std::endl;
			chooseFrom.push_back(options.at(counter));
		}
	}
}

int Storyline::getresponse(int options)
{
	int chosenint = -1;
	std::string chosenintString;
	
	while (chosenint < 1 || chosenint > options)
	{
		usleep(500);
		if (!std::cin) 
		{
			std::cin.clear();
			std::cin.ignore(256, '\n');
		}
		std::getline(std::cin,chosenintString);
		
		try {
			chosenint = stoi(chosenintString);
		} catch(...) {
			chosenint = -1;
		}
		
		std::cin.clear();
		
		if(chosenint == 0)
		{
			if(currentGame->getItems() == "")
				std::cout << "You currently have no items\n";
			else
				std::cout << "You have: " << currentGame->getItems() << std::endl;
			std::cout << "Your health is currently " << currentGame->health << std::endl;
		}
	}
	
	return chosenint-1;
}

std::string Storyline::getOption()
{
	currentGame->gameClock++;
	std::vector<Option*> chooseFrom;
	
	std::cout << "\n";
	currentGame->doCommandBlock(commands); //do commands for storyline
	displayOptions(chooseFrom);
	
	return currentGame->doCommandBlock(chooseFrom.at(getresponse(chooseFrom.size()))->commands); //do commands for option
}
