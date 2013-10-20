/*
 * Alexander Kort
 * 474 Project 3
 * Particle System - Fish Bubbles
 * Dr. Buackalew
 * 2/28/08
 *
 *------------------------------------------------------------------*/


#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <cstdlib>

// some function prototypes
void display(void);
void sumForces(void);
void EulerIntegrate(double t);
void normalize(float[3]);
void normCrossProd(float[3], float[3], float[3]);

// no provision to move viewpoint in this code,
//   but here are the transforms anyway
// initial viewer position
static GLdouble viewer[] = {0.0, 0.0, 15.0};
// initial viewer angle
static GLfloat theta[] = {0.0, 0.0, 0.0};

static GLUquadric* quad;

// animation variables
static int frame = 0;
static int startFrame = 0;
static int endFrame = 20;
static int increment = 1;

const int NUM_PARTICLES = 17;
const float GRAVITY = 2.8;
const float DRAG = 0.01;
const float TIMESTEP = 0.01; 
const float WIND = 2.8;
const float WIND2 = .5;

float mass[NUM_PARTICLES];
float pos[NUM_PARTICLES][3];
float vel[NUM_PARTICLES][3];
float acc[NUM_PARTICLES][3];
float force[NUM_PARTICLES][3];
float colors[NUM_PARTICLES][3];
float sizes[NUM_PARTICLES];
float maxsizes[NUM_PARTICLES];


float vel2[3];
float acc2[3];
float force2[3];

float size = 2.0;

float initpos = 20.0;

float initfish = -3.0;

static GLdouble translate[3] = {initfish, 0.0, 0.0};

//---------------------------------------------------------
//   Set up the view

void setUpView() {
   // this code initializes the viewing transform
   glLoadIdentity();

   // moves viewer along coordinate axes
   gluLookAt(viewer[0], viewer[1], viewer[2], 
             0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

   // move the view back some relative to viewer[] position
   glTranslatef(0.0f,-3.0f, -3.0f);

   return;
}

void setUpLight() {
   // set up the light sources for the scene
   // a directional light source from over the right shoulder

	quad=gluNewQuadric();
	gluQuadricNormals(quad, GLU_SMOOTH);
	gluQuadricTexture(quad, GL_TRUE);
   glEnable(GL_LIGHTING);


   GLfloat light0_diffuse[] = {1.0, 1.0, 1.0, 1.0};
   GLfloat light0_position[] = {0.0, 5.0, 5.0, 0.0};

   GLfloat light1_diffuse[] = {.5, .5, .5, 1.0};
   GLfloat light1_position[] = {1.0, -9.0, 2.0, 0.0};

   GLfloat light2_diffuse[] = {.2, .2, .2, 1.0};
   GLfloat light2_position[] = {0.0, 2.0, -2.0, 0.0};

   GLfloat ambient[] = {0.0f, .8f, 1.0f, 1.0f};

   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);


   glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
   glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
   glLightfv(GL_LIGHT1, GL_POSITION, light1_position);


   return;
}

//--------------------------------------------------------
//  Set up the objects

void drawBubbles() 
{
   // draw all the particles
   for (int i=0; i<NUM_PARTICLES; i++) {
      // for each particle

      // save the transformation state
      
	glPushMatrix();
      // this translation will be used to animate the particle
      glTranslatef(pos[i][0], pos[i][1], pos[i][2]);	 

		// locate it in the scene
		glMatrixMode(GL_MODELVIEW);
		// this translation will be used to animate the sphere
		glRotatef(90, 1.0, 0.0, 0.0); 

		// draw the sphere - the parameters are radius, number of
		//   radial slices (longitude lines) and number of vertical
		//   slices (latitude lines)
		GLfloat material_ambient[] = {0.0f, 0.0f, 0.0f};
	   GLfloat material_diffuse[] = {.2, 0.6, 1.0, 1.0};
	   GLfloat material_specular[] = {0.20f, 0.140f, 1.00f};
	   GLfloat material_shininess = 25.0f;

	   glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	   glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	   glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	   glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);

		glutSolidSphere(sizes[i], 30, 30); 

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_BLEND);

     // recover the transform state
      glPopMatrix();
   }
   return;
}

