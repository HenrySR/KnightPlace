
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define NUM_THREADS 4



void main(){
  int sample_points,inside;
  double x, y;
  inside = 0;
  int i;

  
  #pragma omp parallel reduction(+:sample_points, inside) num_threads(NUM_THREADS) 
  {
    
    sample_points= 20000000;
  for (i = 0; i < sample_points; i++) {   
  x = (double)(rand())/(RAND_MAX)-.5;
  y =(double)(rand())/(RAND_MAX)-.5;
  if (((x * x) + (y * y)) < .25)
  inside++;
  }
  }
  double pi;
  pi= 8*((double)inside/(double)sample_points);
  printf("Pi is about %f", pi);
  
  return;
}