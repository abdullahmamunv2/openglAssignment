#include<iostream>
#include<stdlib.h>
#include<math.h>


#include <GL/glut.h>

#include<vector>


using namespace std;

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double forwardMove=2;









class point
{
    public:
        double x,y,z;
        void print(){
            cout<<"X : "<<x<<endl<<"Y: "<<y<<endl<<"Z: "<<z<<endl;
        }
        void convertToUnit(){
            double vectorValue=floor((sqrt(x*x+y*y+z*z)*100))/100;
            x/=vectorValue;
            y/=vectorValue;
            z/=vectorValue;
        }
};



////// wheel ////////////////

point wheelCenter;
point wheelForwardVector;
point wheelRightVector;
double theta=0.0;
point wheelPoints[2][100];
double wheelRadius=30;
point globalZaxis;
int wheelSegments=20;
bool wheelMove=false;
bool wheelPreviousColor=true;  //// here true means red  false means green
double rotate_degree=0;



///// cube to sphere

double cubeLength;
point cubePoint;
double cubeTranslate;
double d=0;



class Utils{

    public:
        static point getPerpendicularVector(point look,point right,bool positive=true){

            if(positive){
                point p=crossProduct(look,right);
                p.convertToUnit();
                return p;
            }
            else{
                point p=crossProduct(right,look);
                p.convertToUnit();
                p.print();
                return p;
            }


        }
        static point getRightLeftVector(point look,point up,bool right=true){
            if(!right){
                up.z*=-1;
            }

            point p=crossProduct(look,up);
            p.convertToUnit();
            return p;
        }
        static point crossProduct(point u,point v){
            point p;
            p.x=floor((u.y*v.z-v.y*u.z)*100)/100;
            p.y=floor((v.x*u.z-u.x*v.z)*100)/100;
            p.z=floor((u.x*v.y-v.x*u.y)*100)/100;

            return p;
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


class CameraOperation{


        point cameraPosition;  ///// position of camera
        point lookVector;
        point upVector;
        point rightVector;
    public:
        CameraOperation(){
            cameraPosition.x=-200.0;
            cameraPosition.y=-200.0;
            cameraPosition.z=200.0;

            lookVector.x=(0.0-cameraPosition.x);
            lookVector.y=(0.0-cameraPosition.y);
            lookVector.z=(0.0-cameraPosition.z);
            lookVector.convertToUnit();

            upVector.x=0.0;
            upVector.y=0.0;
            upVector.z=1.0;

            rightVector=Utils::crossProduct(lookVector,upVector);
            rightVector.convertToUnit();

            //rightVector.x


        }
        void setCameraPosition(point cameraPosition){
            this->cameraPosition=cameraPosition;
        }

        point getCameraPosition(){
            return cameraPosition;
        }


        void setLookVector(point lookVector){
            this->lookVector=lookVector;
        }

        point getLookVector(){
            return this->lookVector;
        }

        void setUpVector(point upVector){
            this->upVector=upVector;
        }

        point getUpVector(){
            return upVector;
        }

        void setRightVector(point rightVector){
            this->rightVector=rightVector;
        }

        point getRightVector(){
            return this->rightVector;
        }

        /*
            axis=0 add unit with all axis
            axis=1 add unit with x axis
            axis=2 add unit with y axis
            axis=3 add unit with z axis
        */

        void updateCameraPosition(point unitVector,double unit){
            cameraPosition.x+=unitVector.x*unit;
            cameraPosition.y+=unitVector.y*unit;
            cameraPosition.z+=unitVector.z*unit;

        }



};



////Global camera

CameraOperation camera;





void drawAxes()
{
	if(drawaxes==1)
	{
		glBegin(GL_LINES);{
            glColor3f(1.0, 0, 0);
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

            glColor3f(0, 1.0, 0);
			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

            glColor3f(0, 0, 1.0);
			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
    glColor3f(0.6, 0.6, 0.6);	//grey
    glBegin(GL_LINES);{
        for(i=-8;i<=8;i++){

            if(i==0)
                continue;	//SKIP the MAIN axes

            //lines parallel to Y-axis
            glVertex3f(i*10, -90, 0);
            glVertex3f(i*10,  90, 0);

            //lines parallel to X-axis
            glVertex3f(-90, i*10, 0);
            glVertex3f( 90, i*10, 0);
        }
    }glEnd();

}


void drawWheelAxis(){

    glColor3f(0.0,0.8,0.5);
    glBegin(GL_QUADS);
    {

        glVertex3f(wheelPoints[0][0].x,wheelPoints[0][0].y,wheelPoints[0][0].z);
        glVertex3f(wheelPoints[1][0].x,wheelPoints[1][0].y,wheelPoints[0][0].z);
        glVertex3f(wheelPoints[1][wheelSegments/2].x,wheelPoints[1][wheelSegments/2].y,wheelPoints[0][wheelSegments/2].z);
        glVertex3f(wheelPoints[0][wheelSegments/2].x,wheelPoints[0][wheelSegments/2].y,wheelPoints[0][wheelSegments/2].z);


    }
    glEnd();

    glBegin(GL_QUADS);
    {

        glVertex3f(wheelPoints[0][wheelSegments/4].x,wheelPoints[0][wheelSegments/4].y,wheelPoints[0][wheelSegments/4].z);
        glVertex3f(wheelPoints[1][wheelSegments/4].x,wheelPoints[1][wheelSegments/4].y,wheelPoints[0][wheelSegments/4].z);
        glVertex3f(wheelPoints[1][(wheelSegments/4)*3].x,wheelPoints[1][(wheelSegments/4)*3].y,wheelPoints[0][(wheelSegments/4)*3].z);
        glVertex3f(wheelPoints[0][(wheelSegments/4)*3].x,wheelPoints[0][(wheelSegments/4)*3].y,wheelPoints[0][(wheelSegments/4)*3].z);


    }
    glEnd();

}


void drawWheel(){





    point rightVector=Utils::crossProduct(wheelForwardVector,globalZaxis);
    rightVector.convertToUnit();
    glColor3f(1,0,0);
    for(int i=0;i<=wheelSegments;i++)
    {
        wheelPoints[1][i].x=wheelPoints[0][i].x+rightVector.x*10;
        wheelPoints[1][i].y=wheelPoints[0][i].y+rightVector.y*10;
        wheelPoints[1][i].z=wheelPoints[0][i].z;
    }

    glColor3f(1,1,0);
    glBegin(GL_LINES);
        {
			glVertex3f(wheelCenter.x,wheelCenter.y,wheelCenter.z+wheelRadius);
			glVertex3f(wheelCenter.x+wheelForwardVector.x*100,wheelCenter.y+wheelForwardVector.y*100,wheelCenter.z+wheelRadius);
        }
    glEnd();

    glColor3f(1,1,1);
    glBegin(GL_LINES);
        {
			glVertex3f(wheelCenter.x,wheelCenter.y,wheelCenter.z+wheelRadius);
			glVertex3f(wheelCenter.x+wheelRightVector.x*100,wheelCenter.y+wheelRightVector.y*100,wheelCenter.z+wheelRadius);
        }
    glEnd();



    glTranslated(wheelCenter.x,wheelCenter.y,wheelRadius);
    glRotated(rotate_degree,wheelRightVector.x,wheelRightVector.y,0.0);
    glTranslated((-1)*wheelCenter.x,(-1)*wheelCenter.y,(-1)*wheelRadius);
    for(int j=0;j<wheelSegments;j++)
    {
        if(wheelMove){
            if(wheelPreviousColor)
                glColor3f(0,1,0);
            else
                glColor3f(1,0,0);
        }
        else
        if(j%3==0)
            glColor3f(1,0,0);
        else if(j%3==1)
            glColor3f(0,1,0);
        else
            glColor3f(0,0,1);


        glBegin(GL_QUADS);
        {

            glVertex3f(wheelPoints[0][j].x,wheelPoints[0][j].y,wheelPoints[0][j].z);
            glVertex3f(wheelPoints[1][j].x,wheelPoints[1][j].y,wheelPoints[0][j].z);
            glVertex3f(wheelPoints[1][j+1].x,wheelPoints[1][j+1].y,wheelPoints[0][j+1].z);
            glVertex3f(wheelPoints[0][j+1].x,wheelPoints[0][j+1].y,wheelPoints[0][j+1].z);


        }
        glEnd();
    }
    drawWheelAxis();



}


void keyboardListener(unsigned char key, int x,int y){
        if((char)key=='w'){
            //pace_y=pace_y+.5;
            for(int i=0;i<=wheelSegments;i++)
            {
                wheelPoints[0][i].x+=wheelForwardVector.x*forwardMove;
                wheelPoints[0][i].y+=wheelForwardVector.y*forwardMove;
                //wheelPoints[0][i].z=wheelRadius*sin(((double)i/(double)wheelSegments)*2*pi)+wheelRadius;
            }
            rotate_degree-=(forwardMove/wheelRadius)*180;
            wheelCenter.x+=wheelForwardVector.x*forwardMove;
            wheelCenter.y+=wheelForwardVector.y*forwardMove;

        }
        else if((char)key=='s'){
            for(int i=0;i<=wheelSegments;i++)
            {
                wheelPoints[0][i].x-=wheelForwardVector.x*forwardMove;
                wheelPoints[0][i].y-=wheelForwardVector.y*forwardMove;
                //wheelPoints[0][i].z=wheelRadius*sin(((double)i/(double)wheelSegments)*2*pi)+wheelRadius;
            }
            rotate_degree+=(forwardMove/wheelRadius)*180;
            wheelCenter.x-=wheelForwardVector.x*forwardMove;
            wheelCenter.y-=wheelForwardVector.y*forwardMove;


        }
        else if((char)key=='d'){

            theta=pi/18;
            for(int i=0;i<=wheelSegments;i++)
            {
                wheelPoints[0][i]=Utils::rotatePoint(wheelCenter,(-1)*theta,wheelPoints[0][i]);
                //wheelPoints[0][i].z=wheelRadius*sin(((double)i/(double)wheelSegments)*2*pi)+wheelRadius;
            }


            double cosA=cos(theta);
            double sinA=sin(theta);

            wheelForwardVector.x*=cosA;
            wheelForwardVector.y*=cosA;
            //lookVector.z*=cosA;

            wheelRightVector.x*=sinA;
            wheelRightVector.y*=sinA;

            wheelForwardVector.x+=wheelRightVector.x;
            wheelForwardVector.y+=wheelRightVector.y;
            wheelForwardVector.convertToUnit();

            wheelRightVector=Utils::crossProduct(wheelForwardVector,globalZaxis);
            wheelRightVector.convertToUnit();

            wheelRightVector.print();
            wheelForwardVector.print();
            //wheelRightVector.convertToUnit();


        }
        else if((char)key=='a'){
           theta=pi/18;
            for(int i=0;i<=wheelSegments;i++)
            {
                wheelPoints[0][i]=Utils::rotatePoint(wheelCenter,theta,wheelPoints[0][i]);
            }


            double cosA=cos((-1)*theta);
            double sinA=sin((-1)*theta);

            wheelForwardVector.x*=cosA;
            wheelForwardVector.y*=cosA;
            //lookVector.z*=cosA;

            wheelRightVector.x*=sinA;
            wheelRightVector.y*=sinA;

            wheelForwardVector.x+=wheelRightVector.x;
            wheelForwardVector.y+=wheelRightVector.y;
            wheelForwardVector.convertToUnit();

            wheelRightVector=Utils::crossProduct(wheelForwardVector,globalZaxis);
            wheelRightVector.convertToUnit();

            wheelRightVector.print();
            wheelForwardVector.print();
            //wheelRightVector.convertToUnit();


        }

}




void specialKeyListener(int key, int x,int y){


}




void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(10,10,10,	-100,-100,-100,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		1,0,0);

	point position=camera.getCameraPosition();
	point lookVector=camera.getLookVector();
	point upVector=camera.getUpVector();

	//cout<<cameraConfig[0]->x<<endl;

	gluLookAt(position.x,position.y,position.z,position.x+lookVector.x,position.y+lookVector.y,position.z+lookVector.z,upVector.x,upVector.y,upVector.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    drawWheel();
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	//camera=new CameraOperation();
	///// wheel init
	for(int i=0;i<=wheelSegments;i++)
    {
        wheelPoints[0][i].x=wheelCenter.x;
        wheelPoints[0][i].y=wheelRadius*cos(((double)i/(double)wheelSegments)*2*pi)+wheelCenter.y;
        wheelPoints[0][i].z=wheelRadius*sin(((double)i/(double)wheelSegments)*2*pi)+wheelRadius;
    }
    wheelForwardVector.y=1;
    wheelRightVector.x=1;
    globalZaxis.z=1.0;
	/////

	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;


	/////// cube to sphere
    cubePoint.x=30;
    cubePoint.y=30;
    cubePoint.z=30;
    cubeLength=60;
    cubeTranslate=58;

	/////////////


	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){

	glutInit(&argc,argv);
	glutInitWindowSize(800, 700);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);


	glutMainLoop();		//The main loop of OpenGL*/

	return 0;
}
