// ParallelPrograming_Fib3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <chrono>
#include <execution>
#include <algorithm>
#include <vector>
using namespace std;

int randomNext(int start, int end)
{
	return rand() % (end - start + 1) + start;
}

int findFactors(int n) {
	int c = 0; // 1 and n are always factors, we won't be accounting for them
	if (n % (int)sqrt(n) == 0) c++;
	for (size_t i = 2; i < sqrt(n); i++)
		if (n % i == 0) c += 2;
	return c;
}

vector<int> printNumberOfFactors_seq(vector<int> vec) {
	//Применяем функцию нахождения количества делителей к каждому элементу вектора
	transform(execution::seq, vec.begin(), vec.end() - 1, vec.begin(), findFactors);
	return vec;
}

vector<int> printNumberOfFactors_par(vector<int> vec) {
	//Применяем функцию нахождения количества делителей к каждому элементу вектора
	transform(execution::par, vec.begin(), vec.end() - 1, vec.begin(), findFactors);
	return vec;
}

vector<int> printNumberOfFactors_parunseq(vector<int> vec) {
	//Применяем функцию нахождения количества делителей к каждому элементу вектора
	transform(execution::par_unseq, vec.begin(), vec.end() - 1, vec.begin(), findFactors);
	return vec;
}

int main() {
	srand(time(0));
	const int n = 10;
	const int leftBorder = 10e5;
	const int rightBorder = 10e6;
	vector<int> vec;
	for (size_t i = 0; i < n; i++)
	{
		vec.push_back(randomNext(leftBorder, rightBorder));
	}

	cout << findFactors(36) << endl;
}
