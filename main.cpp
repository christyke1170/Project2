#include <iostream>
#include <string>
#include <cmath>
#include "P2random.h"
#include <cstdlib>
#include <queue> 
#include <algorithm>
#include <iomanip>
#include <stdio.h>
#include <string.h>

using namespace std;

struct Tile {
	int col = 0;
	int row = 0;
	int rubble;
	bool tnt = false;
	bool visible = false;
	bool visited = false;
	bool tntqueued = false;
	bool blown = false;
};
struct CompareRubble {
	bool operator()(const Tile& lhs, const Tile& rhs) const {
		if (lhs.rubble == rhs.rubble) {
			if (lhs.col == rhs.col) {
				if (lhs.row > rhs.row) {
					return lhs.row > rhs.row;
				}
			}
			return lhs.col > rhs.col;
		}
		return lhs.rubble > rhs.rubble;
	}
} compareRubble;

struct CompareRubbleOpp {
	bool operator()(const Tile& lhs, const Tile& rhs) const {
		if (lhs.rubble == rhs.rubble) {
			if (lhs.col == rhs.col) {
				if (lhs.row < rhs.row) {
					return lhs.row < rhs.row;
				}
			}
			return lhs.col < rhs.col;
		}
		return lhs.rubble < rhs.rubble;
	}
} compareRubbleOpp;

struct TileMap {
	priority_queue<Tile, vector<Tile>, CompareRubble> queue;
	priority_queue<Tile, vector<Tile>, CompareRubble> tntQueue;
	vector<vector<Tile>> grid;
	vector<Tile> statOut;
	priority_queue<int, vector<int>, greater<int>> medianUpper;
	priority_queue<int, vector<int>, less<int>> medianLower;
	string mapMode = "";
	int unsigned size = 0;
	int unsigned seed = 0;
	int unsigned maxRubble = 0;
	int unsigned tnt = 0;
	int unsigned currentRow = 0;
	int unsigned currentCol = 0;
	int totalTileCleared = 0;
	int totalRubbleCleared = 0;
	bool stats = false;
	int statsOutputNum = 0;
	bool medianMode = false;
	bool verbose = false;
	bool blowUp = false;
	void readInMap();
	void readInPseudo();
	void travelTile();
	void tntExplosion();
	void calcTNT();
	bool notAtEdge();
	double returnMedian();
	void checkMedianSize();
	void returnStats();
};
int main(int argc, char* argv[]) {
	ios_base::sync_with_stdio(false);
	cout << fixed << setprecision(2);
	TileMap journey;

	for (int i = 0; i < argc; ++i) {
		if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			cout << "help haha \n";
			exit(0);
		}
		else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
			journey.verbose = true;
		}
		else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--stats")) {
			journey.stats = true;
			journey.statsOutputNum = atoi(argv[i + 1]);
		}
		else if (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--median")) {
			journey.medianMode = true;
			journey.medianUpper.push(numeric_limits<int>::max());
			journey.medianLower.push(numeric_limits<int>::min());
		}
	}

	string dummy;
	cin >> journey.mapMode;
	cin >> dummy;
	cin >> journey.size;
	cin >> dummy;
	cin >> journey.currentRow;
	cin >> journey.currentCol;
	if ((journey.currentRow >= journey.size) || (journey.currentCol >= journey.size)) {
		cerr << "the start coordinate is not within the grid!" << endl;
		exit(1);
	}
	journey.grid.resize(journey.size);
	for (int unsigned i = 0; i < journey.size; ++i) {
		journey.grid[i].resize(journey.size);
	}
	if (journey.mapMode == "R") {
		cin >> dummy;
		cin >> journey.seed;
		cin >> dummy;
		cin >> journey.maxRubble;
		cin >> dummy;
		cin >> journey.tnt;
		journey.readInPseudo();
	}
	else if (journey.mapMode == "M") {
		journey.readInMap();
	}
	else {
		cerr << "not a valid input" << endl;
		exit(1);
	}

	journey.grid[journey.currentRow][journey.currentCol].visible = true;

	// START LOOP HERE ----------------------------------//
	bool run = true;
	while (run) {
		run = journey.notAtEdge();
		if (journey.grid[journey.currentRow][journey.currentCol].rubble == -1) {
			//if tnt
			journey.tntExplosion();
			if (journey.grid[journey.currentRow][journey.currentCol].rubble == -1) {
				run = true;
			}
			if (journey.blowUp) {
				run = true;
			}
		}
		else if ((journey.tntQueue.size() > 0) || (journey.blowUp)) {
			if (journey.grid[journey.currentRow][journey.currentCol].blown == false) {
				journey.calcTNT();
				journey.blowUp = false;
			}
		}
		else {
			journey.travelTile();
		}
	}
	cout << "Cleared " << journey.totalTileCleared;
	cout << " tiles containing " << journey.totalRubbleCleared;
	cout << " rubble and escaped.\n";
	if (journey.stats) {
		journey.returnStats();
	}
	return 0;
}

