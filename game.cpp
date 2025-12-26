#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <cstring>

using namespace std;

#define H 20
#define W 15

char board[H][W];
int x = 5, y = 0;

/* =======================
   CLASS PIECE (CHA)
======================= */
class Piece {
protected:
    char shape[4][4];
public:
    virtual ~Piece() {}
    virtual void rotate() = 0;

    char(*getShape())[4] {
        return shape;
        }
};

/* =======================
   CÁC KHỐI
======================= */
class IPiece : public Piece {
public:
    IPiece() {
        char t[4][4] = {
            {' ','I',' ',' '},
            {' ','I',' ',' '},
            {' ','I',' ',' '},
            {' ','I',' ',' '}
        };
        memcpy(shape, t, sizeof(shape));
    }
    void rotate() override {
        char t[4][4];
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                t[j][3 - i] = shape[i][j];
        memcpy(shape, t, sizeof(shape));
    }
};

class OPiece : public Piece {
public:
    OPiece() {
        char t[4][4] = {
            {' ',' ',' ',' '},
            {' ','O','O',' '},
            {' ','O','O',' '},
            {' ',' ',' ',' '}
        };
        memcpy(shape, t, sizeof(shape));
    }
    void rotate() override {}
};

class TPiece : public Piece {
public:
    TPiece() {
        char t[4][4] = {
            {' ',' ',' ',' '},
            {' ','T',' ',' '},
            {'T','T','T',' '},
            {' ',' ',' ',' '}
        };
        memcpy(shape, t, sizeof(shape));
    }
    void rotate() override {
        char t[4][4];
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                t[j][3 - i] = shape[i][j];
        memcpy(shape, t, sizeof(shape));
    }
};

Piece* current = nullptr;

/* =======================
   HÀM PHỤ
======================= */
void gotoxy(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void initBoard() {
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            board[i][j] = (i == H - 1 || j == 0 || j == W - 1) ? '#' : ' ';
}

void draw() {
    gotoxy(0, 0);
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++)
            cout << board[i][j];
        cout << endl;
    }
}

void boardDelBlock() {
    auto s = current->getShape();
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (s[i][j] != ' ')
                board[y + i][x + j] = ' ';
}

void block2Board() {
    auto s = current->getShape();
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (s[i][j] != ' ')
                board[y + i][x + j] = s[i][j];
}

bool canMove(int dx, int dy) {
    auto s = current->getShape();
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (s[i][j] != ' ') {
                int tx = x + j + dx;
                int ty = y + i + dy;
                if (tx < 1 || tx >= W - 1 || ty >= H - 1) return false;
                if (board[ty][tx] != ' ') return false;
            }
    return true;
}

void removeLine() {
    for (int i = H - 2; i > 0; i--) {
        bool full = true;
        for (int j = 1; j < W - 1; j++)
            if (board[i][j] == ' ') {
                full = false;
                break;
            }
        if (full) {
            for (int k = i; k > 0; k--)
                for (int j = 1; j < W - 1; j++)
                    board[k][j] = board[k - 1][j];
            i++;
        }
    }
}

Piece* randomPiece() {
    int r = rand() % 3;
    if (r == 0) return new IPiece();
    if (r == 1) return new OPiece();
    return new TPiece();
}

/* =======================
   MAIN
======================= */
int main() {
    srand((unsigned int)time(NULL));
    system("cls");

    initBoard();
    current = randomPiece();

    while (true) {
        boardDelBlock();

        if (_kbhit()) {
            char c = _getch();
            if (c == 'a' && canMove(-1, 0)) x--;
            if (c == 'd' && canMove(1, 0))  x++;
            if (c == 's' && canMove(0, 1))  y++;
            if (c == 'w') current->rotate();
            if (c == 'q') break;
        }

        if (canMove(0, 1)) y++;
        else {
            block2Board();
            removeLine();
            delete current;
            current = randomPiece();
            x = 5; y = 0;
        }

        block2Board();
        draw();
        Sleep(200);
    }

    return 0;
}
