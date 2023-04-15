#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <cstdlib>
#include <algorithm>
#include <random>
#define ROWS 9
#define COLS 9
#define AREA_SQUARE_SIZE 3
#define POPULATION_SIZE 100
#define SELECTION_RATE 0.25f
#define RANDOM_SELECTION_RATE 0.25f
#define NB_CHILDREN 4
#define MUTATION_RATE 0.2f
#define NUM_BLOCKS 9
#define NUM_CELLS_INSIDE_BLOCK 9
#define show_table show_table_3x3
using namespace std;

struct Coord {
    int x, y;
};

struct Cell {
    Coord position;
    int valueAt;
    bool fixed;
};

void initCell(Cell &cell)
{
    cell.position.x = 0;
    cell.position.y = 0;
    cell.valueAt = 0;
    cell.fixed = false;
}

struct Block {
    Cell cells[NUM_CELLS_INSIDE_BLOCK];
};

void initBlock(Block &B)
{
    for(int i = 0; i < NUM_CELLS_INSIDE_BLOCK; i++) {
        initCell(B.cells[i]);
    }
}

struct Sudoku {
    int table[ROWS][COLS];
    int scoreFit;
    Block blocks[NUM_BLOCKS];
};

void init_Sudoku(Sudoku &sudoku)
{
    for(int i = 0; i < NUM_BLOCKS ; i++) {
        initBlock(sudoku.blocks[i]);
    }
    ifstream f;
    f.open("data.txt");
    int num;
    while(!f.eof()) {
        for(int i = 0; i < ROWS; i++){
            for(int j = 0; j < COLS; j++){
                f >> num;
                sudoku.table[i][j] = num;
            }
        }
    }
    f.close();
    int idxCell = 0, orderBlock = 0;
    for(int x = 0; x < COLS; x+=AREA_SQUARE_SIZE){
        for(int y = 0; y < COLS; y+=AREA_SQUARE_SIZE){
            for(int r = x; r < (x+AREA_SQUARE_SIZE); r++){
                for(int c = y; c < (y+AREA_SQUARE_SIZE); c++){
                    Coord pos = {r, c};
                    sudoku.blocks[orderBlock].cells[idxCell].valueAt = sudoku.table[r][c];
                    sudoku.blocks[orderBlock].cells[idxCell].position = pos;
                    idxCell++;
                }
            }
            idxCell = 0; orderBlock++;
        }
    }
    for(int b = 0; b < NUM_BLOCKS; b++){
        for(int c = 0; c < NUM_CELLS_INSIDE_BLOCK; c++){
            if(sudoku.blocks[b].cells[c].valueAt != 0){
                sudoku.blocks[b].cells[c].fixed = true;
            }
        }
    }
    sudoku.scoreFit = 0;
}

void show_table_3x3(Sudoku sudoku){
    for(int i = 0; i < ROWS; i++){
        if(i % AREA_SQUARE_SIZE == 0)
            cout << "-------------------------\n";
        for(int j = 0; j < COLS; j++){
            if(j % AREA_SQUARE_SIZE == 0) {
                cout << "| ";
            }
            cout << sudoku.table[i][j] << " ";
        }
        cout << "|\n";
    }
    cout << "-------------------------\n";
}

void showtable4x4(Sudoku sudoku){
    for(int i = 0; i < ROWS; i++){
        if(i % AREA_SQUARE_SIZE == 0)
            cout << " -----------------------------------------------------------------\n";
        for(int j = 0; j < COLS; j++){
            if(j % AREA_SQUARE_SIZE == 0) {
                cout << " |  ";
            }
            if(sudoku.table[i][j] < 10) {
                cout << "0";
            }
            cout << sudoku.table[i][j] << " ";
        }
        cout << " |\n";
    }
    cout << " -----------------------------------------------------------------\n";
}

bool is_solved(Sudoku &sudoku) {
    return (sudoku.scoreFit == 0);
}

