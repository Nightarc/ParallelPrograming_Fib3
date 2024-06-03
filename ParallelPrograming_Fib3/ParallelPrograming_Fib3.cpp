#include <iostream>
#include <omp.h> 
#include <stdio.h>
#include <thread>
#include <mutex>
#include <vector>
#include <limits>
#include <random>
#include <chrono>
using namespace std;
using Matrix = std::vector<std::vector<int>>;

// Функция для инициализации матрицы размером N x N
void initializeMatrix(Matrix& matrix, int N) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = dis(gen);
        }
    }
}

// Функция умножения матриц
void multiplyMatrices(const Matrix& A, const Matrix& B, Matrix& C, int N, int numThreads) {
    omp_set_num_threads(numThreads);
#pragma omp parallel for collapse(2)
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Функция для нахождения наибольшего простого делителя числа
int maxPrimeDivisor(int n) {
    int maxPrime = -1;
    while (n % 2 == 0) {
        maxPrime = 2;
        n /= 2;
    }
    for (int i = 3; i <= sqrt(n); i += 2) {
        while (n % i == 0) {
            maxPrime = i;
            n = n / i;
        }
    }
    if (n > 2) maxPrime = n;
    return maxPrime;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    cout << "Введите номер задания: ";
    int s;
    cin >> s;
    switch (s)
    {
    case 0:
    {
#ifdef _OPENMP
        printf("OpenMP is supported! %d\n", _OPENMP);
#else
        printf("OpenMP is not supported!\n");
#endif
        break;
    }
    case 1:
    {
        int nthreads, tid;
        int requested_threads;

        printf("Введите кол-во потоков: ");
        cin >> requested_threads;

        // Установка количества потоков для следующей параллельной области
        omp_set_num_threads(requested_threads);

        // Создание параллельной области
#pragma omp parallel private(tid)
        {
            // печать номера потока
            tid = omp_get_thread_num();
            printf("Hello World from thread = %d\n", tid);

            // Печать количества потоков – только master
            if (tid == 0) {
                nthreads = omp_get_num_threads();
                printf("Number of threads = %d\n", nthreads);
            }
        } // Завершение параллельной области
        break;
    }
    case 2: {
        int N;
        std::cout << "Enter the value of N: ";
        std::cin >> N;

        // Создаем два потока
#pragma omp parallel num_threads(2)
        {
            // Получаем номер текущего потока
            int tid = omp_get_thread_num();

            // Первый поток выводит числа от 1 до N
            if (tid == 0) {
                for (int i = 1; i <= N; ++i) {
                    std::cout << "Thread 1: " << i << std::endl;
                }
            }
            // Второй поток выводит слово "HELLO" N раз
            else if (tid == 1) {
                for (int i = 0; i < N; ++i) {
                    std::cout << "Thread 2: HELLO" << std::endl;
                }
            }
        }
        break;
    }
    case 3:
    {
        int N;
        std::cout << "Введите размер массивов N: ";
        std::cin >> N;

        std::vector<int> A(N, 2); // Инициализация массива A значениями 2
        std::vector<int> B(N, 15); // Инициализация массива B значениями 15
        std::vector<int> C(N);

        auto start_time = std::chrono::high_resolution_clock::now();

        // Для static scheduling
    //#pragma omp parallel for schedule(static)
    //    for (int i = 0; i < N; ++i) {
    //        C[i] = A[i] * B[i];
    //    }

    //    // Для dynamic scheduling
    //#pragma omp parallel for schedule(dynamic)
    //    for (int i = 0; i < N; ++i) {
    //        C[i] = A[i] * B[i];
    //    }
    //
    //    // Для guided scheduling
#pragma omp parallel for schedule(guided)
        for (int i = 0; i < N; ++i) {
            C[i] = A[i] * B[i];
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time;

        std::cout << "Результат поэлементного произведения: \n";
        for (int i = 0; i < N; ++i) {
            std::cout << C[i] << " ";
        }
        std::cout << "\n";

        std::cout << "Время выполнения: " << elapsed_time.count() << " мс.\n";


        break;
    }
    case 4:
    {
        const int N = 1000000;
        std::vector<int> vec(N);

        // Генерация случайных чисел
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 53425);

        for (int& val : vec) {
            val = dis(gen);
        }

        int max_val_reduction = std::numeric_limits<int>::min();
        int max_val_critical = std::numeric_limits<int>::min();
        int max_val_atomic = std::numeric_limits<int>::min();

        // a) С использованием параметра reduction
        auto start_reduction = std::chrono::high_resolution_clock::now();
#pragma omp parallel for reduction(max:max_val_reduction)
        for (int i = 0; i < N; ++i) {
            max_val_reduction = std::max(max_val_reduction, vec[i]);
        }
        auto end_reduction = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_reduction = end_reduction - start_reduction;

        // б) С использованием директивы critical
        auto start_critical = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
        for (int i = 0; i < N; ++i) {
#pragma omp critical
            {
                max_val_critical = std::max(max_val_critical, vec[i]);
            }
        }
        auto end_critical = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_critical = end_critical - start_critical;

        // в) С использованием атомарных операций
        auto start_atomic = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
        for (int i = 0; i < N; ++i) {
            int a = 0;
#pragma omp atomic
            a += std::max(max_val_atomic, vec[i]);
            max_val_atomic = a;
        }
        auto end_atomic = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_atomic = end_atomic - start_atomic;

        std::cout << "Максимальное значение (reduction): " << max_val_reduction << ", время: " << elapsed_reduction.count() << " секунд.\n";
        std::cout << "Максимальное значение (critical): " << max_val_critical << ", время: " << elapsed_critical.count() << " секунд.\n";
        std::cout << "Максимальное значение (atomic): " << max_val_atomic << ", время: " << elapsed_atomic.count() << " секунд.\n";

        break;
    }
    case 5:
    {
        int N, sum = 0;
        std::cout << "Введите число N: ";
        std::cin >> N;

        int M = 2; // Количество потоков для суммирования
        int K = 1; // Количество потоков для вычисления длины вектора

        omp_set_nested(1); // Включение вложенного параллелизма

#pragma omp parallel sections
        {
#pragma omp section
            {
                // Вычисление суммы от 1 до N
#pragma omp parallel for num_threads(M) reduction(+:sum)
                for (int i = 1; i <= N; ++i) {
                    sum += i;
                }
                std::cout << "Сумма от 1 до " << N << ": " << sum << std::endl;
            }

#pragma omp section
            {
                // Вычисление длины N-мерного вектора (будет выведено k раз так как создается K потоков)
#pragma omp parallel for num_threads(K)
                for (int i = 0; i < K; ++i) {
                    double length = std::sqrt(N);
#pragma omp critical
                    std::cout << "Длина " << N << "-мерного вектора: " << length << std::endl;
                }
            }
        }


        break;
    }
    case 6:
    {
        std::vector<int> sizes = { 2 * 10000000, 5 * 10000000, 100000000 };
        std::vector<int> threads = { 1, 2, 4, 8 };
        std::vector<double> times;
        const int runs = 3; // Количество запусков для каждого теста
        double totalTime = 0;
        double timeForOneThread = 0;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(100000, 1000000);

        for (int N : sizes) {
            std::vector<int> vec(N);

            std::cout << "Тестирование для N = " << N << std::endl;

            for (int T : threads) {
                totalTime = 0;

                for (int run = 0; run < runs; ++run) {
                    // Генерация чисел для каждого запуска, чтобы избежать эффекта кэширования
                    for (int& val : vec) {
                        val = dis(gen);
                    }

                    omp_set_num_threads(T);
                    auto start_time = std::chrono::high_resolution_clock::now();

#pragma omp parallel for schedule(dynamic)
                    for (int i = 0; i < N; ++i) {
                        vec[i] = maxPrimeDivisor(vec[i]);
                    }

                    auto end_time = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time;
                    totalTime += elapsed_time.count() / 1000; // Время в секундах
                }

                double averageTime = totalTime / runs;
                std::cout << "Среднее время выполнения на " << T << " потоках: " << averageTime << " секунд" << std::endl;
                times.push_back(averageTime);
            }

            // Расчет и вывод ускорения
            std::cout << "Ускорение:" << std::endl;
            for (int i = 1; i < times.size(); i++)
            {
                std::cout << i * 2 << " " << times[0] / times[i] << std::endl;
            }
            times.clear();
        }

        break;
    }
    case 7:
    {
        std::vector<int> sizes = { 500, 1000, 2000 }; // Размеры матриц
        std::vector<int> threads = { 1, 2, 4, 8 }; // Количество потоков
        std::vector<double> times;

        for (int N : sizes) {
            Matrix A(N, std::vector<int>(N));
            Matrix B(N, std::vector<int>(N));
            Matrix C(N, std::vector<int>(N));

            initializeMatrix(A, N);
            initializeMatrix(B, N);

            std::cout << "Тестирование для N = " << N << std::endl;
            for (int T : threads) {
                double totalTime = 0;
                const int runs = 3;
                for (int run = 0; run < runs; ++run) {
                    auto start_time = std::chrono::high_resolution_clock::now();

                    multiplyMatrices(A, B, C, N, T);

                    auto end_time = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> elapsed_time = end_time - start_time;
                    totalTime += elapsed_time.count();
                }
                double averageTime = totalTime / runs;
                std::cout << "Среднее время выполнения на " << T << " потоках: " << averageTime << " секунд" << std::endl;
                times.push_back(averageTime);
            }

            // Расчет и вывод ускорения
            std::cout << "Ускорение:" << std::endl;
            for (int i = 1; i < times.size(); i++)
            {
                std::cout << i * 2 << " " << times[0] / times[i] << std::endl;
            }
            times.clear();
        }


        break;
    }
    case 8:
    {
        std::vector<int> sizes = { 5000, 10000, 20000 };
        std::vector<int> threads = { 1, 2, 4, 8 };
        const int runs = 3; // Количество запусков для каждого теста

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 1000);
        std::vector<double> times;

        for (int N : sizes) {
            std::vector<std::vector<int>> matrix(N, std::vector<int>(N));

            std::cout << "Тестирование для N = " << N << std::endl;

            for (int T : threads) {
                double totalTime = 0;
                double timeForOneThread = 0;

                for (int run = 0; run < runs; ++run) {
                    // Заполнение матрицы случайными значениями
                    for (auto& row : matrix) {
                        for (int& cell : row) {
                            cell = dis(gen);
                        }
                    }

                    omp_set_num_threads(T);
                    auto start_time = std::chrono::high_resolution_clock::now();
                    int maxMin = std::numeric_limits<int>::min();

#pragma omp parallel for collapse(2) reduction(max : maxMin)
                    for (int i = 0; i < N; ++i) {
                        for (int j = 0; j < N; ++j) {
                            // Здесь теперь прямой доступ к элементам, без внутреннего цикла
                            int currentVal = matrix[i][j];
#pragma omp critical
                            {
                                if (currentVal < maxMin) {
                                    maxMin = currentVal;
                                }
                            }
                        }
                    }

                    auto end_time = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time;
                    totalTime += elapsed_time.count();
                }
                double averageTime = totalTime / runs / 1000.0; // Преобразование в секунды
                std::cout << "Среднее время выполнения на " << T << " потоках: " << averageTime << " секунд" << std::endl;
                times.push_back(averageTime);
            }

            // Расчет и вывод ускорения
            std::cout << "Ускорение:" << std::endl;
            for (int i = 1; i < times.size(); i++)
            {
                std::cout << threads[i] << " потоков: " << times[0] / times[i] << "x" << std::endl;
            }
            times.clear();

        }


        break;
    }

    default:
        break;
    }
}
