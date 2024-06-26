#include <mpi.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

//Task1
double calculate_ln2(int n, int rank, int size) {
	double local_sum = 0.0;
	for (int k = rank + 1; k <= n; k += size) {
		local_sum += pow(-1, k - 1) / k;
	}
	return local_sum;
}

void calc_pair_operations(int n, int rank, int size, double& global_sum)
{
	double local_sum = calculate_ln2(n, rank, size);
	if (rank == 0) {
		global_sum = local_sum;
		for (int i = 1; i < size; ++i) {
			double recv_sum;
			MPI_Recv(&recv_sum, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);
			global_sum += recv_sum;
		}
	}
	else {
		MPI_Send(&local_sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
}

void calc_coll_operations(int n, int rank, int size, double&
	global_sum) {
	double local_sum = calculate_ln2(n, rank, size);
	MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0,
		MPI_COMM_WORLD);
}

int task1(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	const int ns[] = { 1e8, 5*1e8, 1e9 };
	const int runs = 3;
	vector<double> times_pairwise, times_collective;
	for (int n : ns) {
		double total_time_pairwise = 0.0;
		double total_time_collective = 0.0;

		//������ ������
		for (int i = 0; i < runs; ++i) {
			double start_time = MPI_Wtime();
			double global_sum = 0.0;
			calc_pair_operations(n, rank, size, global_sum);
			double end_time = MPI_Wtime();
			total_time_pairwise += (end_time - start_time);
		}

		//������������ ������
		for (int i = 0; i < runs; ++i) {
			double start_time = MPI_Wtime();
			double global_sum = 0.0;
			calc_coll_operations(n, rank, size, global_sum);
			double end_time = MPI_Wtime();
			total_time_collective += (end_time - start_time);
		}

		if (rank == 0) {
			double average_time_pairs = total_time_pairwise / runs;
			double average_time_coll = total_time_collective /
				runs;
			times_pairwise.push_back(average_time_pairs);
			times_collective.push_back(average_time_coll);
			cout << "N = " << n << ", Processes = " << size << ", Average Time(Pairwise) : " << average_time_pairs << " seconds" << endl;
			cout << "N = " << n << ", Processes = " << size << ", Average Time(Collective) : " << average_time_coll << " seconds" << endl;
		}
	}
	if (rank == 0) {
		const int baseline_processes = 1;
		vector<double> speedup_pairwise, speedup_collective;
		vector<double> efficiency_pairwise, efficiency_collective;
		for (int i = 1; i < times_pairwise.size(); ++i) {
			speedup_pairwise.push_back(times_pairwise[0] /
				times_pairwise[i]);
			speedup_collective.push_back(times_collective[0] /
				times_collective[i]);
			efficiency_pairwise.push_back(speedup_pairwise.back() / (i +
				1));
			efficiency_collective.push_back(speedup_collective.back() /
				(i + 1));
		}
	}
	MPI_Finalize();
	return 0;
}

//
//TASK 2
//
bool is_sorted_chunk(const vector<int>& chunk) {
	return adjacent_find(chunk.begin(), chunk.end(), greater<int>()) ==
		chunk.end();
}
bool check_sorted(const vector<int>& array, int rank, int size, int n) {
	int chunk_size = n / size;
	int remainder = n % size;
	int start = rank * chunk_size + min(rank, remainder);
	int end = start + chunk_size + (rank < remainder ? 1 : 0);
	vector<int> chunk(array.begin() + start, array.begin() + end);
	bool local_sorted = is_sorted_chunk(chunk);
	bool global_sorted = false;
	MPI_Allreduce(&local_sorted, &global_sorted, 1, MPI_C_BOOL,
		MPI_LAND, MPI_COMM_WORLD);
	return global_sorted;
}
	

int task2(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	const int ns[] = { 1e8, 5*1e8 };
	const int runs = 3;
	vector<double> times;
	for (int n : ns) {
		vector<int> array;
		if (rank == 0) {
			array.resize(n);
			iota(array.begin(), array.end(), 1);
		}
		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
		if (rank != 0) {
			array.resize(n);
		}
		MPI_Bcast(array.data(), n, MPI_INT, 0, MPI_COMM_WORLD);
		double total_time = 0.0;
		for (int i = 0; i < runs; ++i) {
			double start = MPI_Wtime(); 
			bool sorted = check_sorted(array, rank, size, n);
			double end = MPI_Wtime();
			total_time += (end - start);
			if (rank == 0 && i == runs - 1) {
				cout << "N = " << n << ", Processes = " << size << endl;
			}
		}
		if (rank == 0) {
			double average_time = total_time / runs;
			times.push_back(average_time);
			cout << "N = " << n << ", Processes = " << size << ", Average Time : " << average_time << " seconds" << endl;
		}
	}
	if (rank == 0) {
		const int baseline_processes = 1;
		vector<double> speedup, efficiency;
		for (int i = 1; i < times.size(); ++i) {
			speedup.push_back(times[0] / times[i]);
			efficiency.push_back(speedup.back() / (i + 1));
		}
	}
	MPI_Finalize();
	return 0;
}

//
// TASK 3
//
// ������� ��� ��������� ������� �� ������
void matrix_vector_multiply(const vector<vector<int>>& matrix, const
	vector<int>& vec, vector<int>& result, int start_row, int end_row) {
	for (int i = start_row; i < end_row; ++i) {
		result[i - start_row] = 0;
		for (int j = 0; j < vec.size(); ++j) 
			result[i - start_row] += matrix[i][j] * vec[j];
	}
}

int task3(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	srand(time(0));

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int sizes[] = { 3000, 5000, 10000 };
	vector<double> times;
	const int runs = 3;
	for (int n : sizes)
	{	
		vector<vector<int>> M(n);
		vector<int> vec(n);
		vector<int> result(n);
		if (rank == 0) {
			//��������� ��������� ������� � �������
			M.resize(n, vector<int>(n));
			for (size_t i = 0; i < n; i++)
			{
				for (size_t j = 0; j < n; j++)
					M[i][j] = rand() % 10;
				vec[i] = rand() % 10;
			}
				
			g//�������� ������� � ������� ���� ���������
			MPI_Bcast(vec.data(), vec.size(), MPI_INT, 0, MPI_COMM_WORLD);

			// ������������� ������� �� ��������� - ������ ������� ��������� ������� �� ������ ��������/���������
			int rows_per_proc = n / size;
			int remainder = n % size;
			int start_row = rank * rows_per_proc + min(rank, remainder); //��������� ������� �������� ���������� �������
			int end_row = start_row + rows_per_proc + (rank < remainder ? 1 : 0);

			//������� ������� ������� ��������
			vector<vector<int>> local_matrix(end_row - start_row, vector<int>(n));

			//������� �������
			if (rank == 0) {
				//������������� � �������� ������� ������� ���������
				for (int i = 0; i < size; i++)
				{
					//���������� ���������� �����, ������� ���������� ���������
					int start = i * rows_per_proc + min(i, remainder);
					int end = start + rows_per_proc + (i < remainder ? 1 : 0);

					if (i != 0) {
						//�������� ������������ ���������� ����� ��������
						for (int j = 0; j < end; j++)
							MPI_Send(M[j].data(), n, MPI_INT, i, 0, MPI_COMM_WORLD);
					}
					else {
						local_matrix = vector<vector<int>>(M.begin() + start, M.begin() + end);
					}
				}
			}
			//������� ��������
			else {
				//���� ����� �������
				for (int i = 0; i < end_row - start_row; ++i)
					MPI_Recv(local_matrix[i].data(), n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
			//���������� ������������
			double total_time = 0.0;
			for (int i = 0; i < runs; ++i) {
				double start_time = MPI_Wtime();
				vector<int> local_result(end_row - start_row);
				matrix_vector_multiply(local_matrix, vec, local_result, 0,
					end_row - start_row);

				// ���� �����������
				vector<int> recvcounts(size);
				vector<int> displs(size);
				for (int i = 0; i < size; ++i) {
					recvcounts[i] = rows_per_proc + (i < remainder ? 1 : 0);
					displs[i] = (i == 0) ? 0 : displs[i - 1] + recvcounts[i];
				}
				MPI_Gatherv(local_result.data(), local_result.size(),
					MPI_INT, result.data(), recvcounts.data(), displs.data(), MPI_INT, 0,
					MPI_COMM_WORLD);
				double end_time = MPI_Wtime();
				total_time += (end_time - start_time);
			}

			//������� �������
			if (rank == 0) {
				double average_time = total_time / runs;
				times.push_back(average_time);
				cout << "N = " << n << ", Processes = " << size << ", Average Time : " << average_time << " seconds" << endl;
			}
		}
	}
	//������� ���������
	if (rank == 0) {
		const int baseline_processes = 1;
		vector<double> speedup, efficiency;
		for (int i = 1; i < times.size(); ++i) {
			speedup.push_back(times[0] / times[i]);
			efficiency.push_back(speedup.back() / (i + 1));
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}

	

	




int main(int argc, char* argv[]) {
	task3(argc, argv);
}