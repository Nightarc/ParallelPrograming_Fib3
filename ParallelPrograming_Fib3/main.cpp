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
			// �������� ������������ �������
#pragma omp parallel private(tid) num_threads(nthreads)
			{
				// ������ ������ ������
				tid = omp_get_thread_num();
				printf("Hello World from thread = %d\n", tid);
				// ������ ���������� ������� � ������ master if (tid == 0) {
				nthreads = omp_get_num_threads(); printf("Number of threads = %d\n", nthreads);
			}
			break;

		}
		case 2: 
		{
			int N;
			cout << "Enter the value of N: ";
			cin >> N;

			// ������� ��� ������
#pragma omp parallel num_threads(2)
			{
				// �������� ����� �������� ������
				int tid = omp_get_thread_num();

				// ������ ����� ������� ����� �� 1 �� N
				if (tid == 0) {
					for (int i = 1; i <= N; ++i) {
						std::cout << "Thread 1: " << i << std::endl;
					}
				}
				// ������ ����� ������� ����� "HELLO" N ���
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
			cout << "Input array size N: ";
			cin >> N;

			//������������� ��������
			vector<int> A(N, 3); 
			vector<int> B(N, 8); 
			vector<int> C(N);

			auto start_time = chrono::high_resolution_clock::now();
			//���������������� �������� ����� ����� ��������, �� ��� ������ - ������� �� ������������
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

			// ��������� ��������� �����
			random_device rd;
			mt19937 gen(rd());
			uniform_int_distribution<> dis(1, 53425);

			for (int& val : vec) {
				val = dis(gen);
			}
			//��������� ����������� �������� ��� ������ ��������� 
			int max_val_reduction = INT_MIN;
			int max_val_critical = INT_MIN;
			int max_val_atomic = INT_MIN;

			//�������� ��������
#pragma omp parallel for reduction(max: max_val_reduction)
			for (int i = 0; i < N; ++i) 
				max_val_reduction = max(max_val_reduction, vec[i]);


			//�������� � ����������� �������
#pragma omp parallel for
			for (int i = 0; i < N; ++i) {
#pragma omp critical
				{
					max_val_critical = max(max_val_critical, vec[i]);
				}
			}

			//������������� ��������� ��������
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
			std::cout << "������� ����� N: ";
			std::cin >> N;

			int M = 2; // ���������� ������� ��� ������������
			int K = 1; // ���������� ������� ��� ���������� ����� �������

			omp_set_nested(1); // ��������� ���������� ������������

#pragma omp parallel sections
			{
#pragma omp section
				{
					// ���������� ����� �� 1 �� N
#pragma omp parallel for num_threads(M) reduction(+:sum)
					for (int i = 1; i <= N; ++i) {
						sum += i;
					}
					std::cout << "����� �� 1 �� " << N << ": " << sum << std::endl;
				}

#pragma omp section
				{
					// ���������� ����� N-������� ������� (����� �������� k ��� ��� ��� ��������� K �������)
#pragma omp parallel for num_threads(K)
					for (int i = 0; i < K; ++i) {
						double length = std::sqrt(N);
#pragma omp critical
						std::cout << "Length of " << N << "-vector: " << length << std::endl;
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

			const int runs = 3; // ���������� �������� ��� ������� �����
			double totalTime = 0;
			double timeForOneThread = 0;

			// ��������� ��������� �����
			random_device rd;
			mt19937 gen(rd());
			uniform_int_distribution<> dis(100000, 1000000);

			for (int N : sizes) {
				vector<int> vec(N);

				cout << "������������ ��� N = " << N << endl;

				for (int T : threads) {
					totalTime = 0;

					for (int run = 0; run < runs; ++run) {
						// ��������� ����� ��� ������� �������, ����� �������� ������� �����������
						for (int& val : vec) 
							val = dis(gen);

						omp_set_num_threads(T);
						auto start_time = chrono::high_resolution_clock::now();

#pragma omp parallel for schedule(dynamic)
						for (int i = 0; i < N; ++i) {
							vec[i] = maxPrimeDivisor(vec[i]);
						}

						auto end_time = chrono::high_resolution_clock::now();
						chrono::duration<double, milli> elapsed_time = end_time - start_time;
						totalTime += elapsed_time.count() / 1000; // ����� � ��������
					}

					double averageTime = totalTime / runs;
					cout << "������� ����� ���������� �� " << T << " �������: " << averageTime << " ������" << endl;
					times.push_back(averageTime);
				}

				// ������ � ����� ���������
				cout << "���������:" << endl;
				for (int i = 1; i < times.size(); i++)
				{
					cout << i * 2 << " " << times[0] / times[i] << endl;
				}
				times.clear();
			}
			break;
		}
		case 7:
		{

			break;
		}
		case 8:
		{

			break;
		}
	}
}