void TileMap::readInMap() {
	Tile currentTile;
	for (int unsigned i = 0; i < size; ++i) {
		for (int unsigned j = 0; j < size; ++j) {
			currentTile.row = i;
			currentTile.col = j;
			cin >> currentTile.rubble;
			if (currentTile.rubble == -1)
				currentTile.tnt = true;
			grid[i][j] = currentTile;
		}
	}
}

void TileMap::readInPseudo() {
	stringstream ss;
	P2random::PR_init(ss, size, seed, maxRubble, tnt);
	Tile currentTile;
	for (int unsigned i = 0; i < size; ++i) {
		for (int unsigned j = 0; j < size; ++j) {
			currentTile.row = i;
			currentTile.col = j;
			ss >> currentTile.rubble;
			if (currentTile.rubble == -1)
				currentTile.tnt = true;
			grid[i][j] = currentTile;
		}
	}
}

void TileMap::travelTile() {
	int rubble = grid[currentRow][currentCol].rubble;
	int wall = size - 1;
	int col = currentCol;
	int row = currentRow;
	totalRubbleCleared += rubble;

	if (verbose && (grid[currentRow][currentCol].rubble > 0)) {
		cout << "Cleared: " << rubble << " at [" << currentRow << "," << currentCol << "]\n";
	}

	if (medianMode && rubble > 0) {
		if (rubble > medianUpper.top()) {
			medianUpper.push(rubble);
		}
		else {
			medianLower.push(rubble);
		}
		checkMedianSize();
		cout << "Median difficulty of clearing rubble is: " << returnMedian() << "\n";
	}
	if (stats && rubble > 0) {
		Tile temp;
		temp.col = currentCol;
		temp.row = currentRow;
		temp.rubble = rubble;
		statOut.push_back(temp);
	}
	grid[currentRow][currentCol].visited = true;
	grid[currentRow][currentCol].rubble = 0;

	if (row < wall) { //down
		if (grid[currentRow + 1][currentCol].visible == false) {
			grid[currentRow + 1][currentCol].visible = true;
			queue.push(grid[currentRow + 1][currentCol]);
		}
	}
	if (col < wall) {//right
		if (grid[currentRow][currentCol + 1].visible == false) {
			grid[currentRow][currentCol + 1].visible = true;
			queue.push(grid[currentRow][currentCol + 1]);
		}
	}
	if (currentRow >= 1) {//up
		if (grid[currentRow - 1][currentCol].visible == false) {
			grid[currentRow - 1][currentCol].visible = true;
			queue.push(grid[currentRow - 1][currentCol]);
		}
	}
	if (currentCol >= 1) {//left
		if (grid[currentRow][currentCol - 1].visible == false) {
			grid[currentRow][currentCol - 1].visible = true;
			queue.push(grid[currentRow][currentCol - 1]);
		}
	}


	if (notAtEdge()) {
		currentRow = queue.top().row;
		currentCol = queue.top().col;
		Tile temp = queue.top(); //check for ties
		queue.pop();
		if (queue.top().rubble == temp.rubble) {
			if (queue.top().col < temp.col) {
				queue.push(temp);
				currentRow = queue.top().row;
				currentCol = queue.top().col;
				queue.pop();
			}
			else if (queue.top().col == temp.col) {
				if (queue.top().row < temp.row) {
					queue.push(temp);
					currentRow = queue.top().row;
					currentCol = queue.top().col;
					queue.pop();
				}
			}
		}
	}
	if (rubble > 0) {
		totalTileCleared++;
	}
}

