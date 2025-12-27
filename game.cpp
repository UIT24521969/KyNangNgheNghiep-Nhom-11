#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <cstring>
#include <algorithm>

using namespace std;

#define H 24 
#define W 15
const char BLOCK = (char)219;

/* ============================================================
   MODULE QUẢN LÝ TỐC ĐỘ (SPEED MANAGER)
   Tách biệt logic để dễ dàng bảo trì và báo cáo
============================================================ */
class SpeedController {
private:
    int currentDelay;
    int minDelay;
    int step;

public:
    SpeedController(int start = 200, int minD = 50, int s = 15)
        : currentDelay(start), minDelay(minD), step(s) {
    }

    // Tăng tốc độ dựa trên số hàng xóa được
    void updateSpeed(int linesCleared) {
        if (linesCleared > 0) {
            currentDelay -= (linesCleared * step);
            if (currentDelay < minDelay) currentDelay = minDelay;
        }
    }

    int getDelay() const { return currentDelay; }
};

/* =======================
   BIẾN TOÀN CỤC & HÀM PHỤ
======================= */
char board[H][W];
int x = W / 2 - 2, y = 0;
int score = 0;

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void gotoxy(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

/* =======================
   HỆ THỐNG CÁC KHỐI GẠCH
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

class LPiece : public Piece {
public:
    LPiece() {
        color = 6;
        char t[4][4] = { {' ','L',' ',' '}, {' ','L',' ',' '}, {' ','L','L',' '}, {' ',' ',' ',' '} };
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
        color = 12;
        char t[4][4] = { {'Z','Z',' ',' '}, {' ','Z','Z',' '}, {' ',' ',' ',' '}, {' ',' ',' ',' '} };
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
    DotPiece() { color = 15; char t[4][4] = { {' ',' ',' ',' '}, {' ','D',' ',' '}, {' ',' ',' ',' '}, {' ',' ',' ',' '} }; memcpy(shape, t, sizeof(shape)); }
    void rotate() override {}
};

class SmallPiece : public Piece {
public:
    SmallPiece() {
        color = 10;
        char t[4][4] = { {' ',' ',' ',' '}, {' ','S','S',' '}, {' ',' ',' ',' '}, {' ',' ',' ',' '} };
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
        color = 11;
        char t[4][4] = { {' ','I',' ',' '}, {' ','I',' ',' '}, {' ','I',' ',' '}, {' ','I',' ',' '} };
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
    OPiece() { color = 14; char t[4][4] = { {' ',' ',' ',' '}, {' ','O','O',' '}, {' ','O','O',' '}, {' ',' ',' ',' '} }; memcpy(shape, t, sizeof(shape)); }
    void rotate() override {}
};

class TPiece : public Piece {
public:
    TPiece() {
        color = 13;
        char t[4][4] = { {' ',' ',' ',' '}, {' ','T',' ',' '}, {'T','T','T',' '}, {' ',' ',' ',' '} };
        memcpy(shape, t, sizeof(shape));
    }
    void rotate() override {
        char t[4][4];
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) t[j][3 - i] = shape[i][j];
        memcpy(shape, t, sizeof(shape));
    }
};

Piece* current = nullptr;

/* =======================
   LOGIC GAME
======================= */
void initBoard() {
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            board[i][j] = (i == H - 1 || j == 0 || j == W - 1) ? '#' : ' ';
}

void draw(int currentDelay) {
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
        if (i == 4) { setColor(15); cout << "   SPEED: "; setColor(11); cout << currentDelay << "ms "; }
        cout << endl;
    }
}

void boardDelBlock() {
    auto s = current->getShape();
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (s[i][j] != ' ') board[y + i][x + j] = ' ';
}

void block2Board() {
    auto s = current->getShape();
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (s[i][j] != ' ') board[y + i][x + j] = s[i][j];
}

bool canMove(int dx, int dy) {
    auto s = current->getShape();
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (s[i][j] != ' ') {
                int tx = x + j + dx; int ty = y + i + dy;
                if (tx < 1 || tx >= W - 1 || ty >= H - 1) return false;
                if (board[ty][tx] != ' ') return false;
            }
    return true;
}

// Sửa hàm removeLine để báo cáo số dòng xóa được
int removeLine() {
    int linesCleared = 0;
    for (int i = H - 2; i > 0; i--) {
        bool full = true;
        for (int j = 1; j < W - 1; j++) if (board[i][j] == ' ') { full = false; break; }
        if (full) {
            for (int k = i; k > 0; k--)
                for (int j = 1; j < W - 1; j++) board[k][j] = board[k - 1][j];
            i++; linesCleared++;
        }
    }
    if (linesCleared == 1) score += 100;
    else if (linesCleared == 2) score += 500;
    else if (linesCleared == 3) score += 1000;
    else if (linesCleared >= 4) score += 2000;
    return linesCleared;
}

void showGameOver() {
    system("cls");
    int midX = 5, midY = H / 2;
    setColor(12); gotoxy(midX, midY); cout << "======================";
    gotoxy(midX, midY + 1); cout << "      GAME OVER!      ";
    gotoxy(midX, midY + 2); cout << "   FINAL SCORE: " << score;
    gotoxy(midX, midY + 3); cout << "======================";
    setColor(7); gotoxy(midX, midY + 5); cout << "Nhan phim bat ky de thoat...";
    _getch();
}

Piece* randomPiece() {
    int r = rand() % 7;
    if (r == 0) return new IPiece();
    if (r == 1) return new OPiece();
    if (r == 2) return new TPiece();
    if (r == 3) return new LPiece();
    if (r == 4) return new ZPiece();
    if (r == 5) return new DotPiece();
    return new SmallPiece();
}

/* =======================
   MAIN
======================= */
int main() {
    srand((unsigned int)time(NULL));
    system("cls");
    hideCursor();

    initBoard();
    current = randomPiece();
    SpeedController speedManager(200, 50, 15); // Khởi tạo bộ quản lý tốc độ

    while (true) {
        boardDelBlock();

        if (_kbhit()) {
            char c = _getch();
            if (c == 'a' && canMove(-1, 0)) x--;
            if (c == 'd' && canMove(1, 0))  x++;
            if (c == 's' && canMove(0, 1)) {
                y++;
                block2Board(); draw(speedManager.getDelay()); boardDelBlock();
            }
            if (c == 'w') {
                current->rotate();
                if (!canMove(0, 0)) { /* Logic hoàn tác nếu cần */ }
            }
            if (c == 'q') break;
        }

        if (canMove(0, 1)) y++;
        else {
            block2Board();
            int cleared = removeLine();
            speedManager.updateSpeed(cleared); //  Cập nhật tốc độ dựa trên số hàng vừa xóa

            delete current;
            current = randomPiece();
            x = W / 2 - 2; y = 0;

            if (!canMove(0, 0)) { showGameOver(); break; }
        }

        block2Board();
        draw(speedManager.getDelay());
        Sleep(speedManager.getDelay()); // Dùng tốc độ từ bộ điều khiển
    }
    delete current;
    return 0;
}