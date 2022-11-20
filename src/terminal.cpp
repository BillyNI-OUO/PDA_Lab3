#include "terminal.h"
#include <iostream>
#include <string>
#include <algorithm>
Terminal::Terminal(int lx, int ly, int width, int height, int index){
	this->width = width;
	this->height = height;
    this->lx = lx;
    this->ly = ly;
	this->index = index;
}

void Terminal::Swap(){
	int temp = this->height;
	this->height = this->width;
	this->width = temp;
}