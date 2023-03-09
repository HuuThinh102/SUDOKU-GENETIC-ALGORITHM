#include <stdio.h>
#include<stdlib.h>

#define MAX_LENGTH 900
#define NB_ROWS 9
#define NB_COLS 9
#define MAX_VALUE 10
#define EMPTY 0
#define AREA_SQUARE_SIZE 3
#define INF 99999

typedef struct{
	int x,y;
}Coord;

typedef struct{
	Coord dataListCoord[MAX_LENGTH];
	int size;
}ListCoord;

void initListCoord(ListCoord *list){
	list->size = 0;
}

void appendListCoord(ListCoord *list, Coord coord){
	list->dataListCoord[list->size++] = coord;
}

typedef struct{
	int dataConstrains[NB_ROWS*NB_COLS][NB_COLS*NB_ROWS];
	int n;
}Constrains;

void initConstrains(Constrains *constrains){
	int i, j;
	for(i=0; i<NB_ROWS*NB_COLS; i++)
		for(j=0; j<NB_ROWS*NB_COLS; j++)
			constrains->dataConstrains[i][j] = 0;
	constrains->n = NB_ROWS*NB_COLS;
}

void clearConstrains(Constrains *constrains){
	int i, j;
	for(i=0; i<NB_ROWS*NB_COLS; i++)
		for(j=0; j<NB_ROWS*NB_COLS; j++)
			constrains->dataConstrains[i][j] = 0;
	constrains->n = NB_ROWS*NB_COLS;
}


int indexOf(Coord coord){
	return (NB_ROWS*coord.x + coord.y);
}

Coord positionOfVertex(int vertex){
	Coord coord;
	coord.x = vertex / NB_ROWS;
	coord.y = vertex % NB_COLS;
	return coord;
}

int addConstrains(Constrains *constrains, Coord source, Coord target){
	int u = indexOf(source);
	int v = indexOf(target);
	if(constrains->dataConstrains[u][v] == 0){
		constrains->dataConstrains[u][v] = 1;
		constrains->dataConstrains[v][u] = 1;
		return 1;
	}
	return 0;
}

ListCoord getConstrains(Constrains constrains, Coord coord){
	int i;
	int v = indexOf(coord);
	ListCoord result;
	initListCoord(&result);
	for(i=0; i<constrains.n; i++){
		if(constrains.dataConstrains[v][i]==1){
			appendListCoord(&result, positionOfVertex(i));
		}
	}
	return result;
}

typedef struct{
	int data[MAX_LENGTH];
	int size;
}List;

//ham khoi tao danh sach rong
void initList(List *list){
	list->size = 0;
}

//them mot ptu (dinh) vao cuoi danh sach
void appendList(List *list, int x){
	list->data[list->size] = x;
	list->size++;
}

typedef struct{
	int cells[NB_ROWS][NB_COLS];
	Constrains constrains;
}Sudoku;

void initSudoku(Sudoku *sudoku){
	int i, j;
	for(i=0; i<NB_ROWS; i++)
		for(j=0; j<NB_COLS; j++)
			sudoku->cells[i][j] = EMPTY;
	initConstrains(&sudoku->constrains);
}

void initSudokuWithValues(Sudoku *sudoku, int inputs[NB_ROWS][NB_COLS]){
	int i, j;
	for(i=0; i<NB_ROWS; i++)
		for(j=0; j<NB_COLS; j++)
			sudoku->cells[i][j] = inputs[i][j];
	initConstrains(&sudoku->constrains);
}

void printSudoku(Sudoku sudoku){
	int i,j;
	printf("Sudoku: \n");
	for(i=0; i<NB_ROWS; i++){
		if(i % AREA_SQUARE_SIZE == 0)
			printf("-------------------------\n");
		for(j=0; j<NB_COLS; j++){
			if(j % AREA_SQUARE_SIZE == 0)
				printf("| ");
			printf("%d ", sudoku.cells[i][j]);
		}
		printf("|\n");
	}
	printf("-------------------------\n");
}

int isFilledSudoku(Sudoku sudoku){
	int i, j;
	for(i=0; i<NB_ROWS; i++)
		for(j=0; j<NB_COLS; j++)
			if(sudoku.cells[i][j] == EMPTY)
				return 0;
	
	return 1;
}

void spreadConstrainsFrom(Coord position, Constrains* constrains, ListCoord *changeds){
	int row = position.x, col = position.y;
	int i, j;
	for(i=0; i<NB_ROWS; i++){
		if(i!=row){
			Coord pos = {i, col};
			if(addConstrains(constrains, position, pos))
				appendListCoord(changeds, pos);
		}
	}
	
	for(i=0; i<NB_COLS; i++){
		if(i!=col){
			Coord pos = {row, i};
			if(addConstrains(constrains, position, pos))
				appendListCoord(changeds, pos);
		}
	}
	
	for(i=0; i< AREA_SQUARE_SIZE; i++){
		for(j=0; j< AREA_SQUARE_SIZE; j++){
			int areaX = (row/AREA_SQUARE_SIZE) * AREA_SQUARE_SIZE;
			int areaY = (col/AREA_SQUARE_SIZE) * AREA_SQUARE_SIZE;
			if(areaX + i != row || areaY+j != col){
				Coord pos = {areaX+i, areaY+j};
				if(addConstrains(constrains, position, pos))	
					appendListCoord(changeds, pos);
			}
		}
	}
}

