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
int gameSpeed = 200; // SV5: Biến lưu trữ tốc độ (thời gian delay ms)

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
    // Hiển thị tốc độ hiện tại để SV5 dễ theo dõi
    cout << "\nSpeed: " << gameSpeed << "ms  " << endl;
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

// SV5: Chỉnh sửa hàm để trả về số dòng đã xóa
int removeLine() {
    int linesCleared = 0;
    for (int i = H - 2; i > 0; i--) {
        bool full = true;
        for (int j = 1; j < W - 1; j++)
            if (board[i][j] == ' ') {
                full = false;
                break;
            }
        if (full) {
            linesCleared++; // Đếm số dòng bị xóa
            for (int k = i; k > 0; k--)
                for (int j = 1; j < W - 1; j++)
                    board[k][j] = board[k - 1][j];
            i++;
        }
    }
    return linesCleared;
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

    int fallTimer = 0; // Biến đếm để điều khiển tốc độ rơi

    while (true) {
        // 1. XỬ LÝ PHÍM BẤM (Gần như tức thời)
        if (_kbhit()) {
            boardDelBlock(); // Xóa vị trí cũ trước khi di chuyển
            char c = _getch();
            if (c == 'a' && canMove(-1, 0)) x--;
            if (c == 'd' && canMove(1, 0))  x++;
            if (c == 's' && canMove(0, 1))  y++;
            if (c == 'w') current->rotate();
            if (c == 'q') break;

            block2Board(); // Cập nhật vị trí mới
            draw();        // Vẽ lại ngay lập tức khi nhấn phím
        }

        // 2. XỬ LÝ RƠI TỰ NHIÊN (Chỉ rơi khi đủ thời gian)
        // Cứ mỗi 10ms vòng lặp chạy 1 lần, fallTimer sẽ tăng lên
        if (fallTimer >= gameSpeed) {
            boardDelBlock();
            if (canMove(0, 1)) {
                y++;
            }
            else {
                block2Board();
                int lines = removeLine();
                if (lines > 0) {
                    gameSpeed -= (lines * 10);
                    if (gameSpeed < 50) gameSpeed = 50;
                }
                delete current;
                current = randomPiece();
                x = 5; y = 0;
                if (!canMove(0, 0)) break;
            }
            block2Board();
            draw();
            fallTimer = 0; // Reset bộ đếm sau khi rơi
        }

        // 3. ĐỢI MỘT KHOẢNG RẤT NGẮN
        Sleep(10);
        fallTimer += 10; // Cộng dồn thời gian đã trôi qua
    }

    system("cls");
    cout << "GAME OVER!" << endl;
    cout << "Toc do cuoi cung: " << gameSpeed << "ms" << endl;
    return 0;
}