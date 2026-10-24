#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <vector>

constexpr int W = 4096;
constexpr int H = 4096;

__attribute__((noinline))
void init_image(float* img) {
    unsigned s = 12345u;                       // a simple LCG to make random data
    for (int i = 0; i < W * H; ++i) {
        s = s * 1103515245u + 12345u;
        img[i] = (s >> 8) * (1.0f / 16777216.0f);
    }
}

__attribute__((noinline))
void blur_pass(const float* __restrict in, float* __restrict out) {
    for (int x = 1; x < W - 1; ++x)          // outer loop — columns
        for (int y = 1; y < H - 1; ++y) {    // inner loop — adjacent pixels in memory
            float s = 0.0f;
            for (int dy = -1; dy <= 1; ++dy)
                for (int dx = -1; dx <= 1; ++dx)
                    s += in[(y + dy) * W + (x + dx)];
            out[y * W + x] = s * (1.0f / 9.0f);
        }
}

__attribute__((noinline))
double checksum(const float* img) {
    double sum = 0.0;
    for (int i = 0; i < W * H; ++i) sum += img[i];
    return sum;
}

int main(int argc, char** argv) {
    const int passes = (argc > 1) ? std::atoi(argv[1]) : 40;   // number of blur passes
    std::vector<float> a(W * H), b(W * H);
    init_image(a.data());
    b = a;

    auto t0 = std::chrono::steady_clock::now();
    for (int p = 0; p < passes; ++p) {
        blur_pass(a.data(), b.data());
        a.swap(b);
    }
    auto t1 = std::chrono::steady_clock::now();

    std::printf("checksum = %.6f\n", checksum(a.data()));
    std::printf("blur time = %.3f s\n", std::chrono::duration<double>(t1 - t0).count());
    return 0;
}