void TileMap::tntExplosion() {
	if (verbose) {
		cout << "TNT explosion at [" << currentRow << "," << currentCol << "]!\n";
	}
	if (stats) {
		Tile temp;
		temp.col = currentCol;
		temp.row = currentRow;
		temp.rubble = 0;
		temp.tnt = true;
		statOut.push_back(temp);
	}
	int wall = size - 1;
	int col = currentCol;
	int row = currentRow;
	grid[currentRow][currentCol].visited = true;
	grid[currentRow][currentCol].blown = true;
	grid[currentRow][currentCol].rubble = 0;

	if (row < wall) { //down
		grid[currentRow + 1][currentCol].visible = true;
		if ((grid[currentRow + 1][currentCol].rubble != 0) && (grid[currentRow + 1][currentCol].tntqueued == false)) { //checking to make sure not 0
			grid[currentRow + 1][currentCol].tntqueued = true;
			tntQueue.push(grid[currentRow + 1][currentCol]);
		}
		else if ((grid[currentRow + 1][currentCol].rubble == 0) && (!grid[currentRow + 1][currentCol].visited)) {
			queue.push(grid[currentRow + 1][currentCol]);
		}
	}
	if (col < wall) {//right
		grid[currentRow][currentCol + 1].visible = true;
		if ((grid[currentRow][currentCol + 1].rubble != 0) && (grid[currentRow][currentCol + 1].tntqueued == false)) {
			grid[currentRow][currentCol + 1].tntqueued = true;
			tntQueue.push(grid[currentRow][currentCol + 1]);
		}
		else if ((grid[currentRow][currentCol + 1].rubble == 0) && (!grid[currentRow][currentCol + 1].visited)) {
			queue.push(grid[currentRow][currentCol + 1]);
		}
	}
	if (currentRow >= 1) {//up
		grid[currentRow - 1][currentCol].visible = true;
		if ((grid[currentRow - 1][currentCol].rubble != 0) && (grid[currentRow - 1][currentCol].tntqueued == false)) {
			grid[currentRow - 1][currentCol].tntqueued = true;
			tntQueue.push(grid[currentRow - 1][currentCol]);
		}
		else if ((grid[currentRow - 1][currentCol].rubble == 0) && (!grid[currentRow - 1][currentCol].visited)) {
			queue.push(grid[currentRow - 1][currentCol]);
		}
	}
	if (currentCol >= 1) {//left
		grid[currentRow][currentCol - 1].visible = true;
		if ((grid[currentRow][currentCol - 1].rubble != 0) && (grid[currentRow][currentCol - 1].tntqueued == false)) {
			grid[currentRow][currentCol - 1].tntqueued = true;
			tntQueue.push(grid[currentRow][currentCol - 1]);
		}
		else if ((grid[currentRow][currentCol - 1].rubble == 0) && (!grid[currentRow][currentCol - 1].visited)) {
			queue.push(grid[currentRow][currentCol - 1]);
		}
	}
	if (!tntQueue.empty()) {
		blowUp = true;
		currentRow = tntQueue.top().row;
		currentCol = tntQueue.top().col;
		Tile temp = tntQueue.top(); //check for ties
		tntQueue.pop();
		if (tntQueue.top().rubble == temp.rubble) {
			if (tntQueue.top().col < temp.col) {
				tntQueue.push(temp);
				currentRow = tntQueue.top().row;
				currentCol = tntQueue.top().col;
				tntQueue.pop();
			}
			else if (tntQueue.top().col == temp.col) {
				if (tntQueue.top().row < temp.row) {
					tntQueue.push(temp);
					currentRow = tntQueue.top().row;
					currentCol = tntQueue.top().col;
					tntQueue.pop();
				}
			}
		}
	}

}

