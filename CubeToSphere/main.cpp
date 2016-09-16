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



class point
{
    public:
        double x,y,z;
        void print(){
            //cout<<"X : "<<x<<endl<<"Y: "<<y<<endl<<"Z: "<<z<<endl;
        }
        void convertToUnit(){
            double vectorValue=floor((sqrt(x*x+y*y+z*z)*100))/100;
            x/=vectorValue;
            y/=vectorValue;
            z/=vectorValue;
        }
        void scale(double value){
            x*=value;
            y*=value;
            z*=value;
        }
};


/////camera

point globalZaxis;



////// wheel ////////////////

point wheelCenter;
point wheelForwardVector;
point wheelRightVector;
double theta=0.0;
point wheelPoints[2][100];
double wheelRadius=30;

int wheelSegments=20;
bool wheelMove=false;
bool wheelPreviousColor=true;  //// here true means red  false means green


///// cube to sphere

double cubeLength;
point cubePoint;
double cubeTranslate;
double d=0;



class Utils{

    public:
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
            cameraPosition.x=200.0;
            cameraPosition.y=200.0;
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
	if(drawgrid==1)
	{
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
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,2);
		glVertex3f( a,-a,2);
		glVertex3f(-a,-a,2);
		glVertex3f(-a, a,2);
	}glEnd();
}




void drawOneEighthOfSphare(double radius,int slices,int stacks){
    struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*pi/2);
			points[i][j].y=r*sin(((double)j/(double)slices)*pi/2);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                /*glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);*/
			}glEnd();
		}
	}
}
void drawOneFourthOfCylinder(double radius,double height,int segments){
    int i;
    double shade;
    struct point cylinderPoints[2][100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        cylinderPoints[0][i].y=radius*cos(((double)i/(double)segments)*pi/2);
        cylinderPoints[0][i].z=radius*sin(((double)i/(double)segments)*pi/2);
        cylinderPoints[0][i].x=0.0;
    }
    //draw triangles using generated points
    for(int j=0;j<segments;j++)
    {
        //create shading effect
        if(j<segments/2)shade=2*(double)j/(double)segments;
        else shade=2*(1.0-(double)j/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_QUADS);
        {
            glVertex3f(cylinderPoints[0][j].x-height,cylinderPoints[0][j].y,cylinderPoints[0][j].z);
            glVertex3f(cylinderPoints[0][j].x+height,cylinderPoints[0][j].y,cylinderPoints[0][j].z);
            glVertex3f(cylinderPoints[0][j+1].x+height,cylinderPoints[0][j+1].y,cylinderPoints[0][j+1].z);
            glVertex3f(cylinderPoints[0][j+1].x-height,cylinderPoints[0][j+1].y,cylinderPoints[0][j+1].z);


        }
        glEnd();
    }

}



void DrawSquare(){
    double angle=90;

    //glRotatef(angle,1,0,0);
    glColor3f(1,0,0);

    glPushMatrix();
    {
        glTranslated(0,0,cubeTranslate);

    }
    drawSquare(cubeLength);
    glPopMatrix();

    glPushMatrix();
    {
        glRotatef(2*angle,1,0,0);
        glTranslated(0,0,cubeTranslate);

    }
    drawSquare(cubeLength);
    glPopMatrix();
    glColor3f(0,1,0);

    glPushMatrix();
    {
        //glTranslated(0,0,cubeLength);
        glRotatef(angle,1,0,0);
        glTranslated(0,0,cubeTranslate);

    }
    drawSquare(cubeLength);
    glPopMatrix();



    glPushMatrix();
    {
        //glTranslated(0,0,cubeLength);
        glRotatef((-1)*angle,1,0,0);
        glTranslated(0,0,cubeTranslate);
    }
    drawSquare(cubeLength);
    glPopMatrix();


    //////
    glColor3f(0,0,1);
    glPushMatrix();
    {
        //glTranslated(0,0,cubeLength);
        glRotatef(angle,0,1,0);
        glTranslated(0,0,cubeTranslate);

    }
    drawSquare(cubeLength);
    glPopMatrix();

    glPushMatrix();
    {
        //glTranslated(0,0,cubeLength);
        glRotatef((-1)*angle,0,1,0);
        glTranslated(0,0,cubeTranslate);
    }
    drawSquare(cubeLength);
    glPopMatrix();
}