Sudoku fill_random(Sudoku sudoku){
    Sudoku S;
    init_Sudoku(S);
    for(int b = 0; b < NUM_BLOCKS; b++){
        vector<int> clean;
        for(int e = 1; e <= NUM_CELLS_INSIDE_BLOCK; e++){
            clean.push_back(e);
        }
        for(int c = 0; c < NUM_CELLS_INSIDE_BLOCK; c++){
            if(sudoku.blocks[b].cells[c].fixed == true){
                clean[(sudoku.blocks[b].cells[c].valueAt)-1] = 0;
            }
        }
        vector<int> domainValues;
        for(int e = 0; e < clean.size(); e++){
            if(clean[e] != 0)
                domainValues.push_back(clean[e]);
        }
        for(int c = 0; c < NUM_CELLS_INSIDE_BLOCK; c++){
        	if(sudoku.blocks[b].cells[c].fixed == false){
	        	int indexVal;
	        	if(domainValues.size() > 1)
	            	indexVal = rand() % domainValues.size();
	            else indexVal = 0;

	            S.blocks[b].cells[c].valueAt = domainValues[indexVal];
	            Coord pos = sudoku.blocks[b].cells[c].position;
	            S.table[pos.x][pos.y] = domainValues[indexVal];
				
	            swap(domainValues[indexVal], domainValues[domainValues.size()-1]);
	            domainValues.pop_back();
	        }
		}
    }
    return S;
}

void init_population(Sudoku sudoku, vector<Sudoku> &population){
    for(int i = 0; i < POPULATION_SIZE; i++){
        Sudoku P = fill_random(sudoku);
        population.push_back(P);
    }
}

void fitness(Sudoku &sudoku){
    int fit = 0;
    for(int row = 0; row < ROWS; row++){
        set<int> cleanRow;
        set<int> cleanCol;
        for(int col = 0; col < COLS; col++){
            cleanRow.insert(sudoku.table[row][col]);
            cleanCol.insert(sudoku.table[col][row]);
        }
        fit = fit + (ROWS - cleanRow.size());
        fit = fit + (COLS - cleanCol.size());
    }
    sudoku.scoreFit = fit;
}

bool compare_two_individuals(Sudoku &S1, Sudoku &S2){
    return (S1.scoreFit < S2.scoreFit);
}

void rank_population(vector<Sudoku> &population){
    sort(population.begin(), population.end(), compare_two_individuals);
}

vector<Sudoku> pick_from_population(vector<Sudoku> population, float selectRate, float selectRateRand){
    rank_population(population);
    vector<Sudoku> nextBreeders;
    if(is_solved(population[0]) == true){
        nextBreeders.push_back(population[0]);
        return nextBreeders;
    }

    int nbBestToSelect = population.size() * selectRate;
    int nbRandToSelect = population.size() * selectRateRand;
    for(int i = 0; i < nbBestToSelect; i++) {// select individuals best
        nextBreeders.push_back(population[i]);
    }
    vector<Sudoku> nextBreedersTemp;
    for(int p = nbBestToSelect; p < population.size(); p++) {// copy the rest of individuals after select into  nextBreedersTemp
        nextBreedersTemp.push_back(population[p]);
    }
    for(int j = 0; j < nbRandToSelect; j++) {// random select the rest of individuals push to nextBreeders
        
        int random = rand() % nextBreedersTemp.size();
        nextBreeders.push_back(nextBreedersTemp[random]);
        swap(nextBreedersTemp[random], nextBreedersTemp[nextBreedersTemp.size()-1]);
        nextBreedersTemp.pop_back();
    }
    random_device rd;
    shuffle(nextBreeders.begin(), nextBreeders.end(), rd);
    return nextBreeders;
}

void fill_with_block(Sudoku &sudoku, Block block, int idxBlock){
    for(int c = 0; c < NUM_CELLS_INSIDE_BLOCK; c++){
        sudoku.blocks[idxBlock].cells[c].valueAt = block.cells[c].valueAt;
        Coord pos = sudoku.blocks[idxBlock].cells[c].position;
        sudoku.table[pos.x][pos.y] = block.cells[c].valueAt;
    }
}

void create_one_child(vector<int> &mark, Sudoku father, Sudoku mother, Sudoku &child){
    int idxCrossover = rand() % mark.size();
    int crossoverPoint = mark[idxCrossover];
    for(int i = 0; i < NUM_BLOCKS; i++){
        if(i < crossoverPoint) {
            fill_with_block(child, father.blocks[i], i);
        }
        else {
            fill_with_block(child, mother.blocks[i], i);
        }
    }
    swap(mark[idxCrossover], mark[mark.size()-1]);
    mark.pop_back();
}

vector<Sudoku> create_children(vector<Sudoku> nextBreeders, int nbChild, Sudoku &child){
    vector<Sudoku> nextPopulation;
	vector<int> mark;
	for(int i = 0; i < (nextBreeders.size()/2); i++){
		mark.resize(0);
		for(int cr = 1; cr <= NUM_BLOCKS-2; cr++){
	        mark.push_back(cr);
	    }
        for(int j = 0; j < nbChild; j++){
            create_one_child(mark, nextBreeders[i], nextBreeders[nextBreeders.size()-1-i], child);
            nextPopulation.push_back(child);
        }
    }
    return nextPopulation;
}

