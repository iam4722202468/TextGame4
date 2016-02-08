#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <stdlib.h>

std::string char_to_string(char c)
{
	std::string s;
	s.push_back(c);
	return s;
}

int factorial(int n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

void search(std::string haystack, std::string needle, std::vector<int>& foundat)
{
	int place = -1;
	
	foundat.clear();
	
	while(true)
	{
		place = haystack.find(needle, place+1);
		if (place == haystack.npos)
			break;
		foundat.push_back(place);
	}

	return;
}
