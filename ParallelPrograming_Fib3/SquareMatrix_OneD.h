#pragma once
#include <iostream>
using namespace std;
class SquareMatrix_OneD {
public:
    int* coefficients;
    int size;

    SquareMatrix_OneD(int n, int m) {
        this->coefficients = new int[n * m];
        for (size_t i = 0; i < n; i++)
        {
            fill_n(coefficients, n * m, 0);
        }
        this->size = n;
    }



    int getCoefficient(int i, int j) { return coefficients[i * size + j]; };
    void setCoefficient(int i, int j, int val) { coefficients[i * size + j] = val; };

    void printCoefficients()
    {
        for (size_t i = 0; i < size; i++)
        {
            for (size_t j = 0; j < size; j++)
                cout << getCoefficient(i, j) << ' ';
            cout << endl;
        }
    }

    SquareMatrix_OneD Multiple(SquareMatrix_OneD M2)
    {
        SquareMatrix_OneD result(size, size);
        for (size_t i = 0; i < size; i++)
        {
            for (size_t j = 0; j < size; j++)
            {
                int sum = 0;
                for (size_t k = 0; k < M2.size; k++)
                    sum += getCoefficient(i, k) * M2.getCoefficient(k, j);
                result.setCoefficient(i, j, sum);
            }

        }
        return result;
    }
};
