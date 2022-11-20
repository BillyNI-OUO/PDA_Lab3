#ifndef VERTICES_H
#define VERTICES_H 

#include <utility>
#include <iostream>
#include <string>

class Vertices{
	public:
        int weight;
		int index;
        Vertices *next;
		Vertices (int index, int weight);
		Vertices(){};
		void PrintContent(){
			std::cout << "weight: " << weight << std::endl;
			std::cout << "index: " << index << std::endl;
		};
};

#endif