# Multithreading-Mandelbrot-set
Generating images in the Mandelbrot set, which is a well known fractal structure. The set is interesting both mathematically and aesthetically because it has an infinitely recursive structure.

Instead of running multiple programs at once, we are using an approach of making each individual process faster by using multiple threads.
Using use an arbitrary number of threads(user input) to compute the Mandelbrot image. Each thread computes a completely separate band of the image. 

For example, if user specifies three threads and the image is 500 pixels high, then thread 0 works on lines 0-165, thread 1 works on lines 166-331, and thread 2 works on lines 332-499.
