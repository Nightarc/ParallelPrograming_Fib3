#include "mpi.h"
#include <iostream>
#include <vector>
#include <numeric>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;
void master(int num_arrays, int array_size, int num_procs) {
	vector<vector<int>> arrays(num_arrays, vector<int>(array_size));
	int array_index = 0, active_slaves = 0;
	int global_sum = 0;
	// Инициализация массивов случайными числами
	srand(time(0));
	for (auto& arr : arrays) {
		generate(arr.begin(), arr.end(), []() { return rand() % 100; });
	}
	vector<MPI_Request> requests(num_procs);
	vector<int> done_flags(num_procs, 0);
	double start_time = MPI_Wtime();
	// Распределение первых массивов
	for (int i = 1; i < num_procs && array_index < num_arrays; ++i, +
		+array_index) {
		MPI_Isend(arrays[array_index].data(), array_size, MPI_INT, i, 0,
			MPI_COMM_WORLD, &requests[i]);
		++active_slaves;
	}
	// Получение результатов и отправка новых задач
	while (array_index < num_arrays || active_slaves > 0) {
		for (int i = 1; i < num_procs; ++i) {
			if (done_flags[i] == 0) {
				int flag;
				MPI_Test(&requests[i], &flag, MPI_STATUS_IGNORE);
				if (flag) {
					int local_sum;
					MPI_Status status;
					MPI_Recv(&local_sum, 1, MPI_INT, MPI_ANY_SOURCE,
						MPI_ANY_TAG, MPI_COMM_WORLD, &status);
					global_sum += local_sum;
					--active_slaves;
					if (array_index < num_arrays) {
						MPI_Isend(arrays[array_index].data(),
							array_size, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD,
							&requests[i]);
						++array_index;
						++active_slaves;
					}
					else {
						int done = 1;
						MPI_Isend(&done, 1, MPI_INT, status.MPI_SOURCE,
							1, MPI_COMM_WORLD, &requests[i]); // Сообщение об окончании
						done_flags[i] = 1;
					}
				}
			}
		}
	}
	double end_time = MPI_Wtime();
	double elapsed_time = end_time - start_time;
	cout << "Global sum: " << global_sum << endl;
	cout << "Elapsed time (non-blocking): " << elapsed_time << "
		seconds" << endl;
}
void slave(int array_size) {
	vector<int> array(array_size);
	while (true) {
		MPI_Status status;
		MPI_Recv(array.data(), array_size, MPI_INT, 0, MPI_ANY_TAG,
			MPI_COMM_WORLD, &status);
		if (status.MPI_TAG == 1) {
			break; // Окончание работы
		}
		int local_sum = accumulate(array.begin(), array.end(), 0);
		MPI_Send(&local_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
}
int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	int rank, num_procs;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	const int num_arrays[] = { 100, 10000, 1000000 };
	const int array_sizes[] = { 1000000, 10000, 100 };
	for (int i = 0; i < 3; ++i) {
		if (rank == 0) {
			cout << "Testing for N = " << num_arrays[i] << " and M = "
				<< array_sizes[i] << endl;
			master(num_arrays[i], array_sizes[i], num_procs);
		}
		else {
			slave(array_sizes[i]);
		}
		MPI_Barrier(MPI_COMM_WORLD); // Синхронизация перед началом
		следующего теста
	}
	MPI_Finalize();
	return 0;
}