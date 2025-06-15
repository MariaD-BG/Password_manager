#include "matrix.hpp"
#include<vector>
#include<iostream>

bool Matrix::validateMatrix(std::vector< std::vector<int> >& matrix){
    size_t rows = matrix.size();
    if(rows==0) return false; /// We cannot allow an empty matrix;
    size_t cols = matrix[0].size();
    for(size_t i=1;i<rows;i++){
        cols = Utils::min(cols, matrix[i].size());
    }
    if(cols==0) return false; /// We cannot allow an empty matrix;
    for(size_t i=0; i<rows; i++){
        while(matrix[i].size()>cols){
            matrix[i].pop_back();
        }
    }
    return true;
}

Matrix Matrix::operator*(const Matrix& other) const{
    /// We will assume this is only called on valid dimensions
    Matrix res(this->rows, other.cols);
    for(size_t i=0;i<this->rows;i++){
        for(size_t j=0;j<other.cols;j++){
            for(size_t k=0;k<this->cols;k++){
                res[i][j]+=(*this)[i][k]*other[k][j];
                res[i][j]=Utils::mod95(res[i][j]);
            }
        }
    }
    return res;
}
