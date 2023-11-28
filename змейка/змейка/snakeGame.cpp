#include <iostream>
#include <vector>
#include <deque>
#include <iterator>
#include <time.h>
#include <Windows.h>
#include <conio.h>
#include <random>

using namespace std;

void gotoxy(int x, int y) {
    COORD pos = { x, y };
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(output, pos);
}

void HandleInput(int& direction, const int UP, const int DOWN, const int RIGHT, const int LEFT) {
    if (_kbhit()) {
        char key = _getch();
        switch (key) {
        case 'w':
            if (direction != DOWN) direction = UP;
            break;
        case 's':
            if (direction != UP) direction = DOWN;
            break;
        case 'a':
            if (direction != RIGHT) direction = LEFT;
            break;
        case 'd':
            if (direction != LEFT) direction = RIGHT;
            break;
        }
    }
}

void PrintField(const vector<vector<char>>& field, const int HEIGHT, const int WIDTH) {
    for (int i = 0; i < HEIGHT; i++) {
        copy(field[i].begin(), field[i].begin() + WIDTH, ostream_iterator<char>(cout, " "));
        cout << endl;
    }
}

void FillField(vector<vector<char>>& field, const int HEIGHT, const int WIDTH) {
    for (int i = 0; i < HEIGHT; i++) {
        vector<char> tempVector(WIDTH);
        for (int j = 0; j < WIDTH; j++) {
            tempVector[j] = '.';
        }
        field[i] = tempVector;
    }
}

struct Position {
    int X;
    int Y;
};

bool CheckSelfCollision(const deque<Position>& snake) {
    Position head = snake.front();
    for (auto it = snake.begin() + 1; it != snake.end(); ++it) {
        if (head.X == it->X && head.Y == it->Y) {
            return true; // Столкновение с хвостом
        }
    }
    return false; // Нет столкновения с хвостом
}

bool CheckApplePos(const deque<Position>& snake, const Position apple)
{
    for (const auto& segment : snake)
    {
        if (apple.X == segment.X && apple.Y == segment.Y)
        {
            return true;
        }
    }
    return false;
}

Position GenerateApplePosition(const deque<Position>& snake, const int FIELD_WIDTH, const int FIELD_HEIGHT) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> widthDistribution(0, FIELD_WIDTH - 1);
    uniform_int_distribution<int> heightDistribution(0, FIELD_HEIGHT - 1);

    Position applePos;
    bool isOnSnake;
    do {
        applePos.X = widthDistribution(gen);
        applePos.Y = heightDistribution(gen);

        isOnSnake = false;
        for (const auto& segment : snake) {
            if (applePos.X == segment.X && applePos.Y == segment.Y) {
                isOnSnake = true;
                break;
            }
        }
    } while (isOnSnake);
    return applePos;
}

int main() {
    setlocale(0, "");
    srand(time(NULL));
    ShowCursor(FALSE);

    const int FIELD_HEIGHT = 4;
    const int FIELD_WIDTH = 4;

    const int MAX_SNAKE_LENGTH = FIELD_HEIGHT * FIELD_WIDTH;

    const int DOWN = 0;
    const int UP = 1;
    const int LEFT = 2;
    const int RIGHT = 3;

    int direction = UP;

    bool isContinue = true;

    vector <vector<char>> gameField(FIELD_HEIGHT, vector<char>(FIELD_WIDTH));
    int snakeLength = 1;
    deque<Position> snake;
    char apple = '+';

    FillField(gameField, FIELD_HEIGHT, FIELD_WIDTH);

    Position snakePos;
    snakePos.X = FIELD_WIDTH / 2;
    snakePos.Y = FIELD_HEIGHT / 2;

    snake.push_back(snakePos);

    Position applePos = GenerateApplePosition(snake, FIELD_WIDTH, FIELD_HEIGHT);

    while (isContinue) {
        gotoxy(0, 0);

        cout << "Ваш счёт: " << snakeLength << endl;

        // Очистка только тех мест, где были элементы (тело змейки и яблоко)
        gameField[applePos.Y][applePos.X] = '.';
        for (const auto& segment : snake) {
            gameField[segment.Y][segment.X] = '.';
        }

        HandleInput(direction, UP, DOWN, RIGHT, LEFT);

        // Обновление позиции змейки в зависимости от направления
        Position newHead = snake.front();
        switch (direction) {
        case UP:
            newHead.Y--;
            break;
        case DOWN:
            newHead.Y++;
            break;
        case LEFT:
            newHead.X--;
            break;
        case RIGHT:
            newHead.X++;
            break;
        }

        if (snakeLength == MAX_SNAKE_LENGTH)
        {
            isContinue = false;
            gotoxy(2, FIELD_HEIGHT / 2);
            cout << "ВЫ ПОБЕДИЛИ!" << endl;
            gotoxy(0, FIELD_HEIGHT);
            break;
        }

        // Проверка на столкновение с границами поля
        if (newHead.X < 0 || newHead.X >= FIELD_WIDTH || newHead.Y < 0 || newHead.Y >= FIELD_HEIGHT) {
            isContinue = false;
            gotoxy(2, FIELD_HEIGHT / 2);
            cout << "Конец игры! Ваш счет: " << snakeLength << endl;
            gotoxy(0, FIELD_HEIGHT);
            break;
            // Игра завершается при столкновении со стеной
        }

        // Проверка на столкновение с хвостом
        if (CheckSelfCollision(snake)) {
            isContinue = false;
            gotoxy(2, FIELD_HEIGHT / 2);
            cout << "Конец игры! Столкновение с хвостом. Ваш счет: " << snakeLength << endl;
            gotoxy(0, FIELD_HEIGHT);
            break;
        }


        if (newHead.X == applePos.X && newHead.Y == applePos.Y) {
            applePos = GenerateApplePosition(snake, FIELD_WIDTH, FIELD_HEIGHT);
            snakeLength++;
            gameField[applePos.Y][applePos.X] = apple;
        }

        snake.push_front(newHead);
        for (const auto& segment : snake) {
            gameField[segment.Y][segment.X] = 'O';
        }

        // Удаление хвоста змейки, если её длина превышает максимальное значение
        if (snake.size() > snakeLength) {
            gameField[snake.back().Y][snake.back().X] = '.';
            snake.pop_back();
        }

        gameField[applePos.Y][applePos.X] = apple;

        PrintField(gameField, FIELD_HEIGHT, FIELD_WIDTH);

        Sleep(500);
    }
    system("pause");
    return 0;
}