void DrawCylinder(){

    double radius=d;
    double cLength=2*cubeLength;
    double angle=90;


    //glRotated(45,1,0,0);
    //****** blue cylinder ***************************
    //glColor3f(0,0,1);


    glPushMatrix();
    {
        glTranslated(0,cubeLength,cubeLength);

    }
    drawOneFourthOfCylinder(radius,cLength/2,24);
    glPopMatrix();

    glPushMatrix();
    {

        glTranslated(0,(-1)*cubeLength,cubeLength);
        glRotated(angle,1,0,0);

    }
    drawOneFourthOfCylinder(radius,cLength/2,24);
    glPopMatrix();


    glPushMatrix();
    {

        glTranslated(0,(-1)*cubeLength,(-1)*cubeLength);
        glRotated(2*angle,1,0,0);

    }
    drawOneFourthOfCylinder(radius,cLength/2,24);
    glPopMatrix();

    glPushMatrix();
    {

        glTranslated(0,cubeLength,(-1)*cubeLength);
        glRotated(3*angle,1,0,0);

    }
    drawOneFourthOfCylinder(radius,cLength/2,24);
    glPopMatrix();

    //*****************************************



    //******************************************

    glPushMatrix();
    {

        glTranslated((-1)*cubeLength,0,cubeLength);
        glRotated(angle,0,0,1);

    }
    drawOneFourthOfCylinder(radius,cLength/2,24);
    glPopMatrix();

    glPushMatrix();
    {

        glTranslated((-1)*cubeLength,0,(-1)*cubeLength);

        glRotated(3*angle,0,1,0);
        glRotated(angle,0,0,1);

    }
    drawOneFourthOfCylinder(radius,cLength/2,24);
    glPopMatrix();

    glPushMatrix();
    {

        glTranslated(cubeLength,0,cubeLength);

        glRotated(angle,0,1,0);
        glRotated(angle,0,0,1);

    }
    drawOneFourthOfCylinder(radius,cLength/2,24);
    glPopMatrix();

    glPushMatrix();
    {

        glTranslated(cubeLength,0,(-1)*cubeLength);

        glRotated(2*angle,0,1,0);
        glRotated(angle,0,0,1);

    }
    drawOneFourthOfCylinder(radius,cLength/2,24);
    glPopMatrix();



    //*****************************************


    glPushMatrix();
    {

        glTranslated(cubeLength,cubeLength,0);


        glRotated((-1)*angle/2,0,0,1);
        glRotated(angle,0,1,0);
        glRotated((-1)*angle/2,1,0,0);

    }
    drawOneFourthOfCylinder(radius,cLength/2,24);
    glPopMatrix();


    glPushMatrix();
    {

        glTranslated((-1)*cubeLength,cubeLength,0);


        glRotated(angle/2,0,0,1);
        glRotated(angle,0,1,0);
        glRotated((-1)*angle/2,1,0,0);

    }
    drawOneFourthOfCylinder(radius,cLength/2,24);
    glPopMatrix();

    glPushMatrix();
    {

        glTranslated((-1)*cubeLength,(-1)*cubeLength,0);


        glRotated(3*angle/2,0,0,1);
        glRotated(angle,0,1,0);
        glRotated((-1)*angle/2,1,0,0);

    }
    drawOneFourthOfCylinder(radius,cLength/2,24);
    glPopMatrix();



    glPushMatrix();
    {

        glTranslated(cubeLength,(-1)*cubeLength,0);

        glRotated(5*angle/2,0,0,1);
        glRotated(angle,0,1,0);
        glRotated((-1)*angle/2,1,0,0);

    }
    drawOneFourthOfCylinder(radius,cLength/2,24);
    glPopMatrix();



}

