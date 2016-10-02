#ifndef Matrix_H
#define Matrix_H
class Matrix{
    protected:
        double ** ara;
        int row;
        int col;
    public:
        Matrix(int row=4,int col=4);

        int getRowLength();

        int getColLength();
        void convertToIdentityMatrix();

        void print_Mat();

};

#endif
