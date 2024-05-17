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


const bool DEBUG = false;
template <typename TFunc> void RunAndMeasure(const char* title, TFunc func)
{
    const auto start = std::chrono::steady_clock::now();
    auto ret = func();
    const auto end = std::chrono::steady_clock::now();
    cout << title << ": " << std::chrono::duration <double, std::milli>(end - start).count() << " ms, res " << ret << "\n";
}

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
int sharedVar = 0; //wrong, thread number should be limited
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
    size_t gpd = 1;
    for (size_t i = 1; i < sqrt(n); i++)
    {
        if (n % i == 0)
        {
            if(s[i])    gpd = max(gpd, i);
            if(s[n/i])  gpd = max(gpd, n/i);
        }
    }

    return gpd;
}
template <typename T>
void printVector(vector<T> vec) {
    for (size_t i = 0; i < vec.size(); i++)
        cout << vec[i] << ' ';
    cout << endl;
}
vector<int> createVectorRandom(int n, int leftBorder, int rightBorder) {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(leftBorder, rightBorder); // define the range

    vector<int> res;
    for (size_t i = 0; i < n; i++)
        res.push_back(distr(gen));

    return res;
}
void task4_helper(vector<int> vec, int l, int r) {
    for (size_t i = l; i < r; i++) {
        vec[i] = gpd(vec[i]);
        if (DEBUG) cout << "vec[" + to_string(i) + "]: " + to_string(vec[i]) << endl;
    }
}
//if arr divides in even parts by threads, divide in even sectors and run threads
int task4_run(int p, vector<int>& arr) {
    if (arr.size() % p == 0) {
        int partSize = arr.size() / p;
        vector<thread> threads;
        for (size_t i = 0; i < p; i++)
            threads.push_back(thread(task4_helper, ref(arr), i*partSize, (i+1)*partSize));

        for (size_t i = 0; i < p; i++)
            if(threads[i].joinable()) threads[i].join();
    }
    return 0;
}   

void task4(int N) {
    
    //Filling ertosphenes sieve
    for (size_t i = 0; i < 1000000; i++)
        s[i] = true;
    s[0] = false; s[1] = false;
    for (size_t i = 2; i < 1000000; i++)
        for (size_t j = i*i; j < 1000000; j+=i)
            s[j] = false;

    if (DEBUG) {
        vector<int> arr = createVectorRandom(10, 10, 100);
        printVector(arr);
        RunAndMeasure("10", [N, &arr] {return task4_run(N, arr); });

        arr = createVectorRandom(2 * 1e5, (int)1e5, (int)1e6);
        RunAndMeasure("10e6", [N, &arr] {return task4_run(N, arr); });

    }
    else {
        vector<int> arr = createVectorRandom(2 * 1e5, (int)1e5, (int)1e6);
        RunAndMeasure("2*10e5", [N, &arr] {return task4_run(N, arr); });

        arr = createVectorRandom(5 * 10e7, (int)1e5, (int)1e6);
        RunAndMeasure("5*10e7", [N, &arr] {return task4_run(N, arr); });

        arr = createVectorRandom(5 * 10e8, (int)1e5, (int)1e6);
        RunAndMeasure("5*10e8", [N, &arr] {return task4_run(N, arr); });
    }


}
int main()
{
    int N;
    cout << "Input N: "; cin >> N;
    
    task4(N);
    return 0;
}

/*
* 1.01284	0.1558	0.5249
6.44357	2.2128	10.4005
67.39	38.8445	83.2629

*/