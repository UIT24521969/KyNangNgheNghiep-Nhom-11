#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <cstring>
#include <vector>
#include <algorithm>

using namespace std;

#define H 24 
#define W 15
const char BLOCK = (char)219;

char board[H][W];
int x = W / 2 - 2, y = 0;
int score = 0;

vector<int> highScores;

/* =======================
   KHAI BÁO TRƯỚC (PROTOTYPES)
======================= */
class Piece;
Piece* current = nullptr;
Piece* nextP = nullptr;

void setColor(int color);
void gotoxy(int x, int y);
void initBoard();
void draw();
void drawNextPiece();
void resetGame();
void showLeaderboard();
bool showGameOver();
class Piece* randomPiece();

/* =======================
   CLASS PIECE & CÁC KHỐI
======================= */
class Piece {
protected:
    char shape[4][4];
    int color;
public:
    virtual ~Piece() {}
    virtual void rotate() = 0;
    char(*getShape())[4] { return shape; }
    int getColor() { return color; }
};

// ... (Các class khối gạch: LPiece, ZPiece, DotPiece, SmallPiece, IPiece, OPiece, TPiece giữ nguyên như bạn đã viết)
class LPiece : public Piece {
public:
    LPiece() {
        color = 6; // Màu Cam (Orange-ish)
        char t[4][4] = {
            {' ', 'L', ' ', ' '},
            {' ', 'L', ' ', ' '},
            {' ', 'L', 'L', ' '},
            {' ', ' ', ' ', ' '}
        };
        memcpy(shape, t, sizeof(shape));
    }
    void rotate() override {
        char t[4][4];
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) t[j][3 - i] = shape[i][j];
        memcpy(shape, t, sizeof(shape));
    }
};

class ZPiece : public Piece {
public:
    ZPiece() {
        color = 12; // Màu Đỏ (Red)
        char t[4][4] = {
            {'Z', 'Z', ' ', ' '},
            {' ', 'Z', 'Z', ' '},
            {' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' '}
        };
        memcpy(shape, t, sizeof(shape));
    }
    void rotate() override {
        char t[4][4];
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) t[j][3 - i] = shape[i][j];
        memcpy(shape, t, sizeof(shape));
    }
};

class DotPiece : public Piece {
public:
    DotPiece() {
        color = 15; // Màu Trắng (White)
        char t[4][4] = {
            {' ', ' ', ' ', ' '},
            {' ', 'D', ' ', ' '},
            {' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' '}
        };
        memcpy(shape, t, sizeof(shape));
    }
    void rotate() override {} // 1x1 xoay không đổi
};

class SmallPiece : public Piece {
public:
    SmallPiece() {
        color = 10; // Màu xanh lá sáng (Light Green)
        char t[4][4] = {
            {' ', ' ', ' ', ' '},
            {' ', 'S', 'S', ' '},
            {' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' '}
        };
        memcpy(shape, t, sizeof(shape));
    }
    void rotate() override {
        char t[4][4];
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) t[j][3 - i] = shape[i][j];
        memcpy(shape, t, sizeof(shape));
    }
};

class IPiece : public Piece {
public:
    IPiece() {
        color = 11; // Cyan
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
            for (int j = 0; j < 4; j++) t[j][3 - i] = shape[i][j];
        memcpy(shape, t, sizeof(shape));
    }
};

class OPiece : public Piece {
public:
    OPiece() {
        color = 14; // Yellow
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
        color = 13; // Magenta
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
            for (int j = 0; j < 4; j++) t[j][3 - i] = shape[i][j];
        memcpy(shape, t, sizeof(shape));
    }
};
/* =======================
   LOGIC HÀM PHỤ
======================= */
void setColor(int color) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); }
void gotoxy(int x, int y) { COORD c = { (SHORT)x, (SHORT)y }; SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c); }
void hideCursor() { HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); CONSOLE_CURSOR_INFO i; i.dwSize = 100; i.bVisible = FALSE; SetConsoleCursorInfo(h, &i); }

void initBoard() {
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            board[i][j] = (i == H - 1 || j == 0 || j == W - 1) ? '#' : ' ';
}

void drawNextPiece() {
    if (!nextP) return;
    int sX = W * 2 + 5, sY = 5;
    gotoxy(sX, sY - 1); setColor(15); cout << "NEXT PIECE:";
    auto s = nextP->getShape();
    for (int i = 0; i < 4; i++) {
        gotoxy(sX, sY + i);
        for (int j = 0; j < 4; j++) {
            if (s[i][j] != ' ') { setColor(nextP->getColor()); cout << BLOCK << BLOCK; }
            else cout << "  ";
        }
    }
}

void draw() {
    gotoxy(0, 0);
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (board[i][j] == '#') { setColor(8); cout << "[]"; }
            else if (board[i][j] != ' ') {
                if (board[i][j] == 'I') setColor(11);
                else if (board[i][j] == 'O') setColor(14);
                else if (board[i][j] == 'T') setColor(13);
                else if (board[i][j] == 'L') setColor(6);
                else if (board[i][j] == 'Z') setColor(12);
                else if (board[i][j] == 'D') setColor(15);
                else if (board[i][j] == 'S') setColor(10);
                cout << BLOCK << BLOCK;
            }
            else cout << "  ";
        }
        if (i == 2) { setColor(15); cout << "   SCORE: "; setColor(10); cout << score; }
        cout << endl;
    }
}