void TileMap::calcTNT() {
	int rubbletemp = grid[currentRow][currentCol].rubble;
	totalRubbleCleared += rubbletemp;
	if (stats) {
		Tile temp;
		temp.col = currentCol;
		temp.row = currentRow;
		temp.rubble = rubbletemp;
		statOut.push_back(temp);
	}
	grid[currentRow][currentCol].rubble = 0;
	grid[currentRow][currentCol].visible = true;
	queue.push(grid[currentRow][currentCol]);
	totalTileCleared++;
	if (verbose) {
		cout << "Cleared by TNT: " << rubbletemp << " at [" << currentRow << "," << currentCol << "]\n";
	}
	if (medianMode) {
		if (rubbletemp > medianUpper.top()) {
			medianUpper.push(rubbletemp);
		}
		else {
			medianLower.push(rubbletemp);
		}
		checkMedianSize();
		cout << "Median difficulty of clearing rubble is: " << returnMedian() << "\n";
	}
	while (!tntQueue.empty()) {
		Tile temp = tntQueue.top();
		Tile boom = temp;
		tntQueue.pop();
		if (tntQueue.top().rubble == temp.rubble) {
			if (tntQueue.top().col < temp.col) {
				tntQueue.push(temp);
				boom = tntQueue.top();
				tntQueue.pop();
			}
			else if (tntQueue.top().col == temp.col) {
				if (tntQueue.top().row < temp.row) {
					tntQueue.push(temp);
					boom = tntQueue.top();
					tntQueue.pop();
				}
			}
		}
		if (stats) {
			Tile temp;
			temp.col = boom.col;
			temp.row = boom.row;
			temp.rubble = boom.rubble;
			statOut.push_back(temp);
		}
		if (verbose) {
			cout << "Cleared by TNT: " << boom.rubble << " at [" << boom.row << "," << boom.col << "]\n";
		}
		if (medianMode) {
			if (boom.rubble > medianUpper.top()) {
				medianUpper.push(boom.rubble);
			}
			else {
				medianLower.push(boom.rubble);
			}
			checkMedianSize();
			cout << "Median difficulty of clearing rubble is: " << returnMedian() << "\n";
		}
		totalRubbleCleared += grid[boom.row][boom.col].rubble;
		grid[boom.row][boom.col].rubble = 0;
		grid[boom.row][boom.col].visible = true;
		queue.push(grid[boom.row][boom.col]);
		totalTileCleared++;
	}
	if (notAtEdge()) {
		currentRow = queue.top().row;
		currentCol = queue.top().col;
		Tile temp = queue.top(); //check for ties
		queue.pop();
		if (queue.top().rubble == temp.rubble) {
			if (queue.top().col < temp.col) {
				queue.push(temp);
				currentRow = queue.top().row;
				currentCol = queue.top().col;
				queue.pop();
			}
			else if (queue.top().col == temp.col) {
				if (queue.top().row < temp.row) {
					queue.push(temp);
					currentRow = queue.top().row;
					currentCol = queue.top().col;
					queue.pop();
				}
			}
		}
	}
}
bool TileMap::notAtEdge() {
	if (tntQueue.empty()) {
		if (currentCol <= 0) {
			return false;
		}
		if (currentRow <= 0) {
			return false;
		}
		if (currentCol >= size - 1) {
			return false;
		}
		if (currentRow >= size - 1) {
			return false;
		}
	}
	return true;
}

double TileMap::returnMedian() {
	double medianReturn = 0.0;

	if ((int)medianUpper.size() == (int)medianLower.size()) { //if even
		medianReturn = (double)(medianUpper.top() + medianLower.top()) / 2;
	}
	else if((int)medianUpper.size() > (int)medianLower.size()){
		return medianUpper.top();
	}
	else {
		return medianLower.top();
	}
	return medianReturn;
}

void TileMap::checkMedianSize() {
	if ((int) medianUpper.size() > (int)medianLower.size()) {
		medianLower.push(medianUpper.top());
		medianUpper.pop();
	}
	else if((int) medianLower.size() > (int) medianUpper.size()){
		medianUpper.push(medianLower.top());
		medianLower.pop();
	}
}

void TileMap::returnStats() {
	cout << "First tiles cleared:\n";
	for (int i = 0; i < (int)statOut.size(); ++i) {
		if (i == statsOutputNum) {
			break;
		}
		if (statOut[i].tnt == true) {
			cout << "TNT at [" << statOut[i].row << "," << statOut[i].col << "]\n";
		}
		else {
			cout << statOut[i].rubble << " at [" << statOut[i].row << "," << statOut[i].col << "]\n";
		}
	}
	cout << "Last tiles cleared:\n";
	int count = 0;
	for (int i = (int)statOut.size() - 1; i >= 0; --i) {
		if (count == statsOutputNum) {
			break;
		}
		if (statOut[i].tnt == true) {
			cout << "TNT at [" << statOut[i].row << "," << statOut[i].col << "]\n";
		}
		else {
			cout << statOut[i].rubble << " at [" << statOut[i].row << "," << statOut[i].col << "]\n";
		}
		++count;
	}
	cout << "Easiest tiles cleared:\n";
	sort(statOut.begin(), statOut.begin() + statOut.size(), compareRubbleOpp);
	for (int i = 0; i < (int)statOut.size(); ++i) {
		if (i == statsOutputNum) {
			break;
		}
		if (statOut[i].tnt == true) {
			cout << "TNT at [" << statOut[i].row << "," << statOut[i].col << "]\n";
		}
		else {
			cout << statOut[i].rubble << " at [" << statOut[i].row << "," << statOut[i].col << "]\n";
		}
	}
	cout << "Hardest tiles cleared:\n";
	count = 0;
	for (int i = (int)statOut.size() - 1; i >= 0; --i) {
		if (count == statsOutputNum) {
			break;
		}
		if (statOut[i].tnt == true) {
			cout << "TNT at [" << statOut[i].row << "," << statOut[i].col << "]\n";
		}
		else {
			cout << statOut[i].rubble << " at [" << statOut[i].row << "," << statOut[i].col << "]\n";
		}
		++count;
	}
}