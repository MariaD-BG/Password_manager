#pragma once
#include "utils.hpp"
#include<vector>
#include<cassert>
#include<iostream>

enum class Orientation { 
    Row, 
    Column 
};  

class Matrix{
public: 

    Matrix(size_t rows, size_t cols) : rows(rows), cols(cols){
        /// We will assume this is called on rows!=0
        std::vector<std::vector<int>> matrix(rows, std::vector<int>(cols, 0));
        this->matrix = matrix;
    }

    Matrix(size_t rows, size_t cols, int fill) : rows(rows), cols(cols) {
        /// We will assume this is called on rows != 0
        this->matrix = std::vector<std::vector<int>>(rows, std::vector<int>(cols, fill));
    }

    Matrix(std::vector< std::vector<int> > matrix){
        mod95(matrix);
        this->matrix = matrix;
        /// We assume validateMatrix will be called everytime before we try to construct an object
        /// Meaning that matrix is valid
        this->rows = matrix.size();
        this->cols = matrix[0].size();
    }

    Matrix(std::vector<int> matrix, Orientation orien){
        mod95(matrix);
        if(orien == Orientation::Row){
            this->rows = 1;
            this->cols = matrix.size();
            this->matrix = {matrix};
        }else{ ///orien == Orientation::Column
            this->rows = matrix.size();
            this->cols = 1;
            this->matrix = std::vector<std::vector<int>>(rows, std::vector<int>(1));;
            for(size_t i=0;i<rows;i++){
                this->matrix[i][0] = matrix[i];
            }
        }
    }

    Matrix operator*(const Matrix& other) const;

    bool validateMatrix(std::vector< std::vector<int> >& matrix);

    std::vector<int>& operator[](size_t row) {
        return matrix[row];
    }

    const std::vector<int>& operator[](size_t row) const {
        return matrix[row];
    }

    size_t get_rows() const{
        return rows;
    }
    size_t get_cols() const{
        return cols;
    }

    void print() const{
        for(size_t i=0;i<rows;i++){
            for(size_t j=0;j<cols;j++){
                std::cout<<(*this)[i][j]<<" ";
            }
            std::cout<<"\n";
        }
    }


protected:

    void mod95(std::vector< std::vector<int> >& matrix){
        size_t rows = matrix.size();
        assert(rows!=0);
        for(size_t i=0;i<rows;i++){
            for(size_t j=0;j<matrix[i].size();j++){
                matrix[i][j]%=95;
            }
        }
    }

    void mod95(std::vector<int>& matrix){
        size_t cols = matrix.size();
        for(size_t j=0;j<cols;j++){
            matrix[j]%=95;
        }
    }

    size_t rows;
    size_t cols;
    std::vector< std::vector<int> > matrix;

    static constexpr int RANGE = 95;
};