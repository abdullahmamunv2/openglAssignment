#include<iostream>
#include<stdlib.h>
#include<math.h>

//#include <windows.h>
#include <GL/glut.h>

#include<vector>


using namespace std;

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;



//////torus param

double innerRadius;
double outerRadius;
double yAxisTranslate;


class point
{
    public:
        double x,y,z;
        void print(){
            cout<<"X : "<<x<<endl<<"Y: "<<y<<endl<<"Z: "<<z<<endl;
        }
        void convertToUnit(){
            double maxValue=max(max(abs(x),abs(y)),abs(z));
            x/=maxValue;
            y/=maxValue;
            z/=maxValue;
        }
};


class Utils{

    public:
        static point getPerpendicularVector(point v,bool positive=true){

            if(positive){
                point p=crossProduct(v,getRightLeftVector(v));
                p.convertToUnit();
                return p;
            }
            else{
                point p=crossProduct(getRightLeftVector(v),v);
                p.convertToUnit();
                p.print();
                return p;
            }


        }
        static point getRightLeftVector(point v,bool right=true){
            point globalUpVector;
            globalUpVector.z=1;
            if(!right){
                globalUpVector.z=-1;
            }
            point p=crossProduct(v,globalUpVector);
            p.convertToUnit();
            p.print();
            return p;
        }
        static point crossProduct(point u,point v){
            point p;
            p.x=u.y*v.z-v.y*u.z;
            p.y=v.x*u.z-u.x*v.z;
            p.z=u.x*v.y-v.x*u.y;

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

            lookVector.x=(0.0-cameraPosition.x)/cameraPosition.x;
            lookVector.y=(0.0-cameraPosition.y)/cameraPosition.y;
            lookVector.z=(0.0-cameraPosition.z)/cameraPosition.z;

            upVector.x=0.0;
            upVector.y=0.0;
            upVector.z=1.0;

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
            return this->upVector;
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


void drawCircle(double radius,int segments)
{
    int i;
    point points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(0,points[i].x+70,points[i].y);
			glVertex3f(0,points[i+1].x+70,points[i+1].y);
        }
        glEnd();
    }


}

void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawToras(double radius,int segments){

    point points [30][100];
    glColor3f(0.7,0.7,0.7);
    int circleNum=30;
    //generate points
    for(int i=0;i<=segments;i++)
    {
        points[0][i].x=0;
        points[0][i].y=((outerRadius-innerRadius)/2)*cos(((double)i/(double)segments)*2*pi)+yAxisTranslate;
        points[0][i].z=((outerRadius-innerRadius)/2)*sin(((double)i/(double)segments)*2*pi);
    }

    //point point2[100];
    for(int i=1;i<circleNum;i++){
        for(int j=0;j<=segments;j++)
        {

            double newX = (points[0][j].x)*cos(((double)i/(double)segments)*2*pi) - (points[0][j].y)*sin(((double)i/(double)segments)*2*pi);

            double newY = (points[0][j].x)*sin(((double)i/(double)segments)*2*pi) + (points[0][j].y)*cos(((double)i/(double)segments)*2*pi);

            points[i][j].x=newX;
            points[i][j].y=newY;
        }

    }


    for(int i=0;i<circleNum-1;i++){
        for(int j=0;j<segments;j++)
        {
            if(j%2==0)
                glColor3f(1,0,0);
            else
                glColor3f(0,1,0);

            glBegin(GL_QUADS);
            {
                glVertex3f(points[i][j].x,points[i][j].y,points[0][j].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,points[0][j].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[0][j+1].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,points[0][j+1].z);


            }
            glEnd();
        }
    }






}


void drawSphere(double radius,int slices,int stacks)
{
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
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
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
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
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
			points[i][j].x=r*cos(((double)j/(double)slices)*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*pi);
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
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}


void drawSS()
{
    glColor3f(1,0,0);
    drawSquare(20);

    glRotatef(angle,0,0,1);
    glTranslatef(110,0,0);
    glRotatef(2*angle,0,0,1);
    glColor3f(0,1,0);
    drawSquare(15);

    glPushMatrix();
    {
        glRotatef(angle,0,0,1);
        glTranslatef(60,0,0);
        glRotatef(2*angle,0,0,1);
        glColor3f(0,0,1);
        drawSquare(10);
    }
    glPopMatrix();

    glRotatef(3*angle,0,0,1);
    glTranslatef(40,0,0);
    glRotatef(4*angle,0,0,1);
    glColor3f(1,1,0);
    drawSquare(5);
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			drawgrid=1-drawgrid;
			break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){

    cout<<"press"<<endl;
    if(key==GLUT_KEY_DOWN){
        camera.updateCameraPosition(camera.getLookVector(),-5);
    }
    else if(key==GLUT_KEY_UP){

        camera.updateCameraPosition(camera.getLookVector(),5);

    }
    else if(key==GLUT_KEY_RIGHT){

        point temp1=Utils::getRightLeftVector(camera.getLookVector(),false);

        camera.updateCameraPosition(temp1,5);

    }
    else if(key==GLUT_KEY_LEFT){

        point temp1=Utils::getRightLeftVector(camera.getLookVector());

        camera.updateCameraPosition(temp1,5);

    }
    else if(key==GLUT_KEY_PAGE_UP){


        point temp1=Utils::getPerpendicularVector(camera.getLookVector());

        camera.updateCameraPosition(temp1,5);

    }
    else if(key==GLUT_KEY_PAGE_DOWN){
        point temp1=Utils::getPerpendicularVector(camera.getLookVector());

        camera.updateCameraPosition(temp1,-5);
    }
    else if(key==GLUT_KEY_F1){
        cout<<"f1"<<endl;
        outerRadius+=5;
    }
    else if(key==GLUT_KEY_F2){

        outerRadius-=5;
    }
    else if(key==GLUT_KEY_F3){
        cout<<"f1"<<endl;
        innerRadius+=5;
    }
    else if(key==GLUT_KEY_F4){

        innerRadius-=5;
    }

    //cout<<"click"<<endl;
	/*switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
            point p=camera.getLookVector();
            p.x-=5;
            p.y-=5;
            p.z-=5;
            camera.setLookVector(p);
			cameraHeight -= 3.0;
			break;
		case GLUT_KEY_UP:		// up arrow key
            point p=camera.getLookVector();
            p.x+=5;
            p.y+=5;
            p.z+=5;
            camera.setLookVector(p);
			cameraHeight += 3.0;
			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}*/
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
	//drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    //drawSS();

    //drawCircle(30,24);
    drawToras(30,24);
    //drawOneEighthOfSphare(30,24,20);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);




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
	//camera=new CameraOperation();
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;
	innerRadius=70;
	outerRadius=200;
	yAxisTranslate=innerRadius+((outerRadius-innerRadius)/2);


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
	glutInitWindowSize(500, 500);
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
