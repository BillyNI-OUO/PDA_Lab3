#ifndef IO_H
#define IO_H 

#include "terminal.h"
#include "block.h"
#include <utility>
#include <iostream>
#include <string>
#include <vector>

class IO{
	public:
		char *intputFileName;
        char *outputFileName; 
        void intput(std::vector<Terminal>&terminals, std::vector<Block>&blocks, int max);
};

#endif