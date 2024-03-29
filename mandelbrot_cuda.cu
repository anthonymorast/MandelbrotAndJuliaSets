#include "global.h"

Mandelbrot_cu::Mandelbrot_cu()
{
		// set xmin, xmax, ymin, ymax to default values.
		this->ComplexXMin = -2.00;
		this->ComplexXMax = 1;
		this->ComplexYMin = -1.5;
		this->ComplexYMax = 1.5;
}

__global__ void CalcPoint( float *x, float *y, int *scheme, int nx, int ny, int maxIter ) 
{
	// get index into x array
	int index = threadIdx.x + blockIdx.x * blockDim.x;
	if ( index < nx ) 
	{
		int start = index*ny;
		int end = start + ny;
		// iterate the column values corresponding to this row
		for( int i = start; i < end; i++ )
		{
			// x0 and y0 to be added onto points
			double zx0 = x[index];
			double zy0 = y[i];

			double zx = x[index];
			double zy = y[i];

			int count = 0;
			
			// determine convergence/divergence
			while( ( zx*zx + zy*zy <= 4.0 ) && ( count < maxIter ) )
			{
				// complex square
				double new_zx = zx*zx - zy*zy;
				double new_zy = 2 * zx * zy;
				zx = new_zx;
				zy = new_zy;

				// add z0
				zx += zx0;
				zy += zy0;

				// incr count
				count++;
			}
		
			// set color
			if( count >= maxIter )
			{
				scheme[i] = 0;
			}
			else if ( count > ( maxIter / 8) ) 
			{
				scheme[i] = 2;
			}
			else if ( count > ( maxIter / 10) ) 
			{
				scheme[i] = 3;
			}
			else if ( count > ( maxIter / 20) ) 
			{
				scheme[i] = 4;
			}
			else if ( count > ( maxIter / 40) ) 
			{
				scheme[i] = 5;
			}
			else if ( count > ( maxIter / 100) ) 
			{
				scheme[i] = 6;
			}
			else if ( count > (maxIter / 200) )
			{
				scheme[i] = 7;
			}
			else if ( count > (maxIter / 400) )
			{
				scheme[i] = 8;
			}
			else if ( count > (maxIter / 600) )
			{
				scheme[i] = 9;
			}
			else if ( count > (maxIter / 800) )
			{
				scheme[i] = 1;
			}
			else 
			{
				scheme[i] = 10;
			}
		}
	}
}

vector< ComplexPoint > Mandelbrot_cu::GetPoints( int nx, int ny, int maxIter ) 
{
	vector< ComplexPoint > points;

	// determine array size and allocate memory
	int size_nx = nx * sizeof( float );
	int size_nynx = ( nx*ny ) * sizeof( float );
	int size_sch = ( nx*ny ) * sizeof( int );	

	float *x = ( float * )malloc(size_nx);
	float *y = ( float * )malloc(size_nynx);
	int *scheme = ( int *)malloc(size_sch);
	
	// fill arrays with points before passing	
	ComplexWidth = ComplexXMax - ComplexXMin;
	ComplexHeight = ComplexYMax - ComplexYMin;

	ComplexPoint z, zIncr;

	// calculates x and y increments
	zIncr.x = ComplexWidth / float( nx );
	zIncr.y = ComplexHeight / float( ny );

	for( int i = 0; i < nx; i++ )
	{
		// get and set complex x value
		x[i] = ComplexXMin + ( zIncr.x * i );
		int multiplier = 0;
		for( int j = i*ny; j < (i+1)*ny; j++ )
		{
			// get and set complex y value (and default scheme)
			y[j] = ComplexYMin + ( zIncr.y * multiplier );
			scheme[j] = 0;
			multiplier++;
		}
	}

	// Do host side CUDA prep and run kernel on CUDA device
	// create device vectors
	float *d_x, *d_y;
	int *d_scheme;

	// allocate memory on the device for our arrays
	cudaMalloc( ( void** )&d_x, size_nx );
	cudaMalloc( ( void** )&d_y, size_nynx );
	cudaMalloc( ( void** )&d_scheme, size_sch );

	// copy memory from host to the device
	cudaMemcpy( d_x, x, size_nx, cudaMemcpyHostToDevice );
	cudaMemcpy( d_y, y, size_nynx, cudaMemcpyHostToDevice );
	cudaMemcpy( d_scheme, scheme, size_sch,  cudaMemcpyHostToDevice );

	// set number of threads and calculates blocks
	int nThreads = 64;
	int nBlocks = ( nx + nThreads - 1 ) / nThreads;

	// calculate scheme indexes on the GPU
	CalcPoint<<< nBlocks, nThreads >>>( d_x, d_y, d_scheme, nx, ny, maxIter );

	// copy arrays back to host from GPU
	cudaMemcpy( x, d_x, size_nx, cudaMemcpyDeviceToHost );
	cudaMemcpy( y, d_y, size_nynx, cudaMemcpyDeviceToHost );
	cudaMemcpy( scheme, d_scheme, size_sch, cudaMemcpyDeviceToHost );

	// create points from the x, y and scheme values and push onto vector
	for( int i = 0; i < nx; i++ )
	{
		z.x = x[i];
		
		for( int j = i*ny; j < (i+1)*ny; j++ )
		{
			z.y = y[j];
			z.schemeIndex = scheme[j];
			points.push_back(z);
		}
	}
	
	// free memory
	free(x); free(y); free(scheme);
	cudaFree(d_x); cudaFree(d_y); cudaFree(d_scheme);
	return points;
}

// Getters and setters for xmin, xmax, ymin, and ymax
double Mandelbrot_cu::GetComplexXMin()
{
	return this->ComplexXMin;
}

double Mandelbrot_cu::GetComplexXMax()
{
	return this->ComplexXMax;
}

double Mandelbrot_cu::GetComplexYMin()
{
	return this->ComplexYMin;
}

double Mandelbrot_cu::GetComplexYMax()
{
	return this->ComplexYMax;
}


void Mandelbrot_cu::SetComplexXMin( double xmin )
{
	this->ComplexXMin = xmin;
}

void Mandelbrot_cu::SetComplexXMax( double xmax )
{
	this->ComplexXMax = xmax;
}

void Mandelbrot_cu::SetComplexYMin( double ymin )
{
	this->ComplexYMin = ymin;
}

void Mandelbrot_cu::SetComplexYMax( double ymax )
{
	this->ComplexYMax = ymax;
}
