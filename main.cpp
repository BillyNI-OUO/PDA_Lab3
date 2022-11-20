#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <random>
#include <climits>
#include <algorithm>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>

#include "./src/block.cpp"
#include "./src/terminal.cpp"
#include "./src/vertices.cpp"
using namespace std;
int outLineWidth, outLineHeight, rowHeight, rowSize;
int blockSize, terminalSize;
int maxBlockArea = 0;
int maxOverlapArea = 0;
int maxOverlapCount = 0;
int maxGain = 0;
int sumGain = 0;
vector<Block> blocks;
vector<Terminal> terminals;
vector<vector<Vertices>> graph;
vector<int> partitionMap;
vector<int> bestPartitionMap;
vector<vector<int>> freeVertices;
vector<int> verticesMapGain;
void parse(char* fileName);
bool compareBlock(Block a, Block b);
bool intersect(Block a, Block b);
int overlapArea(Block a, Block b);
int blockArea(int index);
long totalArea = 0;
long leftArea = 0;
void buildGraph();
void FM();
void partitionInit();
int main(int argc, char** argv){
    parse(argv[1]);
    sort(blocks.begin(), blocks.end(), compareBlock);
    buildGraph();
    partitionInit();
    for(auto i: partitionMap){
        cout << i;
    }
    cout << endl;
    FM();
    for(auto i: bestPartitionMap){
        cout << i;
    }
}


bool compareBlock(Block a, Block b){
    if(a.lx != b.lx){
        return (a.lx < b.lx);
    }else{
        return (a.ly < b.ly);
    }
}
void parse(char* fileName){
    ifstream finput(fileName, ios::in);
	string useless;

	finput >> useless >> outLineWidth >> outLineHeight;
    finput >> useless >> rowHeight >> rowSize;
	
	finput >> useless >> terminalSize;
    terminals.resize(terminalSize);
	for(int i = 0; i < terminalSize; i++){
		string name;
		int width, height, lx, ly;
		finput >> useless;
		finput >> lx >> ly >>width >> height;
		terminals[i] = Terminal(lx, ly, width, height, i);
	}
    
    finput >> useless >> blockSize;
	blocks.resize(blockSize);
	for(int i = 0; i < blockSize; i++){
		string name;
		int width, height, lx, ly;
		finput >> name >> lx >> ly >> width >> height;
		blocks[i] = Block(lx, ly, width, height, i);
        totalArea += width*height;
        if (maxBlockArea < width*height){
            maxBlockArea = width*height;
        }
	}
	finput.close();
}

bool intersect(Block a, Block b){
    return (a.lx < b.lx && (a.lx+a.width) > b.lx && a.ly < b.ly && (a.ly + a.height) > b.ly); 
}

int overlapArea(Block a, Block b){
    int x = min(a.lx+a.width, b.lx+b.width)- max(a.lx, b.lx);
    int y = min(a.ly+a.height, b.ly+b.height) - max(a.ly, b.ly);
    return x * y;
}

void buildGraph(){
    graph.assign(blockSize, {});
    for(int i = 0; i < blockSize; i ++){
        for(int j = i + 1; j < blockSize; j ++){
            if(blocks[i].lx+blocks[i].width < blocks[j].lx)break;
            if(intersect(blocks[i], blocks[j])){
                Vertices newVerticesA(i, maxBlockArea-overlapArea(blocks[i], blocks[j]));
                Vertices newVerticesB(j, maxBlockArea-overlapArea(blocks[i], blocks[j]));
                graph[i].push_back(newVerticesB);
                graph[j].push_back(newVerticesA);
                if ( maxOverlapArea < maxBlockArea-overlapArea(blocks[i], blocks[j])){
                    maxOverlapArea = maxBlockArea-overlapArea(blocks[i], blocks[j]);
                }
                if (maxOverlapCount < max(graph[i].size(), graph[j].size())){
                    maxOverlapCount = max(graph[i].size(), graph[j].size());
                }
            }
            
        }
    }
}
void partitionInit(){
    partitionMap.clear();
    partitionMap.resize(blockSize);
    bestPartitionMap.resize(blockSize);
    for(int i = 0 ; i < blockSize; i ++){
        partitionMap[i] = rand( ) % 2;
        
        leftArea += blocks[i].width*blocks[i].height*partitionMap[i];
    }
    bestPartitionMap = partitionMap;
}
int blockArea(int index){
    return blocks[index].width*blocks[index].height;
}
void FM(){
    int ratio = maxOverlapArea/5;
    ratio = ratio > 1 ? ratio : 1;
    int pMax = maxOverlapArea/ratio*maxOverlapCount;

    freeVertices.resize(2*pMax+1);
    verticesMapGain.resize(blockSize);
    for(int i = 0; i < blockSize; i ++){
        int gain = 0;
        for(auto j:graph[i]){
            if(partitionMap[i] == partitionMap[j.index]){
                gain -= j.weight/ratio;
            }else{
                gain += j.weight/ratio;
            }
        }
        freeVertices[gain+pMax].push_back(i);
        verticesMapGain[i] = gain+pMax;
    }

    int cnt = blockSize;
    while(cnt--){
        for(int i = 2*pMax; i >= 0; i --){
            if(!freeVertices[i].empty()){
                for(int j = freeVertices[i].size()-1; j >= 0; j --){
                    int vertiex = freeVertices[i][j];
                    
                    if(partitionMap[vertiex]){
                        if(leftArea+blockArea(vertiex) > totalArea*9/10){
                            continue;
                        }else{
                            leftArea+=blockArea(vertiex);
                        }
                    }else{
                        if(leftArea-blockArea(vertiex) < totalArea/10){
                            continue;
                        }else{
                            leftArea-=blockArea(vertiex);
                        }
                    }
                    freeVertices[i].erase(freeVertices[i].begin()+j);
                    
                    for(int k = 0; k < graph[vertiex].size(); k ++){ 
                        int neighbor = graph[vertiex][k].index;
                        int weight = graph[vertiex][k].weight;
                        if(verticesMapGain[neighbor] != -1){
                            int originalGain = verticesMapGain[neighbor];
                            std::vector<int>::iterator position = std::find(freeVertices[originalGain].begin(), freeVertices[originalGain].end(), neighbor);
                            if (position != freeVertices[originalGain].end()) // == myVector.end() means the element was not found
                                freeVertices[originalGain].erase(position);
                            
                            if(partitionMap[neighbor] == partitionMap[vertiex]){                             
                                verticesMapGain[neighbor] -= weight/ratio;
                                freeVertices[verticesMapGain[neighbor]].push_back(neighbor);
                            }else{
                                verticesMapGain[neighbor] += weight/ratio;
                                freeVertices[verticesMapGain[neighbor]].push_back(neighbor);
                            }
                        }
                    
                    }
                    
                    sumGain += i-pMax;
                    verticesMapGain[vertiex] = -1; //lock
                    partitionMap[vertiex] = 1-partitionMap[vertiex]; // change side
                    if(sumGain > maxGain){
                        bestPartitionMap = partitionMap;
                        maxGain = sumGain;
                    }
                    break;
                }
            }
        }
    }

}