// ... (Các hàm boardDelBlock, block2Board, canMove, removeLine giữ nguyên logic của bạn)
void boardDelBlock() { auto s = current->getShape(); for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) if (s[i][j] != ' ') board[y + i][x + j] = ' '; }
void block2Board() { auto s = current->getShape(); for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) if (s[i][j] != ' ') board[y + i][x + j] = s[i][j]; }
bool canMove(int dx, int dy) {
    auto s = current->getShape();
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) if (s[i][j] != ' ') {
        int tx = x + j + dx, ty = y + i + dy;
        if (tx < 1 || tx >= W - 1 || ty >= H - 1 || board[ty][tx] != ' ') return false;
    }
    return true;
}
void removeLine() {
    int lc = 0;
    for (int i = H - 2; i > 0; i--) {
        bool f = true; for (int j = 1; j < W - 1; j++) if (board[i][j] == ' ') { f = false; break; }
        if (f) { for (int k = i; k > 0; k--) for (int j = 1; j < W - 1; j++) board[k][j] = board[k - 1][j]; i++; lc++; }
    }
    if (lc == 1) score += 100; else if (lc == 2) score += 500; else if (lc == 3) score += 1000; else if (lc >= 4) score += 2000;
}

Piece* randomPiece() {
    int r = rand() % 7;
    if (r == 0) return new IPiece(); if (r == 1) return new OPiece();
    if (r == 2) return new TPiece(); if (r == 3) return new LPiece();
    if (r == 4) return new ZPiece(); if (r == 5) return new DotPiece();
    return new SmallPiece();
}

void resetGame() {
    score = 0; x = W / 2 - 2; y = 0;
    initBoard();
    if (current) { delete current; current = nullptr; }
    if (nextP) { delete nextP; nextP = nullptr; }
    current = randomPiece();
    nextP = randomPiece();
}

void showLeaderboard() {
    sort(highScores.begin(), highScores.end(), greater<int>());
    cout << "\n\n   RANKING (Current Session):" << endl;
    for (int i = 0; i < highScores.size() && i < 5; i++) {
        setColor(14); cout << "   " << i + 1 << ". " << highScores[i] << " pts" << endl;
    }
    setColor(7);
}

bool showGameOver() {
    highScores.push_back(score);
    system("cls");
    int mX = 5, mY = H / 2 - 3;
    setColor(12);
    gotoxy(mX, mY);     cout << "======================";
    gotoxy(mX, mY + 1); cout << "      GAME OVER!      ";
    gotoxy(mX, mY + 2); cout << "   FINAL SCORE: " << score;
    gotoxy(mX, mY + 3); cout << "======================";
    showLeaderboard();
    setColor(10); gotoxy(mX, mY + 10); cout << "Press 'R' to RESTART";
    setColor(7);  gotoxy(mX, mY + 11); cout << "Press any other key to EXIT...";
    char choice = _getch();
    return (choice == 'r' || choice == 'R');
}

void showStartScreen() {
    system("cls");
    int mX = 10, mY = H / 2 - 5;
    setColor(11);
    gotoxy(mX, mY); cout << "==============================";
    gotoxy(mX, mY + 1); cout << "      T E T R I S  G A M E    ";
    gotoxy(mX, mY + 2); cout << "==============================";
    setColor(14);
    gotoxy(mX + 2, mY + 5); cout << "W: Rotate      S: Speed Down";
    gotoxy(mX + 2, mY + 6); cout << "A: Move Left   D: Move Right";
    setColor(10);
    gotoxy(mX + 4, mY + 9); cout << "PRESS 'ENTER' TO START";
    while (true) { if (_kbhit() && _getch() == 13) break; }
    system("cls");
}

/* =======================
   MAIN
======================= */
int main() {
    srand((unsigned int)time(NULL));
    hideCursor();
    showStartScreen();

    bool restart = true;
    while (restart) {
        resetGame();
        while (true) {
            boardDelBlock();
            if (_kbhit()) {
                char c = _getch();
                if (c == 'a' && canMove(-1, 0)) x--;
                if (c == 'd' && canMove(1, 0))  x++;
                if (c == 's' && canMove(0, 1)) { y++; block2Board(); draw(); drawNextPiece(); boardDelBlock(); }
                if (c == 'w') { current->rotate(); if (!canMove(0, 0)) { current->rotate(); current->rotate(); current->rotate(); } }
                if (c == 'q') { restart = false; break; }
            }

            if (canMove(0, 1)) y++;
            else {
                block2Board(); removeLine(); delete current;
                current = nextP; nextP = randomPiece();
                x = W / 2 - 2; y = 0;
                if (!canMove(0, 0)) { restart = showGameOver(); break; }
            }
            block2Board(); draw(); drawNextPiece();
            Sleep(max(50, 200 - (score / 500) * 10));
        }
    }
    return 0;
}