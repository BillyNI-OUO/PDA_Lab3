#ifndef BLOCK_H
#define BLOCK_H 

#include <utility>
#include <iostream>
#include <string>

class Block{
	public:
		int width, height;
        int lx, ly;
		std::string name;
		int index;
		Block (int lx, int ly, int width, int height, int index);
		Block(){};
		void Swap();
		void PrintContent(){
            std::cout << "lowerX: " << lx << std::endl;
            std::cout << "lowerY: " << ly << std::endl;
			std::cout << "width: " << width << std::endl;
			std::cout << "height: " << height << std::endl;
			std::cout << "index: " << index << std::endl;
		};
};

#endif