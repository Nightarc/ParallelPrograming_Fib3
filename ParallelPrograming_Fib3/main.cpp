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
// Функция для умножения матрицы на вектор
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

	int sizes[] = { 10, 3000, 5000, 10000 };
	vector<double> times;
	const int runs = 3;
	for (int n : sizes)
	{	
		vector<vector<int>> M(n);
		vector<int> vec(n);
		vector<int> result(n);
		if (rank == 0) {
			//Генерация случайных матрицы и вектора
			M.resize(n, vector<int>(n));
			for (size_t i = 0; i < n; i++)
			{
				M[i].resize(n);
				for (size_t j = 0; j < n; j++)
					M[i][j] = rand() % 10;
				vec[i] = rand() % 10;
			}
		}

		//Отправка вектора всем процессам
		MPI_Bcast(vec.data(), vec.size(), MPI_INT, 0, MPI_COMM_WORLD);

		// Распределение матрицы по процессам - каждый процесс вычисляет сколько он должен получить/отправить
		int rows_per_proc = n / size;
		int remainder = n % size;
		int start_row = rank * rows_per_proc + min(rank, remainder); //Последний процесс добирает оставшиеся массивы
		int end_row = start_row + rows_per_proc + (rank < remainder ? 1 : 0);

		//Кусочек матрицы каждого процесса
		vector<vector<int>> local_matrix(end_row - start_row, vector<int>(n));

		//Базовый процесс
		if (rank == 0) {
			//Распределение и отправка матрицы рабочим процессам
			for (int i = 0; i < size; i++)
			{
				//Вычисление количества рядов, которых необходимо отправить
				int start = i * rows_per_proc + min(i, remainder);
				int end = start + rows_per_proc + (i < remainder ? 1 : 0);

				if (i != 0) {
					//Отправка вычисленного количество рядов процессу
					for (int j = 0; j < end; j++) {
						MPI_Send(M[j].data(), n, MPI_INT, i, 0, MPI_COMM_WORLD);

					}
				}
				else {
					local_matrix = vector<vector<int>>(M.begin() + start, M.begin() + end);
				}
			}
		}
		//Рабочие процессы
		else {
			//Приём куска матрицы
			for (int i = 0; i < end_row - start_row; ++i) {
				MPI_Recv(local_matrix[i].data(), n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
		}
		//Вычисление произведения
		double total_time = 0.0;
		for (int i = 0; i < runs; ++i) {
			double start_time = MPI_Wtime();
			vector<int> local_result(end_row - start_row);
			matrix_vector_multiply(local_matrix, vec, local_result, 0,
				end_row - start_row);

			// Сбор результатов
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

		//Подсчет времени
		if (rank == 0) {
			double average_time = total_time / runs;
			times.push_back(average_time);
			cout << "N = " << n << ", Processes = " << size << ", Average Time : " << average_time << " seconds" << endl;
		}
	}
	//Подсчет ускорения
	if (rank == 0) {
		const int baseline_processes = 1;
		vector<double> speedup, efficiency;
		for (int i = 1; i < times.size(); ++i) {
			speedup.push_back(times[0] / times[i]);
			efficiency.push_back(speedup.back() / (i + 1));
		}
		for (size_t i = 0; i < times.size(); i++)
			cout << speedup[i] << ' ';
		cout << endl;
		for (size_t i = 0; i < times.size(); i++)
			cout << efficiency[i] << ' ';
	}	   
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}

//
// TASK 4
//	Два коммуникатора

// Функция для вычисления минимального значения в массиве
int find_min(const std::vector<int>& data, int l, int r) {
	return *std::min_element(data.begin() + l, data.begin() + r + 1);
}
// Функция для вычисления скалярного произведения двух векторов
int dot_product(const std::vector<int>& vec1, const std::vector<int>&vec2, int l, int r) {
	return std::inner_product(vec1.begin() + l, vec1.begin() + r + 1, vec2.begin() + l, 0);
}

int task4(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	srand(time(0));
	int rank, proc_num;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
						 
	// Вводим размеры массивов M и N
	int M = 0, N = 0;
	if (rank == 0) {
		std::cout << "Enter size M for array: ";
		std::cin >> M;
		std::cout << "Enter size N for vectors: ";
		std::cin >> N;
	}

	// Рассылаем размеры M и N всем процессам
	MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	// Создаем массивы и векторы, заполняем их случайными значениями
	std::vector<int> arr(M);
	std::vector<int> vec1(N), vec2(N);
	if (rank == 0) {
		std::generate(arr.begin(), arr.end(), []() { return rand() % 100 + 3; });
		std::generate(vec1.begin(), vec1.end(), []() { return rand() % 100 + 4; });
		std::generate(vec2.begin(), vec2.end(), []() { return rand() % 100 + 2; });
	}

	// Рассылаем данные массивов и векторов всем процессам
	MPI_Bcast(arr.data(), M, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(vec1.data(), N, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(vec2.data(), N, MPI_INT, 0, MPI_COMM_WORLD);

	// Определяем четный или нечетный процесс
	int color = rank % 2; // 0 для четных, 1 для нечетных процессов
	MPI_Comm new_comm;
	MPI_Comm_split(MPI_COMM_WORLD, color, rank, &new_comm);
	//Четные процессы вычисляют минимальное значение в массиве
	int global_min;
	int global_dot;
	if (color == 0) {
		//Распределяем массив для поиска минимума
		int even_group_rank = rank / 2;	   //ранг процесса в группе четных
		int chunk_size = 2 * M / proc_num ;	//половина процессов
		int remainder = M % proc_num;
		int start = even_group_rank * chunk_size + min(even_group_rank, remainder);
		int end = start + min(chunk_size, M - start - 1);

		int local_min = find_min(arr, start, end);
		//cout << "DEBUG: " << even_group_rank << "(" << rank << ")" << ": " << start << ' ' << end << " min: " << local_min << endl;

		MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, 0, new_comm);

		if (rank == 0) {
			cout << "Global minimum value in array: " << global_min << endl;
			if (M < 20) {
				cout << "arr: ";
				for (int i : arr)
					cout << i << ' ';
				cout << endl;
			}
		}
	}
	//Нечетные вычисляют скалярное произведение
	else {
		//Распределяем массив для скалярного произведения
		int odd_group_rank = rank / 2;	   //ранг процесса в группе четных
		int chunk_size = 2 * M / proc_num;	//половина процессов
		int remainder = M % proc_num;
		int start = odd_group_rank * chunk_size + min(odd_group_rank, remainder);
		int end = start + min(chunk_size, M - start - 1);

		if (rank == 1 && N < 20) {

			cout << "vec1: ";
			for (int i : vec1)
				cout << i << ' ';
			cout << endl;
			cout << "vec2: ";
			for (int i : vec2)
				cout << i << ' ';
			cout << endl;
			//cout << "DEBUG: " << start << ' ' << end << endl;
		}
		int local_dot = dot_product(vec1, vec2, start, end);
		//cout << odd_group_rank << '-' << rank << ": " << start << ' ' << end << ':' << local_dot;
		MPI_Reduce(&local_dot, &global_dot, 1, MPI_INT, MPI_SUM, 0, new_comm);

		if (rank == 1) {
			cout << "Global dot product of vectors: " << global_dot << endl;
		}
	}
	MPI_Finalize();
	return 0;
}

int main(int argc, char* argv[]) {
	task4(argc, argv);
}