void drawFish()
{
	quad=gluNewQuadric();
	gluQuadricNormals(quad, GLU_SMOOTH);
	gluQuadricTexture(quad, GL_TRUE);
	glDisable(GL_BLEND);


	//body
	glPushMatrix();

   // set the material
   GLfloat material_diffuse[] = {.80, .80, .80};

   glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	
	glTranslatef(translate[0], translate[1], translate[2]);


	glScalef(3.0, 1, 1);
	glutSolidSphere(1.0, 30, 30); 
	// recover the transform state
   glPopMatrix();


	//eye
	glPushMatrix();

   GLfloat material_ambient1[] = {.80, .80, .80};
	GLfloat material_diffuse1[] = {.80, .80, .80};
	GLfloat material_specular1[] = {.80, .80, .80};
	GLfloat material_shininess1 = 25.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular1);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess1);
	
	//quad, base radius, top radius, height, slices, stacks
	glTranslatef(translate[0] + 2, translate[1] + .2, translate[2]+.8);
	gluDisk(quad, 0, .3, 20, 20);
	glPopMatrix();

	glPushMatrix();

	GLfloat material_diffuse2[] = {0, 0, 0};

	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse2);

	
	//quad, base radius, top radius, height, slices, stacks
	glTranslatef(translate[0] + 2, translate[1] + .2, translate[2]+.9);
	gluDisk(quad, 0, .15, 10, 10);
	glPopMatrix();


	//fins
	glPushMatrix();
	GLfloat material_diffuse3[] = {.4, .4, .4};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse3);	
	//quad, base radius, top radius, height, slices, stacks
	glTranslatef(translate[0]-3.0, translate[1]+.1, translate[2]+.3);
	gluPartialDisk(quad, 0, 1.3, 20, 20, 0, 180);
	glPopMatrix();

	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse3);
	//quad, base radius, top radius, height, slices, stacks
	glTranslatef(translate[0]-.8, translate[1]+.4, translate[2]+.25);
	gluPartialDisk(quad, 0, 1.3, 20, 20, 0, 90);
	glPopMatrix();

	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse3);
	//quad, base radius, top radius, height, slices, stacks
	glTranslatef(translate[0]-.1, translate[1]-.3, translate[2]+.9);
	gluPartialDisk(quad, 0, 1, 20, 20, 90, 100);
	glPopMatrix();



	glEnable(GL_BLEND);
   return;
}

//-----------------------------------------------------------
//  Callback functions

void reshapeCallback(int w, int h) {
   // from Angel, p.562

   glViewport(0,0,w,h);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w < h) {
      glFrustum(-2.0, 2.0, -2.0*(GLfloat) h / (GLfloat) w,
                2.0*(GLfloat) h / (GLfloat) w, 2.0, 20.0);
   }
   else {
      glFrustum(-2.0, 2.0, -2.0*(GLfloat) w / (GLfloat) h,
                2.0*(GLfloat) w / (GLfloat) h, 2.0, 20.0);
   }

   glMatrixMode(GL_MODELVIEW);
}


void timeStep(int step) {
   // animation code goes here
   // This function is called for each frame of animation
   double t = (double) (frame - startFrame) / (endFrame - startFrame);
   for (int i=0; i<NUM_PARTICLES; i++) 
	{
	   if (pos[i][1] <= -20 || pos[i][1] >= 16.5 || pos[i][0] <= -4 || pos[i][0] >= 5)
	   {
			  mass[i] = 1.0;
		      sizes[i] = (((float)rand()/RAND_MAX))/3;
			  maxsizes[i] = sizes[i] + .3;

			  pos[i][0] = 0.0;
			  pos[i][1] = 0.0;
			  pos[i][2] = 0.0;
		   
			  
			  vel[i][2] = 0.0;

			  if (sizes[i] > .2)
			  {
				vel[i][0] = ((((float)rand()/RAND_MAX)*2)-1)*3 + 3;
				vel[i][1] = ((((float)rand()/RAND_MAX)*2))*15 + 6;
			  }
			  else if (sizes[i] > .1)
			  {
				vel[i][0] = ((((float)rand()/RAND_MAX)*2)-1)*2 + 2;
				vel[i][1] = ((((float)rand()/RAND_MAX)*2))*15 + 4;
			  }
			  else
			  {
				vel[i][0] = ((((float)rand()/RAND_MAX)*2)-1)*1;
				vel[i][1] = ((((float)rand()/RAND_MAX)*2))*5 + 2;
			  }
		      
			  acc[i][0] = 0.0;
			  acc[i][1] = 0.0;
			  acc[i][2] = 0.0;
		      
			  force[i][0] = 0.0;
			  force[i][1] = 0.0;
			  force[i][2] = 0.0;

			  colors[i][0] = 0.0;
			  colors[i][1] = (rand()%100)*.01 - .5;
			  colors[i][2] = (rand()%100)*.01 + .5;
	   }
   }
   for (int j=0; j<NUM_PARTICLES; j++) 
   {
	   if (sizes[j] <= maxsizes[j])
		{
			sizes[j] += .02;
		}
   }
   // do the particle system stuff
   sumForces();
   EulerIntegrate(t);
   if (frame == endFrame) increment = -1;
   else if (frame == startFrame) increment = 1;
    
   frame = frame + increment;

   display();

   glutTimerFunc(10,timeStep, 0);

}

//---------------------------------------------------------
//  Particle system routines

