#include<iostream>
#include<cmath>
#include <fstream>
#include<string>
#include <cstdlib>
#include<vector>


using namespace std;


string pointReflection[4];

class point
{
    public:
        double x,y,z,w;
        point(){
            w=1;
        }
        void print(){
            cout<<"X : "<<x<<" Y: "<<y<<" Z: "<<z<<" W:"<<w<<endl;
        }
        void setVal(string str,double val){
            if(str=="x")
                x=val;
            else if(str=="y")
                y=val;
            else if(str=="z")
                z=val;
            else
                w=val;
        }

        void arrayToPoint(double *points,int startIndex=0,int stopIndex=3){
            for(int i=startIndex;i<stopIndex;i++){
                setVal(pointReflection[i%3],points[i]);
            }
        }

        double * pointToArray(){
            double *points=new double[4];
            for(int i=0;i<4;i++){
                points[i]=getVal(pointReflection[i]);
            }

            return points;
        }

        double getVal(string str){
            double val;
            if(str=="x")
                val=x;
            else if(str=="y")
                val=y;
            else if(str=="z")
                val=z;
            else
                val=w;
            return val;
        }
        void convertToUnit(){
            double vectorValue=floor((sqrt(x*x+y*y+z*z)*100))/100;
            x/=vectorValue;
            y/=vectorValue;
            z/=vectorValue;
        }
};

class Utils{

    public:
        static double** MatrixMultiplication(double **M1 , double **M2,int row1,int col1,int row2,int col2 ){
            if(col1==row2){
                double ** result=new double*[row1];
                for(int i=0;i<row1;i++)
                    result[i]=new double[col1];

                for(int i=0;i<row1;i++)
                    for(int j=0;j<col2;j++){
                        double val=0;
                        for(int k=0;k<row2;k++){
                            val+=M1[i][k]*M2[k][j];
                        }
                        result[i][j]=val;
                    }
                return result;


            }
            else{
                cout<<"ERROR : Multiplication is not possible "<<endl;
            }
        }
        static point getPerpendicularVector(point look,point right,bool positive=true){

            if(positive){
                point p=crossProduct(right,look);
                p.convertToUnit();
                return p;
            }
            else{
                point p=crossProduct(look,right);
                p.convertToUnit();
                return p;
            }


        }
        static point getRightLeftVector(point look,point up,bool right=true){
            if(!right){
                point p=crossProduct(up,look);
                p.convertToUnit();
                return p;
            }
            else{
                point p=crossProduct(look,up);
                p.convertToUnit();
                return p;
            }


        }
        static point crossProduct(point u,point v){
            point p;
            p.x=floor((u.y*v.z-v.y*u.z)*100)/100;
            p.y=floor((v.x*u.z-u.x*v.z)*100)/100;
            p.z=floor((u.x*v.y-v.x*u.y)*100)/100;

            return p;
        }

        static point subTractPoint(point u,point v){
            u.x-=v.x;
            u.y-=v.y;
            u.z-=v.z;
        }

        static point rotatePoint(point base,double angle,point p){
            double cosA=cos(angle);
            double sinA=sin(angle);
            p.x-=base.x;
            p.y-=base.y;

            double newX=p.x*cosA -p.y*sinA;
            double newY=p.x*sinA+p.y*cosA;

            p.x=newX+base.x;
            p.y=newY+base.y;

            return p;
        }

};



/** gluPerspective **/

point Perspective;

/*********************** Matrix ****************************/

class Matrix{
    protected:
        double ** ara;
        int row;
        int col;
        int push_id;
    public:
        Matrix(int row=4,int col=4,int push_id=0){
            this->row=row;
            this->col=col;
            this->push_id=push_id;
            ara=new double*[row];
            for(int i=0;i<row;i++){
                ara[i]=new double[col];
            }
            for(int i=0;i<row;i++){
                for(int j=0;j<col;j++){
                    if(i==j)
                        ara[i][j]=1;
                    else
                        ara[i][j]=0;
                }
            }


        }

