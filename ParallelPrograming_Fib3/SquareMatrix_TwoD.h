#pragma once
using namespace std;
#include <iostream>
class SquareMatrix_TwoD {
public:
    int** coefficients;
    int size;

    SquareMatrix_TwoD(int n, int m) {
        this->coefficients = new int* [n];
        for (size_t i = 0; i < n; i++)
        {
            coefficients[i] = new int[m];
            fill_n(coefficients[i], m, 0);
        }
        this->size = n;
    }

    void printCoefficients() {
        for (size_t i = 0; i < size; i++)
        {
            for (size_t j = 0; j < size; j++)
                cout << coefficients[i][j] << ' ';
            cout << endl;
        }
    }

    int getCoefficient(int i, int j) { return coefficients[i][j]; };

    SquareMatrix_TwoD Multiple(SquareMatrix_TwoD M2) {
        SquareMatrix_TwoD result(size, size);
        for (size_t i = 0; i < size; i++)
        {
            for (size_t j = 0; j < size; j++)
            {
                int sum = 0;
                for (size_t k = 0; k < M2.size; k++)
                    sum += coefficients[i][k] * M2.coefficients[k][j];
                result.coefficients[i][j] = sum;
            }

        }
        return result;
    }
};