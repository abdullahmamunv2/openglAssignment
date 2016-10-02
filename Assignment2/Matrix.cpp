#include "Matrix.h"
#include<iostream>



Matrix::Matrix(int row,int col){
            this->row=row;
            this->col=col;
            ara=new double*[row];
            for(int i=0;i<row;i++){
                ara[i]=new double[col];
            }
            for(int i=0;i<row;i++){
                for(int j=0;j<col;j++){
                    if(i==j)
                        ara[i][j]=1;
                }
            }


        }

int Matrix::getRowLength(){
            return row;
        }

int Matrix::getColLength(){
            return col;
        }
void Matrix::convertToIdentityMatrix(){

        }

void Matrix::print_Mat(){
            for(int i=0;i<row;i++){
                for(int j=0;j<col;j++){
                    std::cout<<ara[i][j]<<"\t";
                }
                std::cout<<std::endl;
            }
        }

