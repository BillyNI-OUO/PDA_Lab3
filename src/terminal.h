#ifndef TERMINAL_H
#define TERMINAL_H 

#include <utility>
#include <iostream>
#include <string>

class Terminal{
	public:
		int width, height;
        int lx, ly;
		int index;
		Terminal (int lx, int ly, int width, int height, int index);
		Terminal(){};
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