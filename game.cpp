#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <cstring>

using namespace std;

#define H 24 
#define W 15
const char BLOCK = (char)219; // Ký tự khối đặc █

char board[H][W];
int x = W / 2 - 2, y = 0;
int score = 0; // Biến lưu trữ điểm

/* =======================
   HÀM HỖ TRỢ MÀU SẮC & CURSOR
======================= */
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
   CLASS PIECE
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
    OPiece() {
        color = 14; // Yellow
        char t[4][4] = { {' ',' ',' ',' '}, {' ','O','O',' '}, {' ','O','O',' '}, {' ',' ',' ',' '} };
        memcpy(shape, t, sizeof(shape));
    }
    void rotate() override {}
};

class TPiece : public Piece {
public:
    TPiece() {
        color = 13; // Magenta
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

void draw() {
    gotoxy(0, 0);
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (board[i][j] == '#') {
                setColor(8); cout << "[]";
            }
            else if (board[i][j] != ' ') {
                // Tự động lấy màu dựa trên ký tự trong board
                if (board[i][j] == 'I') setColor(11);
                else if (board[i][j] == 'O') setColor(14);
                else if (board[i][j] == 'T') setColor(13);
                else if (board[i][j] == 'L') setColor(6);
                else if (board[i][j] == 'Z') setColor(12);
                else if (board[i][j] == 'D') setColor(15);
                else if (board[i][j] == 'S') setColor(10);
                cout << BLOCK << BLOCK;
            }
            else {
                cout << "  ";
            }
        }

        // --- HIỂN THỊ ĐIỂM SỐ Ở ĐÂY ---
        if (i == 2) {
            setColor(15); // Màu trắng
            cout << "   SCORE: ";
            setColor(10); // Màu xanh lá cho điểm
            cout << score;
        }
        /*if (i == 4) {
            setColor(15);
            cout << "   KICH THUOC: " << W - 2 << "x" << H - 2;
        }*/

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
                int tx = x + j + dx;
                int ty = y + i + dy;
                if (tx < 1 || tx >= W - 1 || ty >= H - 1) return false;
                if (board[ty][tx] != ' ' && board[ty][tx] != 'I' && board[ty][tx] != 'O' && board[ty][tx] != 'T') {}
                // Logic kiểm tra va chạm với các khối đã cố định
                if (board[ty][tx] != ' ') return false;
            }
    return true;
}

void removeLine() {
    int linesCleared = 0;
    for (int i = H - 2; i > 0; i--) {
        bool full = true;
        for (int j = 1; j < W - 1; j++) {
            if (board[i][j] == ' ') {
                full = false;
                break;
            }
        }
        if (full) {
            for (int k = i; k > 0; k--)
                for (int j = 1; j < W - 1; j++)
                    board[k][j] = board[k - 1][j];
            i++;
            linesCleared++; // Đếm số hàng bị xóa
        }
    }

    // Cộng điểm dựa trên số hàng xóa được
    if (linesCleared == 1) score += 100;
    else if (linesCleared == 2) score += 500;
    else if (linesCleared == 3) score += 1000;
    else if (linesCleared >= 4) score += 2000; // Tetris!
}

void showStartScreen() {
    system("cls");
    int midX = 10, midY = H / 2 - 5;
    
    setColor(11); // Màu Cyan
    gotoxy(midX, midY);     cout << "==============================";
    gotoxy(midX, midY + 1); cout << "      T E T R I S  G A M E    ";
    gotoxy(midX, midY + 2); cout << "==============================";
    
    setColor(14); // Màu Vàng
    gotoxy(midX + 2, midY + 5); cout << "W: Rotate      S: Speed Down";
    gotoxy(midX + 2, midY + 6); cout << "A: Move Left   D: Move Right";
    
    setColor(10); // Màu Xanh lá
    gotoxy(midX + 4, midY + 9); cout << "PRESS 'ENTER' TO START";
    
    setColor(7); // Trắng lại
    
    // Vòng lặp chờ phím Enter
    while (true) {
        if (_kbhit()) {
            char c = _getch();
            if (c == 13) break; // 13 là mã ASCII của phím Enter
        }
    }
    system("cls");
}

void showGameOver() {
    system("cls");
    int midX = 5, midY = H / 2;
    setColor(12); // Màu đỏ
    gotoxy(midX, midY);     cout << "======================";
    gotoxy(midX, midY + 1); cout << "      GAME OVER!      ";
    gotoxy(midX, midY + 2); cout << "   FINAL SCORE: " << score;
    gotoxy(midX, midY + 3); cout << "======================";

    setColor(7);
    gotoxy(midX, midY + 5);
    cout << "Nhan phim bat ky de thoat...";
    _getch();
}

Piece* randomPiece() {
    int r = rand() % 7; // I, O, T, L, Z, Dot, Small(2x1)
    if (r == 0) return new IPiece();
    if (r == 1) return new OPiece();
    if (r == 2) return new TPiece();
    if (r == 3) return new LPiece();
    if (r == 4) return new ZPiece();
    if (r == 5) return new DotPiece();
    return new SmallPiece(); // Trả về khối 2x1
}

int main() {
    srand((unsigned int)time(NULL));
    system("cls");
    hideCursor();

    showStartScreen();

    initBoard();
    current = randomPiece();

    while (true) {
        boardDelBlock();

        if (_kbhit()) {
            char c = _getch();
            if (c == 'a' && canMove(-1, 0)) x--;
            if (c == 'd' && canMove(1, 0))  x++;
            if (c == 's' && canMove(0, 1)) {
                y++;
                // Vẽ cập nhật ngay lập tức để thấy tốc độ nhanh hơn
                block2Board();
                draw();
                boardDelBlock();
            }
            if (c == 'w') {
                // Tạm thời xoay, nếu va chạm thì xoay ngược lại (để tránh lỗi)
                current->rotate();
                if (!canMove(0, 0)) { /* Code xoay ngược lại ở đây nếu cần */ }
            }
            if (c == 'q') break;
        }

        if (canMove(0, 1)) y++;
        else {
            block2Board();
            removeLine();
            delete current;

            current = randomPiece();
            x = W / 2 - 2;
            y = 0;

            // KIỂM TRA THUA CUỘC: Nếu khối mới tạo ra mà không thể di chuyển ngay
            if (!canMove(0, 0)) {
                showGameOver(); // Gọi hàm kết thúc đẹp mắt
                break; // Thoát vòng lặp game
            }
        }

        block2Board();
        draw();
        Sleep(max(50, 200 - (score / 500) * 10));
    }
    delete current;
    return 0;
}