//gamer id: 524
//step: moving one piece one unit up, down, left, right, or jumping through exactly one piece
//move: a series of steps chained together (can be 0 steps, 1 step, or jumping through multiple pieces consecutively)
//turn: one move from each player (including static moves)
#include "STcpClient.h"
#include <iostream>
#include <climits>
#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define MAXIMIZER 1 //black as maximizer
#define MINIMIZER 2 //white as minimizer
#define MAXDEPTH 2 //number of moves to predict
#define MAXSTEPS 99 //max number of steps in one move
#define WATCHTURN 1600 //display debug info on this turn
using namespace std;
int turn;

typedef struct coordinates {
	int row, col;
	struct coordinates() {}
	struct coordinates(int r, int c) : row(r), col(c) {}
	bool operator==(struct coordinates x)
	{
		return this->row == x.row && this->col == x.col;
	}
} coord_t;

typedef struct {
	vector<coord_t> steps;
	int score;
} move_t;

vector<vector<int>> GetStep(vector<vector<int>>& board, bool is_black);
move_t get_move(vector<vector<int>> board, int player);
move_t try_move(vector<vector<int>> board, int player, move_t curmove);
int heuristic(const vector<vector<int>>& board, int player, bool disp = false);
vector<vector<int>> simulation(vector<vector<int>> board, const move_t& move);
vector<vector<int>> move_to_vvint(move_t move);
void print_move(move_t move);
void print_board(const vector<vector<int>>& board);

vector<vector<int>> GetStep(vector<vector<int>>& board, bool is_black)
{
	++turn;
	int player = is_black ? BLACK : WHITE;
	move_t move = get_move(board, player);
	//print_move(move);
	return move_to_vvint(move);
}

int main()
{
	turn = 0;
	int id_package;
	vector<vector<int>> board, step;
	bool is_black;
	while (true) {
		if (GetBoard(id_package, board, is_black))
			break;

		step = GetStep(board, is_black);
		SendStep(id_package, step);
	}
}

move_t get_move(vector<vector<int>> board, int player)
{
	move_t best_move;
	best_move.score = (player == MAXIMIZER) ? INT_MIN : INT_MAX;
	int enemy = (player == BLACK) ? WHITE : BLACK;
	//heuristic(board, player, 1);

	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (board[i][j] == player) {
				move_t move;
				move.steps.push_back(coord_t(i, j));
				move.score = heuristic(board, player);
				move = try_move(board, player, move);
				if (player == MAXIMIZER && move.score > best_move.score) {
					best_move = move;
				}
				if (player == MINIMIZER && move.score < best_move.score) {
					best_move = move;
				}
			}
		}
	}
	return best_move;
}

