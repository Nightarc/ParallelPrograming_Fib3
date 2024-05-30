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
mutex mtx;
int sharedVar = 0;

const bool DEBUG = true;
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

void task1() {
    thread T1 = thread([]() { //thread that prints even numbers
        for (size_t i = 0; i <= 100; i += 2)
            cout << i << endl;
        });
    thread T2 = thread([]() { //thread that prints odd numbers
        for (size_t i = 1; i <= 100; i += 2)
            cout << i << endl;
        });
    
    T1.join(); T2.join();
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
    
    vector<thread> threads;
    for (size_t i = 0; i < N; i++)
        threads.emplace_back(foo);

    for (auto& thread : threads) {
        thread.join();
    }
}


bool s[1000000];
int gpd(int n) {
    size_t gpd = 1;
    for (size_t i = 2; i <= sqrt(n); i++)
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
void task4_helper(vector<int>& vec, int l, int r) {
    for (size_t i = l; i < r; i++) {
        vec[i] = gpd(vec[i]);
        if (DEBUG && vec.size() <= 100) cout << "vec[" + to_string(i) + "]: " + to_string(vec[i]) << endl;
    }
}
//if arr divides in even parts by threads, divide in even sectors and run threads
int task4_run(int p, vector<int> arr) {
    int partSize = arr.size() / p;
    vector<thread> threads;
    for (size_t i = 0; i < p; i++)
    {
        int start = i * partSize;
        int end = (i == p - 1) ? arr.size() : (i + 1) * partSize; // last thread fills all remaining space
        threads.push_back(thread(task4_helper, ref(arr), start, end));

    }

    for (auto& thread : threads) thread.join();

    if (arr.size() < 100) printVector(arr);
    return 0;
}   

chrono::milliseconds task4(int P, int N) {
    //Filling ertosphenes sieve
    for (size_t i = 0; i < 1000000; i++)
        s[i] = true;
    s[0] = false; s[1] = false;
    for (size_t i = 2; i < 1000000; i++)
        for (size_t j = i*i; j < 1000000; j+=i)
            s[j] = false;

    vector<int> arr = createVectorRandom(N, 1e5, 1e6);
    return RunAndMeasure(to_string(N).c_str(), [P, &arr] {return task4_run(P, arr); }, true);
    

    /*if (DEBUG) {
        vector<int> arr = createVectorRandom(10, 10, 100);
        printVector(arr);
        RunAndMeasure("10", [N, &arr] {return task4_run(N, arr); });

        arr = createVectorRandom(2 * 1e5, (int)1e5, (int)1e6);
        RunAndMeasure("10e6", [N, &arr] {return task4_run(N, arr); });

    }
    else {
        vector<int> arr = createVectorRandom(2 * 1e5, (int)1e5, (int)1e6);
        RunAndMeasure("2*10e5", [N, &arr] {return task4_run(N, arr); });
        arr.clear();

        arr = createVectorRandom(5 * 10e7, (int)1e5, (int)1e6);
        RunAndMeasure("5*10e7", [N, &arr] {return task4_run(N, arr); });
        arr.clear();

        arr = createVectorRandom(5 * 10e8, (int)1e5, (int)1e6);
        RunAndMeasure("5*10e8", [N, &arr] {return task4_run(N, arr); });
        arr.clear();
    }*/


}

void increaseVar(int index) {
    while (sharedVar < 100) {
        this_thread::sleep_for(chrono::microseconds(1000)); //or else it tries to join thread in-creation
        mtx.lock();
        if (sharedVar < 100) sharedVar += index;
        cout << index << ": sharedVar now is " << sharedVar << endl;
        mtx.unlock();
    }
}

//Задача 3: n потоков увеличивают общий ресурс - целочисленную переменную
void task3() {
    //Ввод P
    cout << "Enter number of threads P: ";
    int P; cin >> P;
    if (P <= 0 || P > 10) {
        std::cout << "Wrong thread count!" << std::endl;
        return;
    }

    vector<thread> threads; //вектор потоков
    threads.reserve(P);
    // Создание и запуск потоков
    for (int i = 1; i <= P; ++i) {
        threads.emplace_back(increaseVar, i);
    }

    //Ждём все потоки
    for (auto& thread : threads)
        thread.join();
}

//Задача 5 - последовательные и параллельный алгоритм вычисления произведения квадратной матрицы на вектор. Параллельный алгоритм учитывает доступное количество процессоров.
//N = 500, 10000, 20000
// Генерация квадратной матрицы размерности N
std::vector<std::vector<double>> generateMatrix(int N) {
    std::vector<std::vector<double>> matrix(N, std::vector<double>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = static_cast<double>(rand()) / RAND_MAX; // Заполняем случайными значениями от 0 до 1
        }
    }
    return matrix;
}

