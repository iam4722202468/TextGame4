#ifndef EXTRAS_H
#define EXTRAS_H

#include <string>
#include <vector>

void stringsplit(char splitat, std::string line, std::vector<std::string>& newtext);
std::string removeWhiteSpace(std::string str, char endChar = '\0');

#endif