void initializeParticleDataStructure() 
{
   // initial values for all the particles

   for (int i=0; i<NUM_PARTICLES; i++) 
	{
      mass[i] = 1.0;
      sizes[i] = (((float)rand()/RAND_MAX))/3;
	  maxsizes[i] = sizes[i] + .3;

      pos[i][0] = 0.0;
      pos[i][1] = 0.0;
      pos[i][2] = 0.0;

	  if (sizes[i] > .2)
	  {

		vel[i][0] = ((((float)rand()/RAND_MAX)*2)-1)*3 + 3;

		vel[i][1] = ((((float)rand()/RAND_MAX)*2))*15 + 6;
	  }
	  else if (sizes[i] > .1)
	  {
		vel[i][0] = ((((float)rand()/RAND_MAX)*2)-1)*2 + 2;

		vel[i][1] = ((((float)rand()/RAND_MAX)*2))*15 + 4;
	  }
	  else
	  {
		vel[i][0] = ((((float)rand()/RAND_MAX)*2)-1)*1;

		vel[i][1] = ((((float)rand()/RAND_MAX)*2))*5 + 2;
	  }
      acc[i][0] = 0.0;
      acc[i][1] = 0.0;
      acc[i][2] = 0.0;
      
      force[i][0] = 0.0;
      force[i][1] = 0.0;
      force[i][2] = 0.0;

	  colors[i][0] = 0.0;
	  colors[i][1] = (rand()%20 + 50)*.01;
	  colors[i][2] = (rand()%20 + 50)*.01;
   }
}


void sumForces() 
{
   // for this timestep, accumulate all the forces that
   //   act on each particle

   for (int i=0; i<NUM_PARTICLES; i++) 
	{

      // ZERO ALL FORCES
      force[i][0] = force[i][1] = force[i][2] = 0.0;

	   // GRAVITY AND WIND AND DRAG
      force[i][1] = -GRAVITY*mass[i] + WIND + (-DRAG*pow(vel[i][1], 2));

	  force[i][0] = -WIND2;

		
   }
}
  
void EulerIntegrate(double t) {
   // for each particle, compute the new velocity
   //   and position

   for (int i = 0; i < NUM_PARTICLES; i++) {
  
      // CALCULATE NEW ACCEL
      acc[i][0] = force[i][0] / mass[i];
      acc[i][1] = force[i][1] / mass[i];
      acc[i][2] = force[i][2] / mass[i];

		acc2[1] = force2[1] / mass[i];

      // CALCULATE NEW POS
      pos[i][0] += vel[i][0] * TIMESTEP +
                        0.5 * acc[i][0] * TIMESTEP * TIMESTEP;
      pos[i][1] += vel[i][1] * TIMESTEP +
                        0.5 * acc[i][1] * TIMESTEP * TIMESTEP;
      pos[i][2] += vel[i][2] * TIMESTEP +
                        0.5 * acc[i][2] * TIMESTEP * TIMESTEP;
		
   
      // CALCULATE NEW VEL
      vel[i][0] += acc[i][0] * TIMESTEP;
      vel[i][1] += acc[i][1] * TIMESTEP;
      vel[i][2] += acc[i][2] * TIMESTEP;
      
   }
}
  
//---------------------------------------------------------
//  Main routines

void display (void) {
   // this code executes whenever the window is redrawn (when opened,
   //   moved, resized, etc.
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   // set the viewing transform
   setUpView();

	setUpLight();

   // start drawing objects
	glEnable(GL_DEPTH_TEST);
	drawFish();
	glDisable(GL_DEPTH_TEST);
   drawBubbles();

   glutSwapBuffers();
}

// create a double buffered 500x500 pixel color window
int main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Particle System: Lab 5");
	glClearColor(.3, .6, .9, .2);

	glEnable(GL_DEPTH_TEST);
	
   initializeParticleDataStructure();

	glutDisplayFunc(display);
   glutReshapeFunc(reshapeCallback);
	glutTimerFunc(50,timeStep, 0);  // 50 millisecond callback
	glutMainLoop();
	return 0;
}

//---------------------------------------------------------
//  Utility functions

void normalize(float v[3]) {
   // normalize v[] and return the result in v[]
   // from OpenGL Programming Guide, p. 58
   GLfloat d = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
   if (d == 0.0) {
      printf("zero length vector");
      return;
   }
   v[0] = v[0]/d; v[1] = v[1]/d; v[2] = v[2]/d;
}

void normCrossProd(float v1[3], float v2[3], float out[3]) {
   // cross v1[] and v2[] and return the result in out[]
   // from OpenGL Programming Guide, p. 58
   out[0] = v1[1]*v2[2] - v1[2]*v2[1];
   out[1] = v1[2]*v2[0] - v1[0]*v2[2];
   out[2] = v1[0]*v2[1] - v1[1]*v2[0];
   normalize(out);
}

