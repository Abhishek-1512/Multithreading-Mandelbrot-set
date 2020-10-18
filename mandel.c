/* NAME:Abhishek Jain
   ID: 1001759977
*/
#include "bitmap.h"
#include <time.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

int iteration_to_color( int i, int max );
int iterations_at_point( double x, double y, int max );
void* compute_image(void *bm);

void show_help()
{
    printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates. (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=500)\n");
	printf("-H <pixels> Height of the image in pixels. (default=500)\n");
	printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

int threads = 1;           //Default number of threads
int k;                     //counter to create multiple threads
int l=-1;                  //counter to track a particular thread

double xcenter = 0;        // These are the default configuration values used
double ycenter = 0;        // if no command line arguments are given.
double scale = 4;
int image_width = 500;
int image_height = 500;
int max = 1000;

double xmin;
double xmax;
double ymin;
double ymax;
int max2;

int main( int argc, char *argv[] )
{
	char c;

	const char *outfile = "mandel.bmp";

	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"x:y:s:W:H:m:n:o:h"))!=-1) {
		switch(c) {
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				scale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
            case 'n':
                threads = atoi(optarg);
                break;
            case 'o':
				outfile = optarg;
				break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}

    xmin = (xcenter-scale);
    xmax = (xcenter+scale);
    ymin = (ycenter-scale);
    ymax = (ycenter+scale);
    max2 = max;

	// Display the configuration of the image.

	printf("mandel: x=%lf y=%lf scale=%lf max=%d outfile=%s\n",xcenter,ycenter,scale,max,outfile);

	// Create a bitmap of the appropriate size.
	struct bitmap *bm = bitmap_create(image_width,image_height);

	// Fill it with a dark blue, for debugging
//	bitmap_reset(bm,MAKE_RGBA(0,0,255,0));

    pthread_t compute_thread[threads];                       //Creating an array of thread ids

    // Creating number of threads defined by the user or remains 1 if number of threads is not specified by the user.

	for(k=0;k<threads;k++)
        pthread_create(&compute_thread[k],NULL,compute_image,(void*) bm);

    // Joining all the threads
    for(k=0;k<threads;k++)
        pthread_join(compute_thread[k], NULL);

	// Save the image in the stated file.
	if(!bitmap_save(bm,outfile)) {
		fprintf(stderr,"mandel: couldn't write to %s: %s\n",outfile,strerror(errno));
		return 1;
	}

	return 0;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void* compute_image(void *bm)
{
	int i,start_height,j,thread_counter;
	int width = bitmap_width(bm);
	int height = bitmap_height(bm);

	l++;                                                // global variable-counter to keep track of threads
    thread_counter = l;                                 // thread_counter to track any particular thread

    int initial_height = bitmap_height(bm);             // storing the original height of the complete image separately
	int thread_length;                                  // thread_length stores the length of each thread
	thread_length = initial_height/threads;             // Calculating length of each thread as a ratio of total height and number of threads
	height = (thread_length * (thread_counter + 1));    // It is (thread_counter + one) and not (thread_counter + L). Sets the max height value of each thread

	start_height = height-thread_length;                // Sets the starting height value of each thread by subtracting the thread size from max height of that thread

	// For every pixel in the image...

	for(j=start_height;j<height;j++) {                  // Runs from initial height to max height for each thread

		for(i=0;i<width;i++) {

			// Determine the point in x,y space for that pixel.

			double x = xmin + i*(xmax-xmin)/width;

			double y = ymin + j*(ymax-ymin)/height;

			// Compute the iterations at that point.
			int iters = iterations_at_point(x,y,max);

			// Set the pixel in the bitmap.
			bitmap_set(bm,i,j,iters);
		}
	}
}

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}
	return iteration_to_color(iter,max);
}

/*
Convert a iteration number to an RGBA color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/

int iteration_to_color( int i, int max )
{
	int gray = 255*i/max;
	return MAKE_RGBA(gray,gray,gray,0);
}




