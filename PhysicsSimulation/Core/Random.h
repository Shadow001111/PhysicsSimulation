#include <random>

class Random {
public:
    Random() = delete;

    static int Int(int min, int max);
    static float Float(float min, float max);
private:
    static std::mt19937& GetEngine();
};
