// ParallelPrograming_Fib3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <chrono>
#include "SquareMatrix_OneD.h"
#include "SquareMatrix_TwoD.h"
using namespace std;






double printDeltaTime_OneD(int matrixSize)
{
    SquareMatrix_OneD M1(matrixSize, matrixSize);
    SquareMatrix_OneD M2(matrixSize, matrixSize);

    for (size_t i = 0; i < matrixSize; i++)
    {
        for (size_t j = 0; j < matrixSize; j++)
        {
            M1.setCoefficient(i, j, 1);
            M2.setCoefficient(i, j, 1);
        }
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    start = std::chrono::high_resolution_clock::now();

    SquareMatrix_OneD result = M1.Multiple(M2);

    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    cout << "Matrix size = " << matrixSize << endl;
    cout << "Delta time = " << diff.count() << "s" << endl;
    return diff.count();
}

double printDeltaTime_TwoD(int matrixSize)
{
    SquareMatrix_TwoD M1(matrixSize, matrixSize);
    SquareMatrix_TwoD M2(matrixSize, matrixSize);

    for (size_t i = 0; i < matrixSize; i++)
    {
        for (size_t j = 0; j < matrixSize; j++)
        {
            M1.coefficients[i][j] = 1;
            M2.coefficients[i][j] = 1;
        }
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    start = std::chrono::high_resolution_clock::now();

    M1.Multiple(M2);

    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    cout << "Matrix size = " << matrixSize << endl;
    cout << "Delta time = " << diff.count() << "s" << endl;
    return diff.count();
}
void printAverageRuntime(int matrixSize, int count) {
    double avg = 0;
    for (size_t i = 0; i < count; i++)
        avg += printDeltaTime_OneD(matrixSize);
    cout << "One pointer average time = " << avg / count << endl;

    /*double avg = 0;
    for (size_t i = 0; i < count; i++)
        avg += printDeltaTime_TwoD(matrixSize);
    cout << "Two pointers average time = " << avg / count << endl;*/
}


int main()
{
    printAverageRuntime(512, 3);
    printAverageRuntime(1024, 3);
    printAverageRuntime(2048, 3);


}

/*
* 1.01284	0.1558	0.5249
6.44357	2.2128	10.4005
67.39	38.8445	83.2629

*/