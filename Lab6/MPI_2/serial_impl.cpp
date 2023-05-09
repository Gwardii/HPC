#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

int main()
{
  int size = 1000000;
  auto y = std::vector<double>(size);
  double h = 4 * M_PI / (size-1);
  for (int i=0; i<y.size(); ++i)
  {
      double x = i*h;
      y[i] = std::sin(pow(x, M_PI)) * std::cos(pow(x, M_PI))
          * std::tan(pow(x, M_PI)) - std::atan(exp(x) /(x+1.0) );
  };
  
  double avg = 0;
  for (int i=0; i<y.size(); ++i)
  {
    avg += y[i];
  }
  avg = avg / y.size();

  cout << "Calcualted average is for size " << size << " is " << avg << endl;
}
