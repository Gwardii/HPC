#include "MakeRandomVector.hpp"
#include <cmath>
#include <cstdint>
using std::ptrdiff_t;

int main()
{
    const auto random_vector = makeRandomVector(100, -3.14, 3.14); // 100 random values from [-3.14, 3.14]
    auto       results       = random_vector;
    for(ptrdiff_t i = 0; double x : random_vector)
        results[i++] = sin(x);
}