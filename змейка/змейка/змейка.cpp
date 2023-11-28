#include <iostream>
#include <vector>
#include <deque>
#include <conio.h>
#include <windows.h>
#include <algorithm>
#include <random>
#include <utility>

using namespace std;

struct Cordinate {
	int x;
	int y;
};

const int fieldWidth = 22;
const int fieldHeidth = 22;
const int maxLengthSnake = (fieldWidth - 2) * (fieldHeidth - 2);

int score;
bool newFruit = true;
Cordinate fruit;
deque<Cordinate> snake;
vector<vector<char>> gameField(fieldHeidth, vector<char>(fieldWidth));
enum Direction { Stop = 0, Left, Right, Up, Down };
Direction dir;

void GoToXY(int x, int y) {
	COORD pos = { x,y };
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(output, pos);
}

Cordinate RandomAvaibleCoordinate(vector<Cordinate> Coordinate) {
	random_device rd;
	mt19937 rng(rd());
	shuffle(Coordinate.begin(), Coordinate.end(), rng);
	Cordinate randomCoordinate = Coordinate[0];
	return randomCoordinate;
}

void ChooseDiraction(bool GameOver) {
	if (_kbhit()) {
		switch (_getch())
		{
		case ('a'):
			if (dir != Right) dir = Left;
			break;
		case ('w'):
			if (dir != Down) dir = Up;
			break;
		case ('d'):
			if (dir != Left) dir = Right;
			break;
		case ('s'):
			if (dir != Up) dir = Down;
			break;
		case ('x'):
			GameOver = true;
			break;
		}
	}
}

Cordinate ChangePositionSnakeHead(Cordinate snakeHead) {
	switch (dir)
	{
	case Left:
		snakeHead.x--;
		break;
	case Right:
		snakeHead.x++;
		break;
	case Up:
		snakeHead.y--;
		break;
	case Down:
		snakeHead.y++;
		break;
	}
	return snakeHead;
}

Cordinate SnakeTeleport(Cordinate Head) {
	if (Head.x >= fieldWidth - 1) Head.x = 1;
	else if (Head.y >= fieldHeidth - 1) Head.y = 1;
	else if (Head.y == 0) Head.y = fieldHeidth - 2;
	else if (Head.x == 0) Head.x = fieldWidth - 2;
	return Head;
}

bool WinGame(int snakeLength) {
	if (snakeLength >= maxLengthSnake) return true;
	return false;
}

bool LoseGame(Cordinate Head) {
	for (int i = 1; i < snake.size(); i++) {
		if (Head.x == snake[i].x && Head.y == snake[i].y) {
			return true;
		}
	}
	return false;
}

void SnakeMove(Cordinate Last) {
	for (int i = 1; i < snake.size(); i++) {
		Cordinate temp = snake[i];
		snake[i] = Last;
		Last = temp;
	};
}

bool CheckingPosition(int y, int x, deque<Cordinate> AllCheck) {
	deque<Cordinate> temp = AllCheck;
	while (!temp.empty()) {
		Cordinate CurrentElem = temp.front();
		if (CurrentElem.x == x && CurrentElem.y == y) return true;
		temp.pop_front();
	}
	return false;
}

void DrawSnake() {
	for (int i = 0; i < snake.size(); i++) {
		if (i == 0) {
			gameField[snake[i].y][snake[i].x] = '&';
		}
		else {
			gameField[snake[i].y][snake[i].x] = 'o';
		}
	}
}

void DrawFruit(vector<Cordinate> AvailableCoordinates) {
	Cordinate CurrentHead = snake.front();
	if (CurrentHead.x == fruit.x && CurrentHead.y == fruit.y) {
		newFruit = true;
	}
	//генерация нового яблока
	if (newFruit) {
		fruit = RandomAvaibleCoordinate(AvailableCoordinates);
		gameField[fruit.y][fruit.x] = '$';
		AvailableCoordinates.clear();
		newFruit = false;
	}
	else {
		gameField[fruit.y][fruit.x] = '$';
	}
}

void Instalisation(bool gameOver) {
	gameOver = false;
	snake.push_front({ fieldWidth / 2, fieldHeidth / 2 });
	snake.push_back({ fieldWidth / 2, fieldHeidth / 2 + 1 });
	dir = Up;
}

bool Logic(bool gameOver) {
	ChooseDiraction(gameOver);
	Cordinate head = snake.front();
	Cordinate last = head;
	Cordinate Back_tail = snake.back();

	head = ChangePositionSnakeHead(head);
	if (head.x == fruit.x && head.y == fruit.y) {
		score++;
		snake.push_back(Back_tail);
	}

	head = SnakeTeleport(head);

	gameOver = LoseGame(head);
	if (!gameOver) gameOver = WinGame(score + 2);

	for (int i = 1; i < snake.size(); i++) {
		Cordinate temp = snake[i];
		snake[i] = last;
		last = temp;
	};

	snake.pop_front();
	snake.push_front(head);
	return gameOver;
}


void CreateField() {
	vector<Cordinate> AvailableCoordinates;
	for (int i = 0; i < gameField.size(); i++) {
		for (int j = 0; j < gameField[i].size(); j++) {
			if (CheckingPosition(i, j, snake)) {
				gameField[i][j] = ' ';
			}
			if ((i == 0) || (i + 1 == fieldHeidth)) {
				gameField[i][j] = '#';
			}
			else if ((j == 0) || (j + 1 == fieldWidth)) {
				gameField[i][j] = '#';
			}
			else {
				gameField[i][j] = ' ';
				AvailableCoordinates.push_back({ j, i });
			}
		}
	}
	DrawSnake();
	DrawFruit(AvailableCoordinates);
}

void PrintField() {
	GoToXY(0, 0);
	cout << "Score: " << score << endl;
	CreateField();
	for (int i = 0; i < gameField.size(); i++) {
		for (int j = 0; j < gameField[i].size(); j++) {
			cout << gameField[i][j];
		}
		cout << endl;
	}
}

int main() {
	bool gameOver = false;
	Instalisation(gameOver);
	int time = clock();
	while (!gameOver)
	{
		gameOver = Logic(gameOver);
		PrintField();
		Sleep(300);
	}
	system("cls");
	cout << "Game over" << endl;
	cout << "Your Score: " << score;
	return 0;
}

