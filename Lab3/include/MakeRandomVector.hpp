#include <algorithm>
#include <limits>
#include <random>
#include <type_traits>
#include <vector>

namespace detail
{
template < typename T >
struct rand_dist
{
    using type = std::conditional_t<
        std::is_integral_v< T >,
        std::uniform_int_distribution< T >,
        std::conditional_t< std::is_floating_point_v< T >, std::uniform_real_distribution< T >, void > >;
};
} // namespace detail

template < typename T >
using rand_dist_t = typename detail::rand_dist< T >::type;

template < typename T >
auto makeRandomVector(std::size_t size = 100,
                      T           min  = std::numeric_limits< T >::min(),
                      T           max  = std::numeric_limits< T >::max())
{
    thread_local std::mt19937 prng{std::random_device{}()};
    rand_dist_t< T >          random_dist{min, max};

    std::vector< T > result;
    result.reserve(size);
    std::generate_n(std::back_inserter(result), size, [&] { return random_dist(prng); });
    return result;
}
