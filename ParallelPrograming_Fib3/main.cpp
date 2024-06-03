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

void printMPSupport() {
	#ifdef _OPENMP
		printf("OpenMP is supported! %d\n", _OPENMP);
	#else
		printf("OpenMP is not supported!\n");
	#endif
}
int gpd(int n) {
	int gpd = -1;
	while (n % 2 == 0) {
		gpd = 2;
		n /= 2;
	}
	for (int i = 3; i <= sqrt(n); i += 2) {
		while (n % i == 0) {
			gpd = i;
			n = n / i;
		}
	}
	if (n > 2) gpd = n;
	return gpd;
}
using Matrix = vector<vector<int>>;

// Функция для инициализации матрицы размером N x N
void initializeMatrix(Matrix& matrix, int N) {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(1, 10);
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

int main() 
{
	cout << "Enter task number: " << endl;
	int state = -1;
	cin >> state;
	switch (state) {
		case 1:
		{
			int nthreads = 5, tid;
			cout << "Enter number of threads: "; cin >> nthreads;
			if (nthreads < 1) {
				cout << "Wrong number of threads!";
				return 0;
			}
			// Создание параллельной области
#pragma omp parallel private(tid) num_threads(nthreads)
			{
				// печать номера потока
				tid = omp_get_thread_num();
				printf("Hello World from thread = %d\n", tid);
				// Печать количества потоков – только master if (tid == 0) {
				nthreads = omp_get_num_threads(); printf("Number of threads = %d\n", nthreads);
			}
			break;

		}
		case 2: 
		{
			int N;
			cout << "Enter the value of N: ";
			cin >> N;

			// Создаем два потока
#pragma omp parallel num_threads(2)
			{

				// Получаем номер текущего потока
				int tid = omp_get_thread_num();

				// Первый поток выводит числа от 1 до N
				if (tid == 0) {
					for (int i = 1; i <= N; ++i) {
						cout << "Thread 1: " << i << endl;
					}
				}
				// Второй поток выводит слово "HELLO" N раз
				else if (tid == 1) {
					for (int i = 0; i < N; ++i) {
						cout << "Thread 2: HELLO" << endl;
					}
				}
			}
			break;
		}
		case 3:
		{
			int N;
			cout << "Input array size N: ";
			cin >> N;

			//Инициализация массивов
			vector<int> A(N, 3); 
			vector<int> B(N, 8); 
			vector<int> C(N);

			auto start_time = chrono::high_resolution_clock::now();
			//Распараллеливаем итерации цикла между потоками, размер уменьшается с каждым распределенеием
#pragma omp parallel for schedule(guided)
			for (int i = 0; i < N; ++i) 
				C[i] = A[i] * B[i];

			auto end_time = chrono::high_resolution_clock::now();
			chrono::duration<double, milli> elapsed_time = end_time - start_time;

			cout << "Result: \n";
			for (int i = 0; i < N; ++i) 
				cout << C[i] << " ";
			cout << "\n";

			cout << "Time: " << elapsed_time.count() << " ms.\n";


			break;
			break;
		}
		case 4: 
		{
			const int N = 1000000;
			vector<int> vec(N);

			// Генерация случайных чисел
			random_device rd;
			mt19937 gen(rd());
			uniform_int_distribution<> dis(1, 53425);

			for (int& val : vec) {
				val = dis(gen);
			}
			//Установка минимальных значений для поиска максимума 
			int max_val_reduction = INT_MIN;
			int max_val_critical = INT_MIN;
			int max_val_atomic = INT_MIN;

			//Операция редукции
#pragma omp parallel for reduction(max: max_val_reduction)
			for (int i = 0; i < N; ++i) {
				max_val_reduction = max(max_val_reduction, vec[i]);

			}


			//Операция с критической секцией
#pragma omp parallel for
			for (int i = 0; i < N; ++i) {
#pragma omp critical
				{
					max_val_critical = max(max_val_critical, vec[i]);
				}
			}

			//Использование атомарных операций
#pragma omp parallel for
			for (int i = 0; i < N; ++i) {
				int a = 0;
#pragma omp atomic
				a += max(max_val_atomic, vec[i]);
				max_val_atomic = a;
			}

			cout << "Reduction: Max value in vector is: " << max_val_reduction << endl;
			cout << "Critical: Max value in vector is: " << max_val_critical << endl;
			cout << "Atomic: Max value in vector is: " << max_val_atomic << endl;

			break;
		}
		case 5:
		{
			int N, sum = 0;
			cout << "Enter N: ";
			cin >> N;

			int M = 4; // Количество потоков для суммирования
			int K = 3; // Количество потоков для вычисления длины вектора

			omp_set_nested(1); // Включение вложенного параллелизма

#pragma omp parallel sections
			{
#pragma omp section
				{
					// Вычисление суммы от 1 до N
#pragma omp parallel for num_threads(M) reduction(+:sum)
					for (int i = 1; i <= N; ++i) {
						if (i == 1) cout << "Threads: " << omp_get_num_threads() << " ";
						sum += i;
					}
					cout << "Sum from 1 to " << N << ": " << sum << endl;
				}

#pragma omp section
				{
					// Вычисление длины N-мерного вектора (будет выведено k раз так как создается K потоков)
#pragma omp parallel for num_threads(K)
					for (int i = 0; i < K; ++i) {
						double length = sqrt(N);
#pragma omp critical
						cout << "Length of " << N << "-vector: " << length << endl;
					}
				}
			}
			break;
		}
		case 6:
		{
			vector<int> sizes = { 2 * 10000000, 5 * 10000000, 100000000 };
			vector<int> threads = { 1, 2, 4, 8 };
			vector<double> times;

			const int runs = 3; // Количество запусков для каждого теста
			double totalTime = 0;
			double timeForOneThread = 0;

			// Генерация случайных чисел
			random_device rd;
			mt19937 gen(rd());
			uniform_int_distribution<> dis(100000, 1000000);

			for (int N : sizes) {
				vector<int> vec(N);

				cout << "N = " << N << endl;

				for (int T : threads) {
					totalTime = 0;
					for (int run = 0; run < runs; ++run) {
						auto genStart = chrono::high_resolution_clock::now();
						// Генерация чисел для каждого запуска, чтобы избежать эффекта кэширования
						for (int& val : vec) 
							val = dis(gen);
						auto genEnd = chrono::high_resolution_clock::now();
						chrono::duration<double, milli> gen_time = genEnd - genStart;


						auto start_time = chrono::high_resolution_clock::now();
#pragma omp parallel for num_threads(T) schedule(dynamic)
						for (int i = 0; i < N; ++i)
						{
							vec[i] = gpd(vec[i]);
						}
						

						auto end_time = chrono::high_resolution_clock::now();
						chrono::duration<double, milli> elapsed_time = end_time - start_time;
						totalTime += elapsed_time.count() / 1000; // Время в секундах
					}

					double averageTime = totalTime / runs;
					cout << "Average time with " << T << " threads: " << averageTime << " s" << endl;
					times.push_back(averageTime);
				}

				// Расчет и вывод ускорения
				cout << "Acceleration:" << endl;
				for (int i = 1; i < times.size(); i++)
					cout << i * 2 << " " << times[0] / times[i] << endl;

				times.clear();
			}
			break;
		}
		case 7:
		{
			vector<int> sizes = { 500, 1000, 2000 }; // Размеры матриц
			vector<int> threads = { 1, 2, 4, 8 }; // Количество потоков
			vector<double> times;

			for (int N : sizes) {
				Matrix A(N, vector<int>(N));
				Matrix B(N, vector<int>(N));
				Matrix C(N, vector<int>(N));

				initializeMatrix(A, N);
				initializeMatrix(B, N);

				cout << "N = " << N << endl;
				for (int T : threads) {
					double totalTime = 0;
					const int runs = 3;
					for (int run = 0; run < runs; ++run) {
						auto start_time = chrono::high_resolution_clock::now();

						multiplyMatrices(A, B, C, N, T);

						auto end_time = chrono::high_resolution_clock::now();
						chrono::duration<double> elapsed_time = end_time - start_time;
						totalTime += elapsed_time.count();
					}
					double averageTime = totalTime / runs;
					cout << "Average time with " << T << " threads: " << averageTime << " s" << endl;
					times.push_back(averageTime);
				}

				// Расчет и вывод ускорения
				cout << "Acceleration:" << endl;
				for (int i = 1; i < times.size(); i++)
					cout << i * 2 << " " << times[0] / times[i] << endl;

				times.clear();
			}
			break;
		}
		case 8:
		{
			std::vector<int> sizes = { 5000, 10000, 20000 };
			vector<int> threads = { 1, 2, 4, 8 };
			const int runs = 3; // Количество запусков для каждого теста

			random_device rd;
			mt19937 gen(rd());
			uniform_int_distribution<> dis(1, 1000);
			vector<double> times;

			for (int N : sizes) {
				vector<vector<int>> matrix(N, vector<int>(N));

				cout << "N = " << N << endl;

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
						auto start_time = chrono::high_resolution_clock::now();
						int maxMin = numeric_limits<int>::min();

#pragma omp parallel for reduction(max : maxMin)
						for (int i = 0; i < N; ++i) 
							maxMin = *max_element(matrix[i].begin(), matrix[i].end());
						
						auto end_time = chrono::high_resolution_clock::now();
						chrono::duration<double, milli> elapsed_time = end_time - start_time;
						totalTime += elapsed_time.count();
					}
					double averageTime = totalTime / runs / 1000.0; // Преобразование в секунды
					cout << "Average time " << T << " threads: " << averageTime << " s" << endl;
					times.push_back(averageTime);
				}

				// Расчет и вывод ускорения
				cout << "Acceleration:" << endl;
				for (int i = 1; i < times.size(); i++)
					cout << threads[i] << " threads: " << times[0] / times[i] << "x" << endl;

				times.clear();

			}
			break;
		}
		default:
		{
			cout << "No task has been chosen." << endl;
			break;
		}
	}
}