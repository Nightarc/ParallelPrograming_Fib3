// ParallelPrograming_Fib3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <chrono>
#include <random>
#include <thread>
#include <mutex>
#include <string>
#include <numeric>
#include <vector>
using namespace std;
std::random_device rd; // obtain a random number from hardware
std::mt19937 gen(rd()); // seed the generator
std::uniform_int_distribution<> distr(1000, 2000); // define the range
int nextRandom() {
    return distr(gen);
}
void foo()
{
    std::cout << "Thread start..." << std::endl;
    for (int i = 0; i < 100; ++i)
    {
        std::cout << "Thread id = " << this_thread::get_id()
            << std::endl;
    }
    this_thread::sleep_for(std::chrono::milliseconds(nextRandom()));
    std::cout << "Thread finish!" << std::endl;
    return;
}

void task2(int N) {
    
    for (size_t i = 0; i < N; i++)
    {
        thread T = thread(foo);
        T.detach(); 
    }
}
int sharedVar = 0;
void task3ThreadFunc(int i, std::mutex &mx) {
    std::lock_guard<std::mutex> ulmx(mx);
    if(sharedVar < 100) sharedVar += i;
    cout << to_string(i) + " reporting: sharedVar: " + to_string(sharedVar) << endl;
    if (sharedVar > 100) {
        cout << "aborting!" << endl;
        return;
    }
}
void task3(int P) {
    std::mutex mx;

    int i = 0;
    while(sharedVar < 100)
    {
        thread T = thread(task3ThreadFunc, (i++), ref(mx));
        T.detach();
    }
}
bool s[1000000];
int gpd(int n) {
    int gpd = 2;
    for (size_t i = 2; i < n; i++)
        if (s[i] && n % i == 0) gpd = i;

    return gpd;
}
void task4_helper(vector<int> vec) {
    return transform(vec.begin(), vec.end(), vec.begin(), gpd)
}
void task4() {
    //Filling ertosphenes sieve
    for (size_t i = 0; i < 1000000; i++)
        s[i] = true;
    s[0] = false; s[1] = false;
    for (size_t i = 2; i < 1000000; i++)
    {
        for (size_t j = 0; j < 1000000; j+=i)
        {
            s[i] = false;
        }
    }
}
int main()
{
    int N;
    sharedVar = 0;
    
    task3(N);
    string test;
    cin >> test;
    return 0;
}

/*
* 1.01284	0.1558	0.5249
6.44357	2.2128	10.4005
67.39	38.8445	83.2629

*/