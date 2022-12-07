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
#include "./src/row.h"
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
vector<vector<Row>> rows;
vector<vector<Row>> tempRows;
vector<Block> sortBlocks;
vector<Block> placeBlock;
void parse(char* fileName);
bool compareBlock(Block a, Block b);
bool intersect(Block a, Block b);
int overlapArea(Block a, Block b);
int blockArea(int index);
void output(char *fileName);
long totalArea = 0;
long leftArea = 0;
void buildGraph();
void FM();
void partitionInit();
void sliceRow();
int placeRow(int rowIndex, int rowSecondIndex, int blockIndex, bool flag);
int caculateCost(int rowIndex, int rowSecondIndex);
void Abacus(int flag);
void AddCell(cluster &C, int blockIndex);
void AddCluster(cluster &C, cluster &C_);
void Collapse(vector<cluster>& clusters, int lx, int width, int rowIndex, int rowSecondIndex);
int main(int argc, char** argv){
    parse(argv[1]);
    sortBlocks = blocks;
    placeBlock = blocks;
    sort(sortBlocks.begin(), sortBlocks.end(), compareBlock);
    buildGraph();
    partitionInit();
    FM();
    
    sliceRow();
    tempRows = rows;
    Abacus(0);
    rows = tempRows;    
    Abacus(1);
    output(argv[2]);
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
	
    rows.resize(rowSize);
    for(int i = 0; i < rowSize; i ++){
        rows[i].push_back(Row(0, rowHeight*i, outLineWidth));
    }

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

void sliceRow(){
    for(int i = 0; i < terminalSize; i ++){
        Terminal terminal = terminals[i];
        for(int j = terminal.ly/rowHeight; j <= (terminal.ly+terminal.height)/rowHeight; j ++){
            for(int k = 0; k < rows[j].size(); k ++){
                if(rows[j][k].lx <= terminal.lx && rows[j][k].lx+rows[j][k].width > terminal.lx){
                    Row originRow = rows[j][k];
                    Row leftRow = Row(originRow.lx, originRow.ly, terminal.lx-originRow.lx);
                    Row rightRow = Row(terminal.lx+terminal.width, originRow.ly, originRow.lx+originRow.width-terminal.lx-terminal.width);
                    
                    
                    rows[j].erase(rows[j].begin() + k);
                    
                    if(rightRow.width > 0)
                        rows[j].insert(rows[j].begin()+k, rightRow);
                    if(leftRow.width > 0)  
                        rows[j].insert(rows[j].begin()+k, leftRow);
                }
                else if(rows[j][k].lx < terminal.lx+terminal.width && rows[j][k].lx+rows[j][k].width > terminal.lx+terminal.width){
                    rows[j][k].width = rows[j][k].lx+rows[j][k].width - (terminal.lx+terminal.width);
                    rows[j][k].lx = terminal.lx+terminal.width;
                    
                    if(rows[j][k].width <= 0){
                        rows[j].erase(rows[j].begin() + k);
                    }
                }
            }
        }
    }
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
            if(sortBlocks[i].lx+sortBlocks[i].width < sortBlocks[j].lx)break;
            if(intersect(sortBlocks[i], sortBlocks[j])){
                Vertices newVerticesA(sortBlocks[i].index, maxBlockArea-overlapArea(sortBlocks[i], sortBlocks[j]));
                Vertices newVerticesB(sortBlocks[j].index, maxBlockArea-overlapArea(sortBlocks[i], sortBlocks[j]));
                graph[sortBlocks[i].index].push_back(newVerticesB);
                graph[sortBlocks[j].index].push_back(newVerticesA);
                if ( maxOverlapArea < maxBlockArea-overlapArea(sortBlocks[i], sortBlocks[j])){
                    maxOverlapArea = maxBlockArea-overlapArea(sortBlocks[i], sortBlocks[j]);
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
    int ratio = maxOverlapArea/200;
    ratio = ratio > 1 ? ratio : 1;
    int pMax = maxOverlapArea/ratio;
    pMax*= (maxOverlapCount+5);
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
        bool flag = false;
        for(int i = 2*pMax; i >= 0; i --){
            if(!freeVertices[i].empty()){
                for(int j = freeVertices[i].size()-1; j >= 0; j --){
                    int vertiex = freeVertices[i][j];
                    
                    if(partitionMap[vertiex] == 0){
                        if(leftArea+blockArea(vertiex) > totalArea*3/4){
                            continue;
                        }else{
                            leftArea+=blockArea(vertiex);
                        }
                    }else{
                        if(leftArea-blockArea(vertiex) < totalArea/4){
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
                    flag = true;
                    break;
                }
            }
            if(flag){
                break;
            }
        }
    }
}

void output(char *fileName){
    fstream fouput;
	fouput.open(fileName, ios::out);
    for(int i = 0; i < blockSize; i ++){
        fouput << "C" << i << " " << placeBlock[i].lx << " " << placeBlock[i].ly << " " << bestPartitionMap[i] << endl;
    }
    fouput.close();
}


void Abacus(int flag){
    for(int i = 0; i < blockSize; i ++){
        int costBest = INT32_MAX;
        pair<int, int> bestRow;
        Block curBlock = sortBlocks[i];
        if(bestPartitionMap[curBlock.index] != flag){
            continue;
        }
        bool topFlag = true;
        bool botFlag = true;
        int preBotCost = INT32_MAX;
        int preTopCost = INT32_MAX;
        for(int j = 0; j < rowSize; j ++){
            int top = curBlock.ly/rowHeight + j;
            int bot = curBlock.ly/rowHeight - j;

            if(top < rowSize && topFlag){
                int preCost = INT32_MAX;
                int bestTopCost = INT32_MAX;
                for(int k = 0; k < rows[top].size(); k ++){
                    int tempLx = blocks[curBlock.index].lx;
                    if(blocks[curBlock.index].lx < rows[top][k].lx){
                        blocks[curBlock.index].lx = rows[top][k].lx;
                    }
                    if (blocks[curBlock.index].lx + blocks[curBlock.index].width > rows[top][k].lx + rows[top][k].width) {
                        blocks[curBlock.index].lx  = rows[top][k].lx + rows[top][k].width - blocks[curBlock.index].width;
                    }

                    int cost = placeRow(top, k, curBlock.index, false);
                    
                    if(cost < costBest){
                        costBest = cost;
                        bestRow.first = top;
                        bestRow.second = k;
                    }
                    if(cost < bestTopCost){
                        bestTopCost = cost;
                    }
                    rows[top][k].cells.pop_back();
                    // if(preCost < cost){
                    //     break;
                    // }
                    blocks[curBlock.index].lx = tempLx;
                }
                // if(bestTopCost > preTopCost){
                //     topFlag = false;
                // }
            }

            if(bot >= 0 && botFlag){
                int preCost = INT32_MAX;
                int bestBotCost = INT32_MAX;
                for(int k = 0; k < rows[bot].size(); k ++){
                    int tempLx = blocks[curBlock.index].lx;
                    if(blocks[curBlock.index].lx < rows[bot][k].lx){
                        blocks[curBlock.index].lx = rows[bot][k].lx;
                    }
                    if (blocks[curBlock.index].lx + blocks[curBlock.index].width > rows[bot][k].lx + rows[bot][k].width) {
                        blocks[curBlock.index].lx  = rows[bot][k].lx + rows[bot][k].width - blocks[curBlock.index].width;
                    }


                    int cost = placeRow(bot, k, curBlock.index, false);
                    
                    if(cost < costBest){
                        costBest = cost;
                        bestRow.first = bot;
                        bestRow.second = k;
                    }
                    if(cost < bestBotCost){
                        bestBotCost = cost;
                    }
                    rows[bot][k].cells.pop_back();
                    // if(preCost < cost){
                    //     break;
                    // }
                    blocks[curBlock.index].lx = tempLx;
                }
                // if(bestBotCost > preBotCost){
                //     botFlag = false;
                // }
            }
        }
        if(blocks[curBlock.index].lx < rows[bestRow.first][bestRow.second].lx){
            blocks[curBlock.index].lx = rows[bestRow.first][bestRow.second].lx;
        }
        if (blocks[curBlock.index].lx + blocks[curBlock.index].width > rows[bestRow.first][bestRow.second].lx + rows[bestRow.first][bestRow.second].width) {
            blocks[curBlock.index].lx  = rows[bestRow.first][bestRow.second].lx + rows[bestRow.first][bestRow.second].width - blocks[curBlock.index].width;
        }


        int temp = placeRow(bestRow.first, bestRow.second, curBlock.index, true);
        rows[bestRow.first][bestRow.second].totalWidth += blocks[curBlock.index].width;
    }
}

int placeRow(int rowIndex, int rowSecondIndex, int blockIndex, bool flag){
    
    rows[rowIndex][rowSecondIndex].cells.push_back(blockIndex);
    if(rows[rowIndex][rowSecondIndex].totalWidth + blocks[blockIndex].width > rows[rowIndex][rowSecondIndex].width){
        return INT32_MAX;
    }
    vector<cluster>c;
    for(int i = 0; i < rows[rowIndex][rowSecondIndex].cells.size(); i ++){
        int curBlockIndex = rows[rowIndex][rowSecondIndex].cells[i];
        if(!c.size() ||c.back().lx + c.back().width < blocks[curBlockIndex].lx){
            cluster newCluster;
            newCluster.width = 0;
            newCluster.weight = 0;
            newCluster.quarditac = 0;
            newCluster.lx = blocks[curBlockIndex].lx;
            newCluster.firstCell = blocks[curBlockIndex].index;
            AddCell(newCluster, curBlockIndex);
            c.push_back(newCluster);
        }else{
           AddCell(c.back(), curBlockIndex); 
           Collapse(c, rows[rowIndex][rowSecondIndex].lx, rows[rowIndex][rowSecondIndex].width, rowIndex, rowSecondIndex);
        }
    }
    if(flag){
        int cellIndex = 0;
        for(int i = 0; i < c.size(); i ++){
            
            int X = c[i].lx;
            
            while(cellIndex < rows[rowIndex][rowSecondIndex].cells.size()){
                placeBlock[rows[rowIndex][rowSecondIndex].cells[cellIndex]].lx = X;
                placeBlock[rows[rowIndex][rowSecondIndex].cells[cellIndex]].ly = rowIndex * rowHeight;
                X += blocks[rows[rowIndex][rowSecondIndex].cells[cellIndex]].width;
                if(rows[rowIndex][rowSecondIndex].cells[cellIndex] == c[i].lastCell){
                    cellIndex++;
                    break;
                }
                cellIndex ++;
            }
        }
    }
    //return (placeBlock[blockIndex].lx - blocks[blockIndex].lx)*(placeBlock[blockIndex].lx - blocks[blockIndex].lx) + (placeBlock[blockIndex].ly - blocks[blockIndex].ly)*(placeBlock[blockIndex].ly - blocks[blockIndex].ly);
    //return caculateCost(rowIndex, rowSecondIndex);
    return  max(blocks[blockIndex].lx, placeBlock[blockIndex].lx) - min(blocks[blockIndex].lx, placeBlock[blockIndex].lx) + max(blocks[blockIndex].ly, placeBlock[blockIndex].ly) - min(blocks[blockIndex].ly, placeBlock[blockIndex].ly);
}

void AddCell(cluster &C, int blockIndex){
    C.lastCell = blockIndex;
    C.weight += 1;
    C.quarditac +=  1*(blocks[blockIndex].lx - C.width);
    C.width += blocks[blockIndex].width;
}

void AddCluster(cluster &C, cluster &C_){
    C.lastCell = C_.lastCell;
    C.weight += C_.weight;
    C.quarditac += C_.quarditac - C_.weight*C.width;
    C.width += C_.width;
}

void Collapse(vector<cluster> &Clusters, int lx, int width, int rowIndex, int rowSecondIndex){
    // cluster C  = Clusters.back();
    // C.lx = 
    // if(C.lx < lx)C.lx = lx;
    // if(C.lx > lx+width-C.width)C.lx = lx+width-C.width;
    // if(Clusters.size() > 1){
    //     cluster PC = Clusters[Clusters.size()-2];
    //     if (PC.lx+PC.width > C.lx){
    //         AddCluster(PC, C);
    //         Clusters.pop_back();
    //         Collapse(Clusters, lx, width, rowIndex, rowSecondIndex);
    //     }
    // }

    while(1){
        Clusters.back().lx = Clusters.back().quarditac/Clusters.back().weight;
        if (Clusters.back().lx < rows[rowIndex][rowSecondIndex].lx) {
            Clusters.back().lx = rows[rowIndex][rowSecondIndex].lx;
        }
        if (Clusters.back().lx + Clusters.back().width > rows[rowIndex][rowSecondIndex].lx+rows[rowIndex][rowSecondIndex].width) {
            Clusters.back().lx = rows[rowIndex][rowSecondIndex].lx+rows[rowIndex][rowSecondIndex].width - Clusters.back().width;
        }
        if (Clusters.size() == 1) {
            break;
        }
        else {
            if (Clusters[Clusters.size()-2].lx + Clusters[Clusters.size()-2].width >= Clusters.back().lx) {
                AddCluster(Clusters[Clusters.size()-2], Clusters[Clusters.size()-1]);
                Clusters.pop_back();
            }
            else {
                break;
            }
        }
    }
}   


int caculateCost(int rowIndex, int rowSecondIndex){
    int totalCost = 0;
    for(int i = 0; i < rows[rowIndex][rowSecondIndex].cells.size(); i ++){
        int blockIndex =  rows[rowIndex][rowSecondIndex].cells[i];
        totalCost += max(blocks[blockIndex].lx, placeBlock[blockIndex].lx) - min(blocks[blockIndex].lx, placeBlock[blockIndex].lx);
        totalCost += max(blocks[blockIndex].ly, placeBlock[blockIndex].ly) - min(blocks[blockIndex].ly, placeBlock[blockIndex].ly);
    }
    
    return totalCost;
}