move_t try_move(vector<vector<int>> board, int player, move_t curmove)
{
	//cout << "enter trymove\n";
	int ally = player;
	int enemy = (player == BLACK) ? WHITE : BLACK;
	coord_t movepiece = curmove.steps[curmove.steps.size() - 1];
	int i = movepiece.row;
	int j = movepiece.col;
	move_t bestmove = curmove;
	//cout << heuristic(board, player)<<' ';
	//print_move(curmove);

	if (curmove.steps.size() > MAXSTEPS)
		return curmove;
	if (curmove.steps.size() >= 3 && curmove.steps[curmove.steps.size() - 1] == curmove.steps[curmove.steps.size() - 3])
		return curmove;
	
	int jumping = (curmove.steps.size() > 1) ? 4 : 0; //if in jumping sequence
	for (int n = 1; n <= 8 - jumping; ++n) { //n <= (8 - jumping) to prevent single step after jumping
		vector<vector<int>> newboard = board;
		move_t newmove = curmove;
		switch (n) {
		case 1:
			if (i - 2 >= 0 && board[i - 1][j] != EMPTY && board[i - 2][j] == EMPTY) { //eat enemy above or jump over ally
				//cout << "case 1\n";
				newboard[i][j] = EMPTY;
				newboard[i - 1][j] = (newboard[i - 1][j] == enemy) ? EMPTY : ally;
				newboard[i - 2][j] = ally;
				newmove.steps.push_back(coord_t(i - 2, j));
				break;
			}
			++n;
		case 2:
			if (i + 2 <= 7 && board[i + 1][j] != EMPTY && board[i + 2][j] == EMPTY) { //eat enemy below or jump over ally
				//cout << "case 2\n";
				newboard[i][j] = EMPTY;
				newboard[i + 1][j] = (newboard[i + 1][j] == enemy) ? EMPTY : ally;
				newboard[i + 2][j] = ally;
				newmove.steps.push_back(coord_t(i + 2, j));
				break;
			}
			++n;
		case 3:
			if (j - 2 >= 0 && board[i][j - 1] != EMPTY && board[i][j - 2] == EMPTY) { //eat enemy left or jump over ally
				//cout << "case 3\n";
				newboard[i][j] = EMPTY;
				newboard[i][j - 1] = (newboard[i][j - 1] == enemy) ? EMPTY : ally;
				newboard[i][j - 2] = ally;
				newmove.steps.push_back(coord_t(i, j - 2));
				break;
			}
			++n;
		case 4:
			if (j + 2 <= 7 && board[i][j + 1] != EMPTY && board[i][j + 2] == EMPTY) { //eat enemy right or jump over ally
				//cout << "case 4\n";
				newboard[i][j] = EMPTY;
				newboard[i][j + 1] = (newboard[i][j + 1] == enemy) ? EMPTY : ally;
				newboard[i][j + 2] = ally;
				newmove.steps.push_back(coord_t(i, j + 2));
				break;
			}
			break; //jump-step division line: fallthrough prohibited
		case 5:
			if (i - 1 >= 0 && board[i - 1][j] == EMPTY) { //move up
				//cout << "case 5\n";
				newboard[i][j] = EMPTY;
				newboard[i - 1][j] = ally;
				newmove.steps.push_back(coord_t(i - 1, j));
				break;
			}
			++n;
		case 6:
			if (i + 1 <= 7 && board[i + 1][j] == EMPTY) { //move down
				//cout << "case 6\n";
				newboard[i][j] = EMPTY;
				newboard[i + 1][j] = ally;
				newmove.steps.push_back(coord_t(i + 1, j));
				break;
			}
			++n;
		case 7:
			if (j - 1 >= 0 && board[i][j - 1] == EMPTY) { //move left
				//cout << "case 7\n";
				newboard[i][j] = EMPTY;
				newboard[i][j - 1] = ally;
				newmove.steps.push_back(coord_t(i, j - 1));
				break;
			}
			++n;
		case 8:
			if (j + 1 <= 7 && board[i][j + 1] == EMPTY) { //move right
				//cout << "case 8\n";
				newboard[i][j] = EMPTY;
				newboard[i][j + 1] = ally;
				newmove.steps.push_back(coord_t(i, j + 1));
				break;
			}
		}

		if (turn == WATCHTURN) cout << heuristic(newboard, player) << ' ';
		if (turn == WATCHTURN) print_move(newmove);
		if (newmove.steps.size() != curmove.steps.size()) { //if step can be taken
			if (n <= 4) { //jump
				newmove.score = heuristic(newboard, player);
				newmove = try_move(newboard, player, newmove);
				if (turn == WATCHTURN) heuristic(newboard, player, 1);
			} else { //n>4: single step, no jumping
				newmove.score = heuristic(newboard, player);
				if (turn == WATCHTURN) heuristic(newboard, enemy, 1);
			}

			if (player == MAXIMIZER && newmove.score > bestmove.score) {
				//if (turn == WATCHTURN) cout << "elected! newmove="<<newmove.score<<" bestmove="<<bestmove.score<<'\n';
				//if (turn == WATCHTURN) print_board(board);
				//if (turn == WATCHTURN) heuristic(newboard, player, 1);
				bestmove = newmove;
			}
			if (player == MINIMIZER && newmove.score < bestmove.score) {
				//if (turn == WATCHTURN) cout << "elected! newmove=" << newmove.score << " bestmove=" << bestmove.score << '\n';
				//if (turn == WATCHTURN) print_board(board);
				//if (turn == WATCHTURN) heuristic(newboard, player, 1);
				bestmove = newmove;
			}
		}
	}
	//cout << "exit trymove\n";
	return bestmove;
}

