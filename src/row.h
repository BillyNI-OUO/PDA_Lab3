#ifndef ROW_H
#define ROW_H 

#include "terminal.h"
#include "block.h"
#include <utility>
#include <iostream>
#include <string>
#include <vector>

typedef struct{
    int firstCell;
    int lastCell;
    int cellNumber;
    int lx;
    int width;
    int quarditac;
    int weight = 1;
    std::vector<int> cells;
}cluster;

class Row{
    
	public:
		std::vector<int> cells;
        int cellsNum;
        int lx, ly;
        int width;
        int totalWidth;
        Row(int lx, int ly, int width){
            this->lx = lx;
            this->ly = ly;
            this->width = width;
            this->totalWidth = 0;
        }
        void PrintContent(){
            std::cout << "lx: " << this->lx <<  " ly: " << this->ly << " width: " << this->width << std::endl;
        }
        std::vector<cluster> clusters;
        cluster lastCluster;
};

#endif