// Генерация вектора размерности N
vector<double> generateVector(int N) {
    std::vector<double> vector(N);
    for (int i = 0; i < N; ++i) {
        vector[i] = static_cast<double>(rand()) / RAND_MAX; 
    }
    return vector;
}

vector<double> parallelMatrixVectorMultiplication(const std::vector<std::vector<double>>& matrix, const std::vector<double>& vector) {
    int N = matrix.size();
    std::vector<double> result(N, 0.0);

    for (int numThreads : {1, 2, 4, 8}) {
        auto start = std::chrono::steady_clock::now();
        std::vector<std::thread> threads(numThreads);
        int chunkSize = N / numThreads;
        for (int i = 0; i < numThreads; ++i) {
            int start = i * chunkSize;
            int end = (i == numThreads - 1) ? N : (i + 1) * chunkSize;
            threads[i] = std::thread([&matrix, &vector, &result, &N, start, end]() {
                for (int row = start; row < end; ++row) {
                    for (int col = 0; col < N; ++col) {
                        result[row] += matrix[row][col] * vector[col];
                    }
                }
                });
        }
        for (auto& thread : threads) {
            thread.join();
        }
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> parallelDuration = end - start;
        std::cout << "Параллельное время с " << numThreads << " ядрами: " << parallelDuration.count() << " секунд" << std::endl;
    }

    return result;
}
std::vector<double> MVMult_Seq(const std::vector<std::vector<double>>& matrix, const std::vector<double>& vector) {
	int N = matrix.size();
	std::vector<double> result(N);
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			result[i] += matrix[i][j] * vector[j];
		}
	}
	return result;
}
void task5() {
    std::vector<int> sizes = { 5000, 10000, 20000 };

    for (int N : sizes) {
        std::cout << "Matrix size: " << N << "x" << N << std::endl;

        // Генерация матрицы и вектора
        auto matrix = generateMatrix(N);
        auto vector = generateVector(N);

        // Замер времени для последовательного алгоритма
        auto start = std::chrono::steady_clock::now();
        auto seqResult = MVMult_Seq(matrix, vector);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> seqDuration = end - start;
        std::cout << "Sequential time: " << seqDuration.count() << " s" << std::endl;

        auto parallelResult = parallelMatrixVectorMultiplication(matrix, vector);

    }
}

//Сумма чисел от 1 до N
void task6(int p, int N) {
    auto start = std::chrono::steady_clock::now();
    long long result = 0;
    std::vector<std::thread> threads(p);
    int chunkSize = N / p;
    for (int i = 0; i < p; ++i) {
        int start = i * chunkSize;
        int end = (i == p - 1) ? N : (i + 1) * chunkSize;
        threads[i] = std::thread([&result, &N, start, end]() {
            long long localSum = 0;
            for (size_t j = start; j < end; j++) 
                localSum += j;

            std::lock_guard<std::mutex> lock(mtx);
            result += localSum;
            });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> parallelDuration = end - start;
    std::cout << "time " << p << " threads: " << parallelDuration.count() << " s" << std::endl;
}
int main()
{
    int state = -1;
    int N;
    int p[] = { 1, 2, 4, 8 };
    int ns[] = { 2 * 1e7, 5 * 1e7, 1e8 };
    cout << "Input task number: "; 
    cin >> state;
    switch (state) {
        case 1:
            task1();
            break;
        case 2:
            cout << "Input N <= 10: "; cin >> N;
            task2(N);
            break;
        case 3:
            task3();
            break;
        case 4:
        {
            for (size_t i = 0; i < 4; i++)
            {
                chrono::milliseconds time = (chrono::milliseconds)0;
                for (size_t i = 0; i < 3; i++)
                {
                    auto start = std::chrono::steady_clock::now();
                    task4(p[i], ns[i]);
                    auto end = std::chrono::steady_clock::now();
                    time += chrono::duration<double>(end - start).count();
                }
            }
            break;
        }
        case 5:
            break;
        case 6:
            break;
    }
    
    return 0;
}

