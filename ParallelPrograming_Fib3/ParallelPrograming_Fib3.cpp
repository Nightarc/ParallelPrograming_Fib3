// ParallelPrograming_Fib3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <chrono>
#include <algorithm>
#include <vector>
#include <execution>
#include <random>

using namespace std;
const int base = 1000000000;
const int base_digits = 9;

template <typename TFunc> void RunAndMeasure(const char* title, TFunc func)
{
	const auto start = std::chrono::steady_clock::now();
	auto ret = func();
	const auto end = std::chrono::steady_clock::now();
	cout << title << ": " << std::chrono::duration <double, std::milli>(end - start).count() << " ms, res " << ret << "\n";
}

template <typename TFunc> void RunAndMeasure(const char* title, TFunc func, int execTimes)
{
	double avgTime = 0;
	for (size_t i = 0; i < execTimes; i++)
	{
		const auto start = std::chrono::steady_clock::now();
		auto ret = func();
		const auto end = std::chrono::steady_clock::now();
		avgTime += std::chrono::duration <double, std::milli>(end - start).count();
		cout << title << ", att " << i + 1 << ": " << std::chrono::duration <double, std::milli>(end - start).count() << " ms, res " << ret << "\n";
	}
	
	cout << "average time: " << avgTime / execTimes << " ms" << endl;
}

// 10e5 - 10e6
// (RAND() % 10e6) + 10e5 
vector<int> createVectorRandom(int n, int leftBorder, int rightBorder) {
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> distr(leftBorder, rightBorder); // define the range

	vector<int> res;
	for (size_t i = 0; i < n; i++)
		res.push_back(distr(gen));

	return res;
}

vector<double> createVectorRandomDouble(int n, double leftBorder, double rightBorder) {
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_real_distribution distr(leftBorder, rightBorder); // define the range

	vector<double> res;
	for (size_t i = 0; i < n; i++)
		res.push_back(distr(gen));

	return res;
}

int findFactors(int n) {
	int c = 0; // 1 and n are always factors, we won't be accounting for them
	if (n % (int)sqrt(n) == 0) c++;
	for (size_t i = 2; i < sqrt(n); i++)
		if (n % i == 0) c += 2;
	return c;
}

template <typename Policy>
int printNumberOfFactors(vector<int> vec, Policy policy) {
	//Применяем функцию нахождения количества делителей к каждому элементу вектора
	transform(policy, vec.begin(), vec.end(), vec.begin(), findFactors);
	return 0;
}



template <typename Policy>
__int64 vectorSum(vector<int> vec, Policy policy) {
	//Применяем функцию reduce для нахождения суммы вектора
	return reduce(policy, vec.begin(), vec.end(), (__int64)0);
}

template <typename T>
void printVector(vector<T> vec) {
	for (size_t i = 0; i < vec.size(); i++)
		cout << vec[i] << ' ';
	cout << endl;
}

void RunFindFactorTests() {
	srand(time(0));
	const int n1 = 5 * 1e5;
	const int n2 = 1e6;
	const int n3 = 2 * 1e6;
	const int leftBorder = 1e5;
	const int rightBorder = 1e6;
	vector<int> vec1 = createVectorRandom(n1, leftBorder, rightBorder);

	//onelaunch(vec);
	RunAndMeasure("printNumberOfFactors 5 * 10e5 seq", [&vec1] {
		return printNumberOfFactors(vec1, execution::seq);
		});

	RunAndMeasure("printNumberOfFactors 5 * 10e5 par", [&vec1] {
		return printNumberOfFactors(vec1, execution::par);
		});

	RunAndMeasure("printNumberOfFactors 5 * 10e5 par_unseq", [&vec1] {
		return printNumberOfFactors(vec1, execution::par_unseq);
		});

	vector<int> vec2 = createVectorRandom(n2, leftBorder, rightBorder);
	RunAndMeasure("printNumberOfFactors 10e6 seq", [&vec2] {
		return printNumberOfFactors(vec2, execution::seq);
		});

	RunAndMeasure("printNumberOfFactors 10e6 par", [&vec2] {
		return printNumberOfFactors(vec2, execution::par);
		});

	RunAndMeasure("printNumberOfFactors 10e6 par_unseq", [&vec2] {
		return printNumberOfFactors(vec2, execution::par_unseq);
		});


	vector<int> vec3 = createVectorRandom(n3, leftBorder, rightBorder);
	RunAndMeasure("printNumberOfFactors 2* 10e6 seq", [&vec3] {
		return printNumberOfFactors(vec3, execution::seq);
		});

	RunAndMeasure("printNumberOfFactors 2* 10e6 par", [&vec3] {
		return printNumberOfFactors(vec3, execution::par);
		});

	RunAndMeasure("printNumberOfFactors 2* 10e6 par_unseq", [&vec3] {
		return printNumberOfFactors(vec3, execution::par_unseq);
		});
}