void mutate_with_block(Sudoku &sudoku, int idxBlock){
    vector<int> clean;
    for(int e = 1; e <= NUM_CELLS_INSIDE_BLOCK; e++) {
        clean.push_back(e);
    }
    for(int c = 0; c < NUM_CELLS_INSIDE_BLOCK; c++){
        if(sudoku.blocks[idxBlock].cells[c].fixed == true) {
            clean[(sudoku.blocks[idxBlock].cells[c].valueAt)-1] = 0;
        }
    }
    vector<int> domainValues;
    for(int e = 0; e < clean.size(); e++){
        if(clean[e] != 0)
            domainValues.push_back(clean[e]);
    }
    for(int c = 0; c < NUM_CELLS_INSIDE_BLOCK; c++){
        if(sudoku.blocks[idxBlock].cells[c].fixed == false){
            int indexVal;
            if(domainValues.size() > 1)
                indexVal = rand() % domainValues.size();
            else indexVal = 0;

            sudoku.blocks[idxBlock].cells[c].valueAt = domainValues[indexVal];
            Coord pos = sudoku.blocks[idxBlock].cells[c].position;
            sudoku.table[pos.x][pos.y] = domainValues[indexVal];
            
            swap(domainValues[indexVal], domainValues[domainValues.size()-1]);
            domainValues.pop_back();
        }
    }
}

void mutate_one_individual(Sudoku &sudoku)
{
    int idxBlock, idxRow, count = 0;
    idxRow = idxBlock = rand() % ROWS;
    vector<int> clean;
    for(int e = 1; e <= ROWS; e++) {
        clean.push_back(e);
    }
    for(int r = 0; r < ROWS; r++){
        int valuesInsideRow = sudoku.table[idxRow][r];
        clean[valuesInsideRow-1] = 0;
    }
    for(int e = 0; e < clean.size(); e++) {
        if(clean[e] != 0)
            count++;
    }
    if(count > 0) {
        mutate_with_block(sudoku, idxBlock);
    }
}

void mutate_population(vector<Sudoku> &population, float mutationRate){
    random_device rd;
    shuffle(population.begin(), population.end(), rd);

    int nbMutation = population.size() * mutationRate;
    for(int cnt = 0; cnt < nbMutation; cnt++){
        mutate_one_individual(population[cnt]);
    }
}

bool same_table(Sudoku A, Sudoku B){
    for(int x = 0; x < ROWS; x++){
        for(int y = 0; y < COLS; y++)
            if(A.table[x][y] != B.table[x][y])
                return false;
    }
    return true;
}

int main()
{
    Sudoku start, child;
    init_Sudoku(start);
    show_table(start);
    init_Sudoku(child);
    vector<Sudoku> population;
    init_population(start, population); // initialization
    for(int i = 0; i < population.size(); i++){// calculating fitness
        fitness(population[i]);
    }
	bool flag = false;
	int count = 0;
	while(flag == false){
		count++;
        // cout << count << endl;
		vector<Sudoku> nextBreeders = pick_from_population(population, SELECTION_RATE, RANDOM_SELECTION_RATE); // selection
		if(is_solved(nextBreeders[0]) == true){
            cout << "Problem solved after" << count << "generations!!! Solution found is:\n" << count;
			flag = true;
			show_table(nextBreeders[0]);
			break;
		}
		else flag = false;
		vector<Sudoku> newGeneration = create_children(nextBreeders, NB_CHILDREN, child); // crossover
		for(int i = 0; i < newGeneration.size(); i++){
			for(int j = 0; j < newGeneration.size(); j++){
				if(i != j && same_table(newGeneration[i], newGeneration[j]) == true){
                    newGeneration[j] = fill_random(start);
				}
			}
		}

        mutate_population(newGeneration, MUTATION_RATE); // mutation
		for(int child = 0; child < newGeneration.size(); child++){
			fitness(newGeneration[child]);
			if(is_solved(newGeneration[child]) == true){
                cout << "Problem solved after " << count << " generations!!! Solution found is:\n";
				flag = true;
				show_table(newGeneration[child]);
				break;
			}
			else flag = false;
		}

		population.clear();
		for(int p = 0; p < newGeneration.size(); p++){ // new generation
            population.push_back(newGeneration[p]);
		}
	}
    return 0;
}