int heuristic(const vector<vector<int>>& board, int player, bool disp) //assume black is maximizer
{
	//cout << "enter heuristic\n";
	if(disp) print_board(board);
	int score = 0;
	int ally = player;
	int enemy = (player == BLACK) ? WHITE : BLACK;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (board[i][j] != EMPTY) {
				if (disp && turn == WATCHTURN) {
					if (board[i][j] == BLACK) {
						cout << "BLACK " << i << ' ' << j << ' ';
					} else {
						cout << "WHITE " << i << ' ' << j << ' ';
					}
				}
				if (board[i][j] == BLACK) {
					score += (MAXIMIZER == BLACK) ? (128 + j * j) : (-128 - j * j); //
					//if (disp&&turn == WATCHTURN) cout << "black: +pts\n";
				}else{
					score += (MAXIMIZER == WHITE) ? (128 + (7 - j) * (7 - j)) : (-128 - (7 - j) * (7 - j));
					//if (disp && turn == WATCHTURN) cout << "white: -pts\n";
				}
				if(disp) cout << "existential+positional scoring: " << score << endl;
				if (player == board[i][j]) { //things to check for when it's your turn
					//stay away from enemies, don't suicide (score penalty counted only once per piece)
					if (i - 1 >= 0 && board[i - 1][j] == enemy) { //enemy above
						if (disp) cout << "enemy above\n";
						score += (player == MAXIMIZER) ? (-64) : (64); //dont stay in place if you can eat enemy, move away if enemy can eat you
						//if (disp) cout << "predator-prey scoring: " << score << endl;
					} else if (i + 1 <= 7 && board[i + 1][j] == enemy) { //enemy below
						if(disp) cout << "enemy below\n";
						score += (player == MAXIMIZER) ? (-64) : (64);
						//if (disp) cout << "predator-prey scoring: " << score << endl;
					} else if (j - 1 >= 0 && board[i][j - 1] == enemy) { //enemy to the left
						if (disp) cout << "enemy left\n";
						score += (player == MAXIMIZER) ? (-64) : (64);
						//if (disp) cout << "predator-prey scoring: " << score << endl;
					} else if (j + 1 <= 7 && board[i][j + 1] == enemy) { //enemy to the right
						if (disp) cout << "enemy right\n";
						score += (player == MAXIMIZER) ? (-64) : (64);
						//if (disp) cout << "predator-prey scoring: " << score << endl;
					} else {
						//threatening: rewards stack for keeping multiple enemies at bay, only valid if not in immediate danger
						int dir = (player == BLACK) ? 1 : -1; //direction to watch for enemies
						if (j + 2 * dir >= 0 && j + 2 * dir <= 7 && board[i][j + 2 * dir] == enemy) //enemy two units in front
								score += (player == MAXIMIZER) ? (48) : (-48); //slightly higher reward for 2 unit advance when capturing
						if (i - 1 >= 0 && j + dir >= 0 && j + dir <= 7 && board[i - 1][j + dir] == enemy) {//enemy at upper diagonal
							if (i - 2 >= 0 && board[i - 2][j] == EMPTY) //if valid threat up
								score += (player == MAXIMIZER) ? (32) : (-32);
							if (j + 2 * dir >= 0 && j + 2 * dir <= 7 && board[i][j + 2 * dir] == EMPTY) //if valid threat front
								score += (player == MAXIMIZER) ? (32) : (-32);
						}
						if (i + 1 <= 7 && j + dir >= 0 && j + dir <= 7 && board[i + 1][j + dir] == enemy) { //enemy at lower diagonal
							if (i + 2 <= 7 && board[i + 2][j] == EMPTY) //if valid threat
								score += (player == MAXIMIZER) ? (32) : (-32);
						}
					}
					
				}
			}
		}
	}
	//cout << "exit heuristic\n";
	return score;
}

vector<vector<int>> move_to_vvint(move_t move)
{
	vector<vector<int>> move_vec;
	vector<int> step;
	for (unsigned int i = 0; i < move.steps.size(); ++i) {
		step.push_back(move.steps[i].row);
		step.push_back(move.steps[i].col);
		move_vec.push_back(step);
		step.resize(0);
	}
	return move_vec;
}

void print_move(move_t move)
{
	for (unsigned int i = 0; i < move.steps.size(); ++i) {
		cout << '(' << move.steps[i].row << ',' << move.steps[i].col << ')';
	}
	cout << '\n';
}

void print_board(const vector<vector<int>>& board)
{
	cout << "print board:\n";
	char conv[] = { '-','X','O' };
	for (int i = -1; i < 8; ++i) {
		for (int j = -1; j < 8; ++j) {
			if (i == -1 && j == -1) {
				cout << '~' << "  ";
				continue;
			}
			if (i == -1) {
				cout << j << "  ";
				continue;
			}
			if (j == -1) {
				cout << i << "  ";
				continue;
			}
			cout << conv[board[i][j]] << "  ";
		}
		cout << '\n';
	}
	cout << '\n';
}

vector<vector<int>> simulation(vector<vector<int>> board, const move_t& move)
{
	/*cout << "enter simul\n";
	print_move(move);
	cout << "after print\n";*/
	if (move.steps.size() == 0) return board;
	int player = board[move.steps[0].row][move.steps[0].col];
	for (unsigned int i = 0; i + 1 < move.steps.size(); ++i) {
		coord_t pt1 = move.steps[i], pt2 = move.steps[i + 1];
		coord_t midpt = coord_t((pt1.row + pt2.row) / 2, (pt1.col + pt2.col) / 2);

		board[pt1.row][pt1.col] = EMPTY;
		if (board[midpt.row][midpt.col] != player) {
			board[midpt.row][midpt.col] = EMPTY;
		}
		board[pt2.row][pt2.col] = player;
	}
	//cout << "exit simul\n";
	return board;
}