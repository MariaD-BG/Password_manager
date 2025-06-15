# pragma once
#include "matrix.hpp"
#include<iostream>

class MatrixSquare : public Matrix{
public:
    
    MatrixSquare(std::vector< std::vector<int> > matrix) : Matrix(matrix){
        this->rows = matrix.size();
    }

    MatrixSquare(size_t rows) : Matrix(rows, rows) {}

    MatrixSquare(size_t rows, int fill) :  Matrix(rows, rows, fill) {}

    size_t get_size() const{ 
        return rows;
    }

    int determinantMod95() const;
    MatrixSquare inverseMod95() const;
    MatrixSquare adjugate() const;
    MatrixSquare getMinor(size_t row_ind, size_t col_ind) const;

    MatrixSquare blockDiag(size_t num) const;

private:
    bool checkSquare(std::vector< std::vector<int> > matrix);
};