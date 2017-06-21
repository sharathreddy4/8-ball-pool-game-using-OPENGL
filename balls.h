
double ballRadius = 0.12;

class Balls
{
	double px[16], pz[16]; // ball positions
	double vx[16], vz[16]; // ball velocities
	bool pocketed[16];     // ball states
	GLuint textures[16];   // ball textures

public:
	// constructor
	Balls()
	{
		reset();
	}

	// ball texture creation
	void createTextures()
	{
		// create textures for all balls
		textures[0] = createTexture("ball00.bmp");
		textures[1] = createTexture("ball01.bmp");
		textures[2] = createTexture("ball02.bmp");
		textures[3] = createTexture("ball03.bmp");
		textures[4] = createTexture("ball04.bmp");
		textures[5] = createTexture("ball05.bmp");
		textures[6] = createTexture("ball06.bmp");
		textures[7] = createTexture("ball07.bmp");
		textures[8] = createTexture("ball08.bmp");
		textures[9] = createTexture("ball09.bmp");
		textures[10] = createTexture("ball10.bmp");
		textures[11] = createTexture("ball11.bmp");
		textures[12] = createTexture("ball12.bmp");
		textures[13] = createTexture("ball13.bmp");
		textures[14] = createTexture("ball14.bmp");
		textures[15] = createTexture("ball15.bmp");
	}

	// resetting to initial position
	void reset()
	{
		// initial position for cue ball
		px[0] = 2;
		pz[0] = 0;
		vx[0] = 0;
		vz[0] = 0;
		pocketed[0] = false;

		// initial position for other balls
		int k = 1;
		for(int i = 1; i <= 5; i++)
		{
			for(int j = 1; j <= i; j++)
			{
				// at start balls are placed in triangular rack
				px[k] = -2 + (3 - i) * sqrt(3.0) * ballRadius;
				pz[k] = (j * 2 - i - 1) * ballRadius;
				vx[k] = 0;
				vz[k] = 0;
				pocketed[k] = false;
				k++;
			}
		}
	}

	// current cue ball position
	double cueBallX()
	{
		return px[0];
	}

	// current cue ball position
	double cueBallY()
	{
		return ballRadius;
	}

	// current cue ball position
	double cueBallZ()
	{
		return pz[0];
	}

	// current number of pocketed balls
	int pocketedNumber()
	{
		int number = 0;
		for(int i = 0; i < 16; i++)
		{
			if(pocketed[i]) number++;
		}
		return number;
	}

	// current moving state for all balls
	bool stopped()
	{
		for(int i = 0; i < 16; i++)
		{
			// ignore pocketed balls
			if(pocketed[i]) continue;

			// detect moving balls
			if(vx[i] != 0) return false;
			if(vz[i] != 0) return false;
		}

		// all balls have stopped
		return true;
	}

	// shooting the cue ball
	void shoot()
	{
		const double v = 20; // shooting velocity
		double a = cueStickAngle / 57.2958; // convert degrees to radians

		// set cue ball velocity along cue stick
		vx[0] = -v * sin(a);
		vz[0] = -v * cos(a);
	}