void DrawSphere(){

    double radius=d;
    double angle=90;

    glPushMatrix();
    {

        glTranslated(cubeLength,cubeLength,cubeLength);

    }
    drawOneEighthOfSphare(radius,30,30);
    glPopMatrix();

    glPushMatrix();
    {

        glTranslated(cubeLength,(-1)*cubeLength,cubeLength);
        glRotated((-1)*angle,0,0,1);

    }
    drawOneEighthOfSphare(radius,30,30);
    glPopMatrix();

    glPushMatrix();
    {

        glTranslated((-1)*cubeLength,cubeLength,cubeLength);
        glRotated(angle,0,0,1);

    }
    drawOneEighthOfSphare(radius,30,30);
    glPopMatrix();

    glPushMatrix();
    {

        glTranslated((-1)*cubeLength,(-1)*cubeLength,cubeLength);
        glRotated(2*angle,0,0,1);

    }
    drawOneEighthOfSphare(radius,30,30);
    glPopMatrix();

    //**************************************************



    glPushMatrix();
    {

        glTranslated(cubeLength,cubeLength,(-1)*cubeLength);
        glRotated(2*angle,1,1,0);

    }
    drawOneEighthOfSphare(radius,30,30);
    glPopMatrix();

    glPushMatrix();
    {

        glTranslated(cubeLength,(-1)*cubeLength,(-1)*cubeLength);
        glRotated((-1)*angle,0,0,1);
        glRotated(2*angle,1,1,0);

    }
    drawOneEighthOfSphare(radius,30,30);
    glPopMatrix();

    glPushMatrix();
    {

        glTranslated((-1)*cubeLength,cubeLength,(-1)*cubeLength);
        glRotated(angle,0,0,1);
        glRotated(2*angle,1,1,0);

    }
    drawOneEighthOfSphare(radius,30,30);
    glPopMatrix();

    glPushMatrix();
    {

        glTranslated((-1)*cubeLength,(-1)*cubeLength,(-1)*cubeLength);
        glRotated(2*angle,0,0,1);
        glRotated(2*angle,1,1,0);

    }
    drawOneEighthOfSphare(radius,30,30);
    glPopMatrix();


}


void drawCubeToSphere(){

    DrawSquare();
    DrawCylinder();
    DrawSphere();


}




