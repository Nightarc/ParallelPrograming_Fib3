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

		//Парные обмены
		for (int i = 0; i < runs; ++i) {
			double start_time = MPI_Wtime();
			double global_sum = 0.0;
			calc_pair_operations(n, rank, size, global_sum);
			double end_time = MPI_Wtime();
			total_time_pairwise += (end_time - start_time);
		}

		//Коллективные обмены
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
// 
// 

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
//
//

int main(int argc, char* argv[]) {

}