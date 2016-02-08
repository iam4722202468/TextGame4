#ifndef EXTRAS_H
#define EXTRAS_H

#include <string>
#include <vector>

void stringsplit(char splitat, std::string line, std::vector<std::string>& newtext);
std::string removeWhiteSpace(std::string str, char endChar = '\0');
int factorial(int number);
void search(std::string haystack, std::string needle, std::vector<int>& foundat);
#endif
