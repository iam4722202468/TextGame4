#include <vector>
#include <string>

#include "extras.h"

void stringsplit(char splitat, std::string line, std::vector<std::string>& newtext)
{
	std::string word = "";
	
	newtext.clear();
	
	for(int z = 0; z < line.size(); z++)
	{
		if(line[z] == splitat)
		{
			newtext.push_back(word);
			word = "";
		}
		else
			word += line[z];
	}
	
	newtext.push_back(word);
}

std::string removeWhiteSpace(std::string str, char endChar)
{
	for(int place = 0; place < str.size(); place++)
	{
		if(str[place] == ' ' || str[place] == '	')
		{
			str.erase(place, 1);
			place--;
		}
		else if((endChar != '\0' && str[place] == endChar) || endChar == '\0')
			break;
	}
	
	return str;
}
