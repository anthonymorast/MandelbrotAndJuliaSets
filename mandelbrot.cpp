#include "mandelbrot.h" 

int manWin, julWin;

int main( int argc, char* argv[] )
{
	// get Mandelbrot points
	MandelbrotPoints = mandelbrot.GetPoints(1000, 1000, 1000, NULL);
	vector<ComplexPoint> test = mandelbrotCu.GetPoints(1000, 1000, 1000, NULL);

	cout << test.size() << " == " << MandelbrotPoints.size() << endl;

	// Initialize glut/openGL
	glutInit( &argc, argv );
	initOpenGL();

	// enter glut main loop
	glutMainLoop();

	/*
		"To keep the compiler from bitching."
			- Dr. John Weiss
	*/
	return 0;
}

/*******************************************************************************
 *								OpenGL Functions							   *
 ******************************************************************************/
void display( void )
{
	//glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	float xmin = 0;
	float xmax = 0;
	float ymin = 0;
	float ymax = 0;

	if( !IsJulia )
	{
		xmin = mandelbrot.GetComplexXMin();
		xmax = mandelbrot.GetComplexXMax();
		ymin = mandelbrot.GetComplexYMin();
		ymax = mandelbrot.GetComplexYMax();
	}
	else 
	{
		xmin = julia.GetComplexXMin();
		xmax = julia.GetComplexXMax();
		ymin = julia.GetComplexYMin();
		ymax = julia.GetComplexYMax();
	}

	gluOrtho2D( xmin, xmax, ymin, ymax );
	glViewport( 0, 0, ScreenWidth, ScreenHeight );

	glClear( GL_COLOR_BUFFER_BIT );	
	glutSwapBuffers();

	vector<ComplexPoint> plotVec = IsJulia ? JuliaPoints : MandelbrotPoints;

	ComplexPoint pt;
	for( vector<ComplexPoint>::const_iterator it=plotVec.begin(); it < plotVec.end(); it++ )
	{
		pt = *it;
		float color[3] = { pt.color.r, pt.color.g, pt.color.b };
		//cout << pt.x << ", " << pt.y << endl;
		glColor3fv( color );
		glBegin( GL_POINTS );
			glVertex2f( pt.x, pt.y );
		glEnd();
	}

	glFlush();
}

void reshape( int w, int h )
{
	ScreenWidth = w;
	ScreenHeight = h;

	float xmin = 0;
	float xmax = 0;
	float ymin = 0;
	float ymax = 0;

	if( !IsJulia )
	{
		xmin = mandelbrot.GetComplexXMin();
		xmax = mandelbrot.GetComplexXMax();
		ymin = mandelbrot.GetComplexYMin();
		ymax = mandelbrot.GetComplexYMax();
	}
	else 
	{
		// set x/y min/max based on Julia values 
		xmin = julia.GetComplexXMin();
		xmax = julia.GetComplexXMax();
		ymin = julia.GetComplexYMin();
		ymax = julia.GetComplexYMax();
	}

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	gluOrtho2D( xmin, xmax, ymin, ymax  );
	//glutReshapeWindow( ScreenWidth, ScreenHeight );
	glViewport( 0, 0, ScreenWidth, ScreenHeight );


	glClear( GL_COLOR_BUFFER_BIT );
}

void keyboard( unsigned char key, int x, int y )
{
	// +/- keys for zoom (scaling transform)
	// J - toggle between Mandelbrot and Julia Sets at Current Cursor position
	// 	   (open new window for Julia)
	// C - change color maps
	// R - generate random color map
	// A - animate by cycling between color maps
	switch ( key ) 
	{
		case Plus:
			break;
		case Minus:
			break;
		case J:
			// Calc/Open Julia Set
			if ( !IsJulia )
			{
				JuliaPoints = julia.GetPoints( JuliaSeed, 1000, 1000, 1000 );
			}
			IsJulia = !IsJulia;
			break;
		case C:
			break;
		case R:
			break;
		case A:
			break;
	}	
	glutPostRedisplay();
}

void mouseclick( int button, int state, int x, int y )
{
	
}

void mousemove( int x, int y )
{
	// invert y
	y = ScreenHeight - y;


	float xmin = 0;
	float xmax = 0;
	float ymin = 0;
	float ymax = 0;

	if ( !IsJulia ) 
	{
		// track the mouse position to open Julia Set
		xmin = mandelbrot.GetComplexXMin();
		xmax = mandelbrot.GetComplexXMax();
		ymin = mandelbrot.GetComplexYMin();
		ymax = mandelbrot.GetComplexYMax();


		float gridStepsX = ( ( fabs(xmax) + fabs(xmin) ) / ScreenWidth );
		float plotx = xmin + ( x * gridStepsX );

		float gridStepsY = ( ( fabs(ymax) + fabs(ymin) ) / ScreenHeight );
		float ploty = ymin + ( y * gridStepsY );

		JuliaSeed.x = plotx;
		JuliaSeed.y = ploty;
	}
	else 
	{
		// Not sure what to do here
		xmin = julia.GetComplexXMin();
		xmax = julia.GetComplexXMax();
		ymin = julia.GetComplexYMin();
		ymax = julia.GetComplexYMax();

		float gridStepsX = ( ( fabs(xmax) + fabs(xmin) ) / ScreenWidth );
		float gridStepsY = ( ( fabs(ymax) + fabs(ymin) ) / ScreenHeight );
		
		float plotx = xmin + ( x * gridStepsX );
		float ploty = ymin + ( y * gridStepsY );

		JuliaSeed.x = plotx;
		JuliaSeed.y = ploty;
	}
}

/*******************************************************************************
 *								Misc. Functions								   *
 ******************************************************************************/
void initOpenGL( void )
{
	glutInitDisplayMode( GLUT_RGB | GLUT_SINGLE );
	
	// window settings 
	glutInitWindowSize( ScreenWidth, ScreenHeight );
	glutInitWindowPosition( 200, 40 );
	manWin = glutCreateWindow( "NineteenSixtyX" );
	
	// color used to clear screen - black
	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	// callbacks
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyboard );
	glutPassiveMotionFunc( mousemove );
}
