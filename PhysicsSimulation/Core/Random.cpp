#include "Random.h"

std::mt19937& Random::GetEngine()
{
    static std::random_device rd;
    static std::mt19937 engine(rd());
    return engine;
}

int Random::Int(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(Random::GetEngine());
}

float Random::Float(float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(GetEngine());
}
