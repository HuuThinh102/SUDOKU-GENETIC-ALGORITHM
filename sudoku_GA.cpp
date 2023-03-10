#include <iostream>
#include <vector>
#include <cmath>
#include <set>


#define AREA_SQUARE_SIZE 9
#define NUM_BOX 9
#define NUM_ROW 9
#define NUM_COLUMN 9

using namespace std;


struct Coord{
    int x,y;
};

//Cấu trúc 1 ô
struct Cell{
    Coord coord;
    int value; // giá trị trong ô
    bool available; // kiểm tra xem ô đó có phải là ô chứa giá trị có sẵn của đề ko
};

//Cấu trúc khối gồm nhiều ô trong đó
struct Box{
    Cell cells[AREA_SQUARE_SIZE];
};

//Cấu trúc Sudoku gồm có số lượng các khối, độ thích nghi, và 1 cái bảng để điền giá trị
struct Sudoku{
    Box boxs[NUM_BOX];
    int fitness;// Độ thích nghi
    int table[NUM_ROW][NUM_COLUMN];
};



int main(){
    
}