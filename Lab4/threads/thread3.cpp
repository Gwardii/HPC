#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
 
using namespace std;

atomic<int> thread_counter{0};
//int thread_counter{0};

void foo(int id) 
{
	cout << "Thred with ID: "<< id << " Started" << endl;	
  for(int i=0; i<1000; ++i)
  {
    thread_counter += 1;
  }
	cout << "ID:" << id << " " << thread_counter << " Completed! " << endl;
}

int main() 
{
	vector<thread> tv(0);
	tv.reserve(100);

	//start threads
	for(int i=0; i<100; ++i)
	{
		tv.push_back(thread(foo, i));
	}
	
  cout << "main thread says: foo now executes concurrently..." << endl;

	for(auto &t : tv)
	{
		t.join();
		//cout << " Joined " << it - tv.begin() << endl;
	}

  cout << thread_counter << endl;

  return 0;
}
