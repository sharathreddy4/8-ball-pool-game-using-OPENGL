
// quadric object for 3D primitives
GLUquadricObj *qobj;

// lamp light source position
GLfloat lightPosition[4] = {0.0f, 2.5f, 0.0f, 1.0f};

// game state
double horizontalCameraAngle = -90;
double verticalCameraAngle = 30;
double cueStickAngle = 0;
int mouseX = 0;

// change of current material color
void setMaterialColor(double red, double green, double blue)
{
	// set ambient and diffuse material colors
	float color[4];
	color[0] = (float)red;
	color[1] = (float)green;
	color[2] = (float)blue;
	color[3] = 1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
}

// change of current material shininess
void setMaterialShininess(double s)
{
	// set specular material color
	float color[4];
	color[0] = (float)s;
	color[1] = (float)s;
	color[2] = (float)s;
	color[3] = 1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);

	// set specular material shininess
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);
}
