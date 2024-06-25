#include <mpi.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

int task1(int argc, char* argv[]) {
	int proc_rank, proc_num;
	MPI_Status st;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
	cout << "I am " << proc_rank + 1 << " process from " << proc_num << " processes!" << endl;
	MPI_Finalize();
	return 0;
}

int task2(int argc, char* argv[]) {
	int proc_rank, proc_num;
	MPI_Status st;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
	if (proc_rank == 0) {
		cout << "Number of processes: " << proc_num << endl;
	}
	if (proc_rank % 2 == 0) {
		cout << proc_rank << ": FIRST!" << endl;
	}
	else {
		cout << proc_rank << ": SECOND!" << endl;
	}
	MPI_Finalize();
	return 0;
}

constexpr auto MSGLEN = 32768; //размер сообщения
constexpr auto TAG_A = 100; constexpr auto TAG_B = 200;

int task3(int argc, char* argv[]) {
	vector<float> message1(MSGLEN), message2(MSGLEN); //пересылаемые сообщения
	int rank, dest, source, send_tag, recv_tag; MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Request send_request, recv_request;
	for (int i = 0; i < MSGLEN; i++) {
		message1[i] = 1 - 2 * rank;
	}

	if (rank == 0) {
		dest = 1;
		source = 1; send_tag = TAG_A; recv_tag = TAG_B;
	}
	else if (rank == 1) {
		dest = 0;
		source = 0; send_tag = TAG_B; recv_tag = TAG_A;
	}
	else {
		MPI_Finalize();
		return 0;
	}
	cout << "Task " << rank << " has sent the message" << endl;
	MPI_Isend(message1.data(), MSGLEN, MPI_FLOAT, dest, send_tag,
		MPI_COMM_WORLD, &send_request);
	MPI_Irecv(message2.data(), MSGLEN, MPI_FLOAT, source, recv_tag, MPI_COMM_WORLD, &recv_request);

	MPI_Wait(&send_request, &status);
	MPI_Wait(&recv_request, &status);

	cout << " Task " << rank << " has received the message" << endl;


	MPI_Finalize();
	return 0;
}

int task4(int argc, char* argv[]) {
	int proc_rank, proc_num, randNumber, number;
	MPI_Status st;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);

	if (proc_rank == 0) {
		srand(time(0));
		randNumber = rand() % 100;
		MPI_Send(&randNumber, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(&number, 1, MPI_INT, proc_num - 1, 0, MPI_COMM_WORLD, &st);
		if (number - randNumber == proc_num - 1) cout << "0: CORRECT!" << endl;
		else cout << "0: ERROR!" << endl;
	}
	else {
		MPI_Recv(&number, 1, MPI_INT, proc_rank - 1, 0, MPI_COMM_WORLD, &st);
		cout << proc_rank << ": Received a " << number << " from " << proc_rank - 1 << endl;
		number++;
		MPI_Send(&number, 1, MPI_INT, (proc_rank + 1) % proc_num, 0, MPI_COMM_WORLD);
		cout << proc_rank << ": Sent an " << number << " to " << (proc_rank + 1) % proc_num << endl;
	}

	MPI_Finalize();
	return 0;
}

//TASK 5: blocking
void master(int num_arrays, int array_size, int num_procs) {
	vector<vector<int>> arrays(num_arrays, vector<int>(array_size));
	int array_index = 0, active_slaves = 0;
	int global_sum = 0;



	// Инициализация массивов случайными числами
	srand(time(0));
	for (auto& arr : arrays)
		generate(arr.begin(), arr.end(), []() { return rand() % 100; });

	double start_time = MPI_Wtime();

	// Распределение первых массивов
	for (int i = 1; i < num_procs && array_index < num_arrays; ++i, ++array_index) {
		MPI_Send(arrays[array_index].data(), array_size, MPI_INT, i, 0,
			MPI_COMM_WORLD);
		++active_slaves;
	}

	// Основной рабочий цикл, получающий результаты и выдающий новые задачи процессам
	while (array_index < num_arrays || active_slaves > 0) {
		int local_sum;
		MPI_Status status;
		MPI_Recv(&local_sum, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
			MPI_COMM_WORLD, &status);
		global_sum += local_sum;
		--active_slaves;
		if (array_index < num_arrays) {
			MPI_Send(arrays[array_index].data(), array_size, MPI_INT,
				status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			++array_index;
			++active_slaves;
		}
		else {
			int done = 1;
			MPI_Send(&done, 1, MPI_INT, status.MPI_SOURCE, 1,
				MPI_COMM_WORLD); // Сообщение об окончании
		}
	}
	double end_time = MPI_Wtime();
	double elapsed_time = end_time - start_time;
	cout << "Sum: " << global_sum << endl;
	cout << "Time: " << elapsed_time << " seconds" << endl;
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

int task5_block(int argc, char* argv[]) {
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
		MPI_Barrier(MPI_COMM_WORLD); // Синхронизация перед началом следующего теста
	}
	MPI_Finalize();
	return 0;
}

//TASK 5: non-blocking
void master_nonblock(int num_arrays, int array_size, int num_procs) {
	vector<vector<int>> arrays(num_arrays, vector<int>(array_size));
	int array_index = 0, active_slaves = 0;
	int global_sum = 0;

	// Инициализация массивов случайными числами
	srand(time(0));
	for (auto& arr : arrays)
		generate(arr.begin(), arr.end(), []() { return rand() % 100; });

	vector<MPI_Request> requests(num_procs);
	vector<int> done_flags(num_procs, 0);

	double start_time = MPI_Wtime();

	// Распределение первых массивов
	for (int i = 1; i < num_procs && array_index < num_arrays; ++i, ++array_index) {
		MPI_Isend(arrays[array_index].data(), array_size, MPI_INT, i, 0,
			MPI_COMM_WORLD, &requests[i]);
		++active_slaves;
	}

	// Основной рабочий цикл, получающий результаты и выдающий новые задачи процессам
	while (array_index < num_arrays || active_slaves > 0) {
		for (size_t i = 0; i < num_procs; i++) //Итерация по списку процессов в поиске свободного
		{
			int flag;
			MPI_Test(&requests[i], &flag, MPI_STATUS_IGNORE);
			if (flag) {
				int local_sum;
				MPI_Status status;
				MPI_Recv(&local_sum, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
					MPI_COMM_WORLD, &status);
				global_sum += local_sum;
				--active_slaves;
				if (array_index < num_arrays) {
					MPI_Isend(arrays[array_index].data(), array_size, MPI_INT,
						status.MPI_SOURCE, 0, MPI_COMM_WORLD, &requests[i]);
					++array_index;
					++active_slaves;
				}
				else {
					int done = 1;
					MPI_Isend(&done, 1, MPI_INT, status.MPI_SOURCE, 1,
						MPI_COMM_WORLD, &requests[i]); // Сообщение об окончании
					done_flags[i] = 1;
				}
			}
		}

	}
	double end_time = MPI_Wtime();
	double elapsed_time = end_time - start_time;
	cout << "Sum: " << global_sum << endl;
	cout << "Time: " << elapsed_time << " seconds" << endl;
}

void slave_nonblock(int array_size) {
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

int task5_nonblock(int argc, char* argv[]) {
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
			master_nonblock(num_arrays[i], array_sizes[i], num_procs);
		}
		else {
			slave_nonblock(array_sizes[i]);
		}
		MPI_Barrier(MPI_COMM_WORLD); // Синхронизация перед началом следующего теста
	}
	MPI_Finalize();
	return 0;
}


int main(int argc, char* argv[])
{
	task5_nonblock(argc, argv);
}