        int getPushId(){
            return push_id;
        }
        void setPushId(int push_id){
            this->push_id=push_id;
        }

        int getRowLength(){
            return row;
        }

        int getColLength(){
            return col;
        }
        void convertToIdentityMatrix(){

        }

        double ** get2dArray(){
            return ara;
        }

        void print_Mat(){
            for(int i=0;i<row;i++){
                for(int j=0;j<col;j++){
                    cout<<ara[i][j]<<"\t";
                }
                cout<<endl;
            }
        }

        void matrixMultiplyBy(Matrix *M){
            this->ara=Utils::MatrixMultiplication(M->get2dArray(),ara,M->getRowLength(),M->getColLength(),row,col);
            //this->ara=r
        }

};

class IdentityMatrix:public Matrix{
    public:

        IdentityMatrix():Matrix(){


        }
};

class TranslateMatrix:public Matrix{

    public:
        TranslateMatrix(point d):Matrix(){
            double *translate=d.pointToArray();
            for(int i=0;i<row-1;i++)
                ara[i][col-1]=translate[i];
        }
        TranslateMatrix(double * translate):Matrix(){
            for(int i=0;i<row-1;i++)
                ara[i][col-1]=translate[i];
        }


};

class ScaleMatrix:public Matrix{

    public:
        ScaleMatrix(point d):Matrix(){
            double *scale=d.pointToArray();
            for(int i=0;i<row-1;i++){
                for(int j=0;j<col-1;j++){
                    if(i==j)
                        ara[i][j]=scale[i];
                }
            }
        }
        ScaleMatrix(double * scale):Matrix(){
            for(int i=0;i<row-1;i++){
                for(int j=0;j<col-1;j++){
                    if(i==j)
                        ara[i][j]=scale[i];
                }
            }
        }


};

/***************************************************/


/***********************Model****************************/
/** interface Model
    each model mush implement this interface

**/
class Model{
    protected:
        point * tri;
        int _size;
    public:
        Model(int _size){
            tri=new point[_size];
            this->_size=_size;
        }

        void Transform(Matrix *mat){
            double ** matrix=mat->get2dArray();
            double *result=new double[4];
            for(int i=0;i<_size;i++){
                double * M2 = tri[i].pointToArray();
                for(int j=0;j<4;j++){
                    double val=0;
                    for(int k=0;k<4;k++){
                        val+=matrix[j][k]*M2[k];
                    }
                    result[j]=val;

                }
                tri[i].arrayToPoint(result,0,3);
            }
        }
        virtual void print(){
            cout<<"This is Model"<<endl;
        }
};



class Triangle:public Model{
    public:
        Triangle(int _size=3):Model(_size){
        }
        Triangle(double *val, int _size=3):Model(_size){
            point *p1=new point;
            point *p2=new point;
            point *p3=new point;
            p1->arrayToPoint(val,0,3);
            tri[0]=*p1;

            p2->arrayToPoint(val,3,6);
            tri[1]=*p2;

            p3->arrayToPoint(val,6,9);
            tri[2]=*p3;
        }
        void print(){
            for(int i=0;i<3;i++){
                for(int j=0;j<3;j++){
                    cout<<tri[i].getVal(pointReflection[j])<<" ";
                }
                cout<<endl;
            }
        }
};

class Line:public Model{
    public:
        Line(int _size=2):Model(_size){
        }
};

/***************************************************/

class Camera{

        point cameraPosition;
        /**camera look vector***/
        point l;
        /**camera up vector***/
        point u;

        /**camera right vector***/
        point r;
    public:
        Camera(){


        }

        Camera(point eye,point look,point up){
            cameraPosition=eye;

            l=Utils::subTractPoint(look,eye);
            l.convertToUnit();

            r=Utils::crossProduct(l,up);

            u=Utils::crossProduct(r,l);



        }



