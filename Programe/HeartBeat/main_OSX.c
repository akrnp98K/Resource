//
// Created by Xjy on 2024/10/10.
//

#include <math.h>
#include <unistd.h>
#include <ncurses.h>  // 使用 ncurses 库
#include <locale.h>   // 用于支持 Unicode

float f(float x, float y, float z) {
    float a = x * x + 9.0f / 4.0f * y * y + z * z - 1;
    return a * a * a - x * x * z * z * z - 9.0f / 80.0f * y * y * z * z * z;
}

float h(float x, float z) {
    for (float y = 1.0f; y >= 0.0f; y -= 0.001f)
        if (f(x, y, z) <= 0.0f)
            return y;
    return 0.0f;
}

int main() {
    setlocale(LC_ALL, "");  // 设置 locale 以支持 Unicode 字符
    initscr();              // 初始化 ncurses
    noecho();               // 禁止键盘输入回显
    curs_set(0);            // 隐藏光标

    char buffer[25][80] = { ' ' };
    char ramp[] = ".:-=+*#%@";

    for (float t = 0.0f;; t += 0.1f) {
        int sy = 0;
        float s = sinf(t);
        float a = s * s * s * s * 0.2f;
        for (float z = 1.3f; z > -1.2f; z -= 0.1f) {
            char* p = &buffer[sy++][0];
            float tz = z * (1.2f - a);
            for (float x = -1.5f; x < 1.5f; x += 0.05f) {
                float tx = x * (1.2f + a);
                float v = f(tx, 0.0f, tz);
                if (v <= 0.0f) {
                    float y0 = h(tx, tz);
                    float ny = 0.01f;
                    float nx = h(tx + ny, tz) - y0;
                    float nz = h(tx, tz + ny) - y0;
                    float nd = 1.0f / sqrtf(nx * nx + ny * ny + nz * nz);
                    float d = (nx + ny - nz) * nd * 0.5f + 0.5f;
                    *p++ = ramp[(int)(d * 5.0f)];
                } else {
                    *p++ = ' ';
                }
            }
        }

        for (sy = 0; sy < 25; sy++) {
            move(sy, 0);             // 移动到行首
            for (int sx = 0; sx < 79; sx++) {
                addch(buffer[sy][sx]);  // 输出字符到屏幕
            }
        }
        refresh();                 // 刷新屏幕显示
        usleep(33000);             // 延迟 33 毫秒（接近 30 FPS）

        // 清空缓冲区，准备下一帧
        for (sy = 0; sy < 25; sy++) {
            for (int sx = 0; sx < 79; sx++) {
                buffer[sy][sx] = ' ';
            }
        }
    }

    endwin();  // 退出 ncurses
    return 0;
}
