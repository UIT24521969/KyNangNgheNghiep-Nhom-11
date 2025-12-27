#ifndef SPEED_MANAGER_H
#define SPEED_MANAGER_H

#include <algorithm>

/* NHIỆM VỤ SV5: QUẢN LÝ TỐC ĐỘ
   - Giảm thời gian Sleep mỗi khi xóa được hàng.
   - Giới hạn tốc độ tối đa để game không bị quá nhanh.
*/

class SpeedManager {
private:
    int currentDelay;   // Thời gian chờ hiện tại (ms)
    int minDelay;       // Tốc độ nhanh nhất cho phép (ms)
    int speedFactor;    // Lượng ms giảm đi cho mỗi hàng xóa được

public:
    // Khởi tạo mặc định: bắt đầu 200ms, nhanh nhất 50ms, giảm 15ms/hàng
    SpeedManager(int startDelay = 200, int minD = 50, int factor = 15)
        : currentDelay(startDelay), minDelay(minD), speedFactor(factor) {
    }

    // SV5: Hàm xử lý tăng tốc độ
    void increaseSpeed(int linesCleared) {
        if (linesCleared > 0) {
            // Càng xóa nhiều hàng một lúc, tốc độ tăng càng nhiều
            currentDelay -= (linesCleared * speedFactor);

            // Đảm bảo không vượt quá tốc độ tối đa (nhanh quá không chơi được)
            if (currentDelay < minDelay) {
                currentDelay = minDelay;
            }
        }
    }

    // Lấy giá trị delay để đưa vào hàm Sleep() trong main
    int getDelay() const {
        return currentDelay;
    }
};

#endif