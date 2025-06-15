#include "matrixSquare.hpp"

MatrixSquare MatrixSquare::inverseMod95() const {
    int det = (*this).determinantMod95();
    int detInv = Utils::modInverse95(det);

    if(detInv==0) throw("Matrix not invertible");

    MatrixSquare adj = this->adjugate();

    MatrixSquare inverse(this->rows);

    for (size_t i = 0; i < this->rows; ++i) {
        for (size_t j = 0; j < this->rows; ++j) {
            inverse[i][j] = Utils::mod95(adj[i][j] * detInv);
        }
    }

    return inverse;
}



MatrixSquare MatrixSquare::adjugate() const{
    MatrixSquare adjugate(rows);
    for(size_t i=0;i<rows;i++){
        for(size_t j=0;j<rows;j++){
            int factor = (i+j)%2==0 ? 1 : RANGE - 1;
            adjugate[j][i] = Utils::mod95(factor*((*this).getMinor(i,j)).determinantMod95());
        }
    }
    return adjugate;
}

int MatrixSquare::determinantMod95() const{
    
    if(rows==1) return (*this)[0][0];

    int det = 0;
    for(size_t i=0;i<rows;i++){
        int factor = i%2==0 ? 1 : RANGE-1;
        MatrixSquare minor = this->getMinor(0,i);
        det+=Utils::mod95(factor*(*this)[0][i]*minor.determinantMod95());
        det=Utils::mod95(det);
    }

    return det;

}


MatrixSquare MatrixSquare::getMinor(size_t row_ind, size_t col_ind) const{
    MatrixSquare minor(this->rows-1);
    for(size_t i=0;i<this->rows-1;i++){
        for(size_t j=0;j<this->rows-1;j++){
            size_t ind_r = i < row_ind ? i : i+1;
            size_t ind_c = j < col_ind ? j : j+1;
            minor[i][j] = (*this)[ind_r][ind_c];
        }
    }
    return minor;
}


bool MatrixSquare::checkSquare(std::vector< std::vector<int> > matrix){
    size_t rows = matrix.size();
    for(size_t i=0;i<rows;i++){
        if(matrix[i].size()!=rows) return false;
    }
     return true;
}

MatrixSquare MatrixSquare::blockDiag(size_t num) const{
    MatrixSquare result(num*this->rows, 0);

    for(size_t i = 0; i<num; i++){
        for(size_t j=i*this->rows;j<(i+1)*this->rows;j++){
            for(size_t k=i*this->rows;k<(i+1)*this->rows;k++){
                result[j][k] = matrix[j%this->rows][k%this->rows];
            }
        }
    } 
    return result;
}  