        void setCameraPosition(point cameraPosition){
            this->cameraPosition=cameraPosition;
        }

        point getCameraPosition(){
            return cameraPosition;
        }


        void setLookVector(point lookVector){
            this->l=lookVector;
        }

        point getLookVector(){
            return this->l;
        }

        void setUpVector(point upVector){
            this->u=upVector;
        }

        point getUpVector(){
            return u;
        }

        void setRightVector(point rightVector){
            this->r=rightVector;
        }

        point getRightVector(){
            return this->r;
        }

};

/**************** Global variable *********************/
vector<Model *> models;
vector<Matrix *>matrixs;
Camera *camera;

/*************************************/










/** file object **/



void Task1(ifstream &file){
    if(file.is_open()){
        point eye,look,up;
        string line;
        double val[16];

        for(int i=0;i<3;i++){
            file>>val[i];
        }
        eye.arrayToPoint(val);

        for(int i=0;i<3;i++){
            file>>val[i];
        }
        look.arrayToPoint(val);

        for(int i=0;i<3;i++){
            file>>val[i];
        }
        up.arrayToPoint(val);

        for(int i=0;i<4;i++){
            file>>val[i];
            Perspective.setVal(pointReflection[i],val[i]);
        }

        /****** camera initialization ******/
        camera=new Camera(eye,look,up);
        /*****************************/

        getline (file,line);

        int push,pop=0;
        while ( getline (file,line) )
        {
            if(line=="triangle"){
                for(int i=0;i<9;i++){
                    file>>val[i];
                }
                Model *triangle=new Triangle(val);
                triangle->Transform(matrixs[matrixs.size()-1]);
                models.push_back(triangle);
            }
            else if(line=="translate"){
                for(int i=0;i<3;i++){
                    file>>val[i];
                }
                Matrix *translate=new TranslateMatrix(val);
                translate->setPushId(push);
                translate->print_Mat();
                translate->matrixMultiplyBy(matrixs[matrixs.size()-1]);
                matrixs.push_back(translate);


            }
            else if(line=="scale"){
                for(int i=0;i<3;i++){
                    file>>val[i];
                }
                Matrix *scale=new ScaleMatrix(val);
                scale->setPushId(push);
                scale->print_Mat();
                scale->matrixMultiplyBy(matrixs[matrixs.size()-1]);
                matrixs.push_back(scale);
            }
            else if(line=="rotate"){

            }
            else if(line=="push"){
                push++;
            }
            else if(line=="pop"){
                if(push==0)
                    continue;
                for(int i=matrixs.size()-1;i>0;i--){
                    if(matrixs[i]->getPushId()==push ){
                        matrixs.erase(matrixs.begin()+i);
                    }
                    else
                        break;
                }
                pop--;
                push--;
            }
            else if(line=="end"){
                break;
            }
        }
    }
    else{
        cout<<"file is closed !!!!!!!!!!!!!"<<endl;
    }
}

void Task2(){
    point p=camera->cameraPosition();
    p.x*=-1;
    p.y*=-1;
    p.z*=1;
    Matrix * translateToOrigin = new TranslateMatrix(p);


}


int main(){

    ifstream myfile ("scene.txt");
    pointReflection[0]="x";
    pointReflection[1]="y";
    pointReflection[2]="z";
    pointReflection[3]="w";
    matrixs.push_back(new IdentityMatrix());


    Task1(myfile);
    Task2();
    for(int i=0;i<models.size();i++){
        cout<<"model : "<<i+1<<endl;
        models[i]->print();
    }


    /*double ** mat1=new double[2];
    double ** mat2=new double[2];
    for(int i=0;i<2;i++){
        mat1[2]=new double[2]
    }*/

    //test
//    point d;
//    d.x=4;
//    d.y=5;
//    d.z=4;
//    Matrix *mat=new TranslateMatrix(d);
//    mat->print_Mat();
    //
}
