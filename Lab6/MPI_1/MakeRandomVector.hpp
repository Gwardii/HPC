#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>

using namespace std;

template<typename Generator>
void getrands(vector<double>& x, Generator& gen, unsigned num)
{
  generate_n(std::back_inserter(x), num, std::ref(gen));
}

auto makeRandomVector(std::size_t size = 100,
                      double min = std::numeric_limits<double>::min(),
                      double max = std::numeric_limits<double>::max())
{
    uniform_real_distribution<double> unif(0.0,1.0);
    mt19937 re(std::random_device{}());
    auto generator = bind(unif, std::ref(re));

    vector<double> vs;

    getrands(vs, generator, size);
    
    return vs;
}