void keyboardListener(unsigned char key, int x,int y){
        if((char)key=='1'){
        double upDownRotate=pi/18;
        point lookVector=camera.getLookVector();
        point rightVector=camera.getRightVector();


        double cosA=cos(upDownRotate);
        double sinA=sin(upDownRotate);

        lookVector.x*=cosA;
        lookVector.y*=cosA;

        rightVector.x*=sinA;
        rightVector.y*=sinA;

        lookVector.x+=rightVector.x;
        lookVector.y+=rightVector.y;

        camera.setRightVector(Utils::getRightLeftVector(lookVector,camera.getUpVector()));
        camera.setLookVector(lookVector);

    }
    else if((char)key=='2'){
        double upDownRotate=pi/18;
        point lookVector=camera.getLookVector();
        point leftVector=Utils::getRightLeftVector(lookVector,globalZaxis,false);


        double cosA=cos(upDownRotate);
        double sinA=sin(upDownRotate);

        lookVector.x*=cosA;
        lookVector.y*=cosA;

        leftVector.x*=sinA;
        leftVector.y*=sinA;

        lookVector.x+=leftVector.x;
        lookVector.y+=leftVector.y;

        camera.setRightVector(Utils::getRightLeftVector(lookVector,camera.getUpVector()));
        camera.setLookVector(lookVector);

    }
    else if((char)key=='3'){
        double upDownRotate=pi/18;
        point lookVector=camera.getLookVector();
        point upVector=camera.getUpVector();


        double cosA=cos(upDownRotate);
        double sinA=sin(upDownRotate);

        lookVector.z*=cosA;

        upVector.z*=sinA;

        lookVector.z+=upVector.z;

        camera.setUpVector(Utils::getPerpendicularVector(lookVector,camera.getRightVector()));
        camera.setLookVector(lookVector);

    }
    else if((char)key=='4'){
        double upDownRotate=pi/18;
        point lookVector=camera.getLookVector();
        point downVector=Utils::getPerpendicularVector(lookVector,camera.getRightVector(),false);


        double cosA=cos(upDownRotate);
        double sinA=sin(upDownRotate);

        lookVector.z*=cosA;

        downVector.z*=sinA;

        lookVector.z+=downVector.z;

        camera.setUpVector(Utils::getPerpendicularVector(lookVector,camera.getRightVector()));
        camera.setLookVector(lookVector);

    }

    else if((char)key=='5'){
        double tiltRotate=pi/18;
        point rightVector=camera.getRightVector();
        point upVector=camera.getUpVector();


        double cosA=cos(tiltRotate);
        double sinA=sin(tiltRotate);

        upVector.x*=cosA;
        upVector.z*=cosA;

        rightVector.x*=sinA;
        rightVector.z*=sinA;

        upVector.x+=rightVector.x;
        upVector.z+=rightVector.z;

        camera.setRightVector(Utils::getRightLeftVector(camera.getLookVector(),upVector));
        camera.setUpVector(upVector);

    }

    else if((char)key=='6'){
        double tiltRotate=pi/18;
        point upVector=camera.getUpVector();
        point leftVector=Utils::getRightLeftVector(camera.getLookVector(),upVector,false);


        double cosA=cos(tiltRotate);
        double sinA=sin(tiltRotate);

        upVector.x*=cosA;
        upVector.z*=cosA;

        leftVector.x*=sinA;
        leftVector.z*=sinA;

        upVector.x+=leftVector.x;
        upVector.z+=leftVector.z;

        camera.setRightVector(Utils::getRightLeftVector(camera.getLookVector(),upVector));
        camera.setUpVector(upVector);

    }
}


void specialKeyListener(int key, int x,int y){


    if(key==GLUT_KEY_DOWN){
        camera.updateCameraPosition(camera.getLookVector(),-5);
    }
    else if(key==GLUT_KEY_UP){

        camera.updateCameraPosition(camera.getLookVector(),5);

    }
    else if(key==GLUT_KEY_RIGHT){

        camera.updateCameraPosition(camera.getRightVector(),5);

    }
    else if(key==GLUT_KEY_LEFT){

        camera.updateCameraPosition(camera.getRightVector(),-5);

    }
    else if(key==GLUT_KEY_PAGE_UP){

        camera.updateCameraPosition(camera.getUpVector(),5);

    }
    else if(key==GLUT_KEY_PAGE_DOWN){

        camera.updateCameraPosition(camera.getUpVector(),-5);
    }

	else if(key==GLUT_KEY_HOME){
        if(cubeLength>0){
            cubeLength-=5;
            d+=5;
        }

	}
	else if(key==GLUT_KEY_END){
        if(cubeLength<58){
            cubeLength+=5;
            d-=5;
        }
	}
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
	//lookVector.scale(200.0);

	//cout<<"look "<<lookVector.x<<endl;

	gluLookAt(position.x,position.y,position.z,position.x+lookVector.x,position.y+lookVector.y,position.z+lookVector.z,upVector.x,upVector.y,upVector.z);
	//gluLookAt(position.x,position.y,position.z,position.x,position.y,position.z,upVector.x,upVector.y,upVector.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
    drawCubeToSphere();




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization


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

	glutCreateWindow("Cube to sphere");

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