List getAvailableValues(Coord position, Sudoku sudoku){
	ListCoord posList = getConstrains(sudoku.constrains, position);
	int availables[MAX_VALUE];
	int i;
	for(i=1; i<MAX_VALUE; i++)
		availables[i] = 1;
	for(i=0; i<posList.size; i++){
		Coord pos = posList.dataListCoord[i];
		if(sudoku.cells[pos.x][pos.y] != EMPTY)
			availables[sudoku.cells[pos.x][pos.y]] = 0;
	}
	
	List result;
	initList(&result);
	for(i=1; i<MAX_VALUE; i++)
		if(availables[i])
			appendList(&result, i);
			
	return result; 
}

Coord getNextEmptyCell(Sudoku sudoku){
	int i, j;
	for(i=0; i<NB_ROWS; i++){
		for(j=0; j<NB_COLS; j++){
			Coord pos = {i, j};
			if(sudoku.cells[i][j] == EMPTY)
				return pos;
		}
	}
}

Coord getNextMinDomainCell(Sudoku sudoku){
	int minLength = INF;
	int i, j;
	Coord result;
	for(i=0; i<NB_ROWS; i++){
		for(j=0; j<NB_COLS; j++){
			if(sudoku.cells[i][j] == EMPTY){
				Coord pos = {i, j};
				int availablesLength = getAvailableValues(pos, sudoku).size;
				if(availablesLength < minLength);
					minLength = availablesLength;
					result = pos;
			}
		}
	}
	return result;
}

int exploredCounter = 0;
int sudokuBackTracking(Sudoku *sudoku){
	if(isFilledSudoku(*sudoku))
		return 1;
	Coord position = getNextEmptyCell(*sudoku);
	List availables = getAvailableValues(position, *sudoku);
	if(availables.size == 0)
		return 0;	
	int j;
	for(j=0; j< availables.size; j++){
		int value = availables.data[j];
		sudoku->cells[position.x][position.y] = value;
		exploredCounter++;
		ListCoord history;
		initListCoord(&history);
		if(sudokuBackTracking(sudoku))
			return 1;
		sudoku->cells[position.x][position.y] = EMPTY;
	}		
	return 0;
}

Sudoku solveSudoku(Sudoku sudoku){
	int i, j;
	clearConstrains(&sudoku.constrains);
	for(i=0; i<NB_ROWS; i++){
		for(j=0; j<NB_COLS; j++){
			if(sudoku.cells[i][j] == EMPTY) {
				ListCoord history;
				initListCoord(&history);
				Coord pos = {i, j};
				spreadConstrainsFrom(pos, &sudoku.constrains, &history);
			}
		}
	}
	exploredCounter = 0;
	if(sudokuBackTracking(&sudoku))
		printf("Solved\n");
	else
		printf("Can not Solved\n");
	printf("Explored %d states \n", exploredCounter);
	return sudoku;
}

int demRangBuoc(Sudoku s){
	Coord coord = {8,8};
	int i, j;
    initConstrains(&s.constrains);
    ListCoord lc;
    spreadConstrainsFrom(coord, &s.constrains, &lc);
    lc = getConstrains(s.constrains,coord);
    int count = 0;
    int dem=0;
    for(i=0; i<=8; i++){
    	for(j=0; j<=8; j++){
    		Coord coord = {i,j};
    		ListCoord lc;
		    spreadConstrainsFrom(coord, &s.constrains, &lc);
		    lc = getConstrains(s.constrains,coord);
    		for(int e = 0; e < lc.size; e++){
		        int temp = s.cells[lc.dataListCoord[e].x][lc.dataListCoord[e].y];
//				printf("%d\n", temp);
		        if(s.cells[i][j] == temp){
		            count++;
        		}
    		}
		}
	}
    return count;
}

int inputs1[9][9]= {
	{5,3,4,6,7,8,9,1,2},
	{6,7,2,1,9,5,3,4,8},
	{1,9,8,3,4,2,5,6,7},
	{8,5,9,7,6,1,4,2,3},
	{4,2,6,8,5,3,7,9,1},
	{7,1,3,9,2,4,8,5,6},
	{9,6,1,5,3,7,2,8,4},
	{2,8,7,4,1,9,6,3,5},
	{3,4,5,2,8,6,1,7,9}
	};


int main(){
	Sudoku sudoku;
	initSudokuWithValues(&sudoku, inputs1);
	
	printf("%d", demRangBuoc(sudoku));
}

//sudoku và kenken thu?t toán di truy?n thu?t toán quá trình luy?n kim
