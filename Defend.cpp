//defend id: 614
#include "STcpClient.h"
#include <iostream>
#define EMPTY 0
#define BLACK 1
#define WHITE 2
using namespace std;
vector<int> coord(int x, int y);

std::vector<std::vector<int>> GetStep(std::vector<std::vector<int>>& board, bool is_black) {
	std::vector<std::vector<int>> step;
	int ally = is_black ? BLACK : WHITE;
	int enemy = is_black ? WHITE : BLACK;
	
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (board[i][j] == ally) {
				step.push_back(coord(i,j));
				while (1) {
					if (i - 2 >= 0 && board[i - 1][j] == enemy && board[i - 2][j] == EMPTY) { //eat enemy above
						step.push_back(coord(i - 2, j));
						board[i][j] = EMPTY;
						board[i - 1][j] = EMPTY;
						board[i - 2][j] = ally;
						i -= 2;
					} else if (i + 2 <= 7 && board[i + 1][j] == enemy && board[i + 2][j] == EMPTY) { //eat enemy below
						step.push_back(coord(i + 2, j));
						board[i][j] = EMPTY;
						board[i + 1][j] = EMPTY;
						board[i + 2][j] = ally;
						i += 2;
					} else if (j - 2 >= 0 && board[i][j - 1] == enemy && board[i][j - 2] == EMPTY) { //eat enemy left
						step.push_back(coord(i, j - 2));
						board[i][j] = EMPTY;
						board[i][j - 1] = EMPTY;
						board[i][j - 2] = ally;
						j -= 2;
					} else if (j + 2 <= 7 && board[i][j + 1] == enemy && board[i][j + 2] == EMPTY) { //eat enemy right
						step.push_back(coord(i, j + 2));
						board[i][j] = EMPTY;
						board[i][j + 1] = EMPTY;
						board[i][j + 2] = ally;
						j += 2;
					} else {
						break;
					}
					/*cout << "vector size:" << step.size() << endl;
					if (step.size() > 100) {
						cout << "steps over 100\n";
						break;
					}*/
				} //end while
				if (step.size() == 1) {
					step.resize(0);
					continue;
				} else {
					/*cout << "moves: ";
					for (int x = 0; x < step.size(); ++x) {
						cout << '(' << step[x][0] << ',' << step[x][1] << ')';
					}
					cout << '\n';*/
					return step;
				}
			}
		}
	}
	return step;
}

int main() {
	int id_package;
	std::vector<std::vector<int>> board, step;
	bool is_black;
	while (true) {
		if (GetBoard(id_package, board, is_black))
			break;

		step = GetStep(board, is_black);
		SendStep(id_package, step);
	}
}

vector<int> coord(int x, int y) {
	vector<int> mycoord;
	mycoord.push_back(x);
	mycoord.push_back(y);
	return mycoord;
}