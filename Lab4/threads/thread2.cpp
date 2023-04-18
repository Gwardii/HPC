#include <iostream>
#include <thread>
 
#include <unistd.h>
 
using namespace std;
 
void foo() 
{
  for(int i=0; i<100; ++i)
  {
  	cout << "foo says: foo sleeps " << i << endl;
  	sleep ( 1 );
  }
	cout << "foo says: completed " << endl;
}

void thread_caller()
{
  std::thread first (foo);     // spawn new thread that calls foo()
  first.detach();
}

int main() 
{
  thread_caller();
  cout << "main thread says: foo now executes concurrently..." << endl;

  for(int i=0; i<5; ++i)
  {
  	cout << "main says: There can be only one! " << i << endl;
  	sleep ( 1 );
  }

  return 0;
}