void RunSingleTestReduce() {
	int n1 = 2;
	const int leftBorder = 1e5; //100.000
	const int rightBorder = 1e6; //1.000.000
	vector<int> vec1 = createVectorRandom(n1, leftBorder, rightBorder);
	if (n1 < 500)printVector(vec1);
	cout << vectorSum(vec1, execution::seq) << endl;
}
void RunFindReduceTests() {
	srand(time(0));
	const unsigned long int n0 = 1e5;
	const unsigned long int n1 = 5 * 1e8;
	const __int64  n2 = 1e9;
	const __int64 n3 = 2 * 1e9;
	const unsigned long int leftBorder = 10e5;
	const unsigned long int rightBorder = 10e6;
	vector<int> vec1 = createVectorRandom(n1, leftBorder, rightBorder);

	RunAndMeasure("vectorSum 5 * 10e8 PAR", [&vec1] {
		return vectorSum(vec1, execution::par);
		}, 3);
	RunAndMeasure("vectorSum 5 * 10e8 SEQ", [&vec1] {
		return vectorSum(vec1, execution::seq);
		}, 3);
	RunAndMeasure("vectorSum 5 * 10e8 PAR_UNSEQ", [&vec1] {
		return vectorSum(vec1, execution::par_unseq);
		}, 3);

	vec1.clear();
	vector<int> vec2 = createVectorRandom(n2, leftBorder, rightBorder);
	RunAndMeasure("vectorSum 10e9 SEQ", [&vec2] {
		return vectorSum(vec2, execution::seq);
		}, 3);
	RunAndMeasure("vectorSum 10e9 PAR", [&vec2] {
		return vectorSum(vec2, execution::par);
		}, 3);
	RunAndMeasure("vectorSum 10e9 PAR_UNSEQ", [&vec2] {
		return vectorSum(vec2, execution::par_unseq);
		}, 3);
	vec2.clear();

	vector<int> vec3 = createVectorRandom(n1, leftBorder, rightBorder);
	RunAndMeasure("vectorSum 2 * 10e9 SEQ", [&vec3] {
		return vectorSum(vec3, execution::seq);
		}, 3);
	RunAndMeasure("vectorSum 2 * 10e9 PAR", [&vec3] {
		return vectorSum(vec3, execution::par);
		}, 3);
	RunAndMeasure("vectorSum 2 * 10e9 PAR_UNSEQ", [&vec3] {
		return vectorSum(vec3, execution::par_unseq);
		}, 3);
	vec3.clear();

}
vector<vector<int>> CreateMatrix(int n) {
	vector<vector<int>> M;
	for (size_t i = 0; i < n; i++)
	{
		vector<int> filVec;
		for (size_t i = 0; i < n; i++)
			filVec.push_back(0);
		M.push_back(filVec);
	}
	return M;

}

vector<vector<int>> CreateRandomMatrix(int n, unsigned long int leftBorder, unsigned long int rightBorder) {
	vector<vector<int>> M;
	for (size_t i = 0; i < n; i++)
		M.push_back(createVectorRandom(n, leftBorder, rightBorder));
	return M;
}


int MultiplyStep(const vector<vector<int>> &M1, const vector<vector<int>> &M2, int rowPtr, int colPtr) {
	int sum = 0;
	for (size_t i = 0; i < M1.size(); i++)
		sum += M1[rowPtr][i] * M2[i][colPtr];

	return sum;
}
void Transpose(vector<vector<int>> &M) {
	for (size_t i = 0; i < M.size(); i++)
	{
		for (size_t j = i+1; j < M.size(); j++)
		{
			swap(M[i][j], M[j][i]);
		}
	}
	
}
template <typename Policy>
vector<vector<int>> MultiplyMatrices(Policy policy, vector<vector<int>> &M1, vector<vector<int>> &M2) {
	vector<vector<int>> R = CreateMatrix(M1.size());
	Transpose(M2);

	for (size_t i = 0; i < M1.size(); i++)
		for (size_t j = 0; j < M1.size(); j++)
			R[i][j] = transform_reduce(policy, M1[i].begin(), M1[i].end(), M2[j].begin(), 0);

	return R;
}

void RunMatricesMultiplicationTest() {
	const unsigned long int leftBorder = 1;
	const unsigned long int rightBorder = 10;

	int n = 4;

	vector<vector<int>> M1 = CreateRandomMatrix(n, leftBorder, rightBorder);
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < n; j++)
			cout << M1[i][j] << ' ';
		cout << endl;
	}
	cout << endl;
	vector<vector<int>> M2 = CreateRandomMatrix(n, leftBorder, rightBorder);
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < n; j++)
			cout << M2[i][j] << ' ';
		cout << endl;
	}
	cout << endl;
	vector<vector<int>> R = MultiplyMatrices(execution::par, M1, M2);

	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < n; j++)
			cout << R[i][j] << ' ';
		cout << endl;
	}
}


template <typename Policy>
void RunSortingTest(Policy policy) {
	vector<double> arr = createVectorRandomDouble(10, 1, 20);
	printVector(arr);
	sort(policy, arr.begin(), arr.end());
	cout << "sorted: " << endl;
	printVector(arr);
}

int main() {
	RunSortingTest(execution::par);
	return 0;
}