	// ball state update
	void update(double timeStep)
	{
		int i, j;

		// ball-pocket collisions
		for(i = 0; i < 16; i++)
		{
			// check distances to 6 pockets
			double d = 0.3;
			if((px[i] - 4) * (px[i] - 4) + (pz[i] - 2) * (pz[i] - 2) < d * d) pocketed[i] = true;
			if((px[i] - 4) * (px[i] - 4) + (pz[i] + 2) * (pz[i] + 2) < d * d) pocketed[i] = true;
			if((px[i] + 4) * (px[i] + 4) + (pz[i] - 2) * (pz[i] - 2) < d * d) pocketed[i] = true;
			if((px[i] + 4) * (px[i] + 4) + (pz[i] + 2) * (pz[i] + 2) < d * d) pocketed[i] = true;
			if((px[i] - 0) * (px[i] - 0) + (pz[i] - 2) * (pz[i] - 2) < d * d) pocketed[i] = true;
			if((px[i] - 0) * (px[i] - 0) + (pz[i] + 2) * (pz[i] + 2) < d * d) pocketed[i] = true;
		}

		// ball-ball collisions
		for(i = 0; i < 16; i++)
		for(j = 0; j < i; j++)
		{
			// ignore pocketed balls
			if(pocketed[i]) continue;
			if(pocketed[j]) continue;

			// relative vector between the balls
			double dx = px[j] - px[i];
			double dz = pz[j] - pz[i];

			// no collision for balls too far from each other
			double distance = sqrt(dx * dx + dz * dz);
			if(distance > ballRadius * 2) continue;
			if(distance == 0) continue;

			// unit vector between the balls
			double ux = dx / distance;
			double uz = dz / distance;

			// normal velocity components
			double ni = vx[i] * ux + vz[i] * uz;
			double nj = vx[j] * ux + vz[j] * uz;

			// ignore balls moving away from each other
			if(ni < nj) continue;

			// swap normal velocity components
			vx[i] += nj * ux - ni * ux;
			vz[i] += nj * uz - ni * uz;
			vx[j] += ni * ux - nj * ux;
			vz[j] += ni * uz - nj * uz;
		}

		// ball-cushion collisions
		for(i = 0; i < 16; i++)
		{
			// ignore pocketed balls
			if(pocketed[i]) continue;

			// flip normal velocity components
			if(px[i] - ballRadius < -3.9 && vx[i] < 0) vx[i] = -vx[i];
			if(px[i] + ballRadius > +3.9 && vx[i] > 0) vx[i] = -vx[i];
			if(pz[i] - ballRadius < -1.9 && vz[i] < 0) vz[i] = -vz[i];
			if(pz[i] + ballRadius > +1.9 && vz[i] > 0) vz[i] = -vz[i];
		}

		// update ball velocities
		for(i = 0; i < 16; i++)
		{
			// ignore pocketed balls
			if(pocketed[i]) continue;

			// velocity magnitude
			double v = sqrt(vx[i] * vx[i] + vz[i] * vz[i]);
			double k = 5; // coefficient of friction

			// stop balls if too slow
			if(v < k * timeStep)
			{
				vx[i] = 0;
				vz[i] = 0;
				continue;
			}

			// apply friction
			double f = 1 - k * timeStep / v;
			if(f < 0) f = 0;
			vx[i] *= f;
			vz[i] *= f;
		}

		// update ball positions
		for(i = 0; i < 16; i++)
		{
			// ignore pocketed balls
			if(pocketed[i]) continue;

			// change positions according to velocities
			px[i] += vx[i] * timeStep;
			pz[i] += vz[i] * timeStep;
		}

		// if cue ball is pocketed, place it at center
		if(stopped() && pocketed[0])
		{
			px[0] = 0;
			pz[0] = 0;
			vx[0] = 0;
			vz[0] = 0;
			pocketed[0] = false;
		}

		// if all balls are pocketed, reset the game
		if(pocketedNumber() == 15) reset();
	}

	// ball drawing
	void draw()
	{
		for(int i = 0; i < 16; i++)
		{
			// ignore pocketed balls
			if(pocketed[i]) continue;

			// ball shadow
			setMaterialColor(0.1, 0.2, 0.1); // gray color
			setMaterialShininess(0.0); // not shiny
			glPushMatrix();
			glTranslated(px[i], 0, pz[i]);
			double slope = ballRadius / lightPosition[1]; // shadow slope
			glTranslated((px[i] - lightPosition[0]) * slope, 0, (pz[i] - lightPosition[2]) * slope);
			glRotated(-90, 1, 0, 0);
			glutSolidCone(ballRadius, 0.01, 20, 1);
			glPopMatrix();

			// ball
			setMaterialColor(0.8, 0.8, 0.8); // white color
			setMaterialShininess(1.0); // shiny
			glPushMatrix();
			glTranslated(px[i], ballRadius, pz[i]);
			glRotated(180, 1, 0, 0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textures[i]); // select ball texture
			gluSphere(qobj, ballRadius, 30, 30);
			glDisable(GL_TEXTURE_2D);
			glPopMatrix();
		}
	}
};
