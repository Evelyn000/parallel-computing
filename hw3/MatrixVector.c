#include <stdio.h>
#include <stdlib.h>
#include <time.h>
void rerror (char *s)
{
   printf ("%s\n", s);
   exit (-1);
}

void allocate_matrix (float ***subs, int rows, int cols) {
   int    i;
   float *lptr, *rptr;
   float *storage;

   storage = (float *) malloc (rows * cols * sizeof(float));
   *subs = (float **) malloc (rows * sizeof(float *));
   for (i = 0; i < rows; i++)
      (*subs)[i] = &storage[i*cols];
   return;
}

void read_matrix (
   char    *s,          /* File name */
   float ***subs,       /* 2D submatrix indices */
   int     *m,          /* Number of rows in matrix */
   int     *n)          /* Number of columns in matrix */
{
   char     error_msg[80];
   FILE    *fptr;          /* Input file pointer */

   fptr = fopen (s, "r");
   if (fptr == NULL) {
      sprintf (error_msg, "Can't open file '%s'", s);
      rerror (error_msg);
   }
   fread (m, sizeof(int), 1, fptr);
   fread (n, sizeof(int), 1, fptr);
   allocate_matrix (subs, *m, *n);
   fread ((*subs)[0], sizeof(float), *m * *n, fptr);
   fclose (fptr);
   return;
}

void read_vector(char *s, float **subs, int *m, int *n)
{
	char error_msg[80];
	FILE *fptr;
	fptr = fopen (s, "r");
   if (fptr == NULL) {
      sprintf (error_msg, "Can't open file '%s'", s);
      rerror (error_msg);
   }
   fread (m, sizeof(int), 1, fptr);
   fread (n, sizeof(int), 1, fptr);
   *subs = malloc (*m * sizeof(float));
   fread (*subs, sizeof(float), *m, fptr);
   fclose (fptr);
   return;
}

void print_matrix (float **a, int rows, int cols)
{
   int i, j;

   for (i = 0; i < rows; i++) {
      for (j = 0; j < cols; j++)
	 printf ("%6.2f ", a[i][j]);
      putchar ('\n');
   }
   putchar ('\n');
   return;
}

void print_vector(float *b, int rows)
{
	for (int i=0; i<rows; i++)
		printf("%6.2f", b[i]);
	putchar('\n');
	return;
}

void matrix_multiply (float **a, float *b, float *c,
		      int arows, int acols)
{
   int i, k;
   float tmp;

   for (i = 0; i < arows; i++)
   {
	  tmp = 0.0;
	  for (k = 0; k < acols; k++)
	    tmp += a[i][k] * b[k];
	  c[i] = tmp;
   }
   return;
}

int main (int argc, char *argv[])
{
   int m1, n1;        /* Dimensions of matrix 'a' */
   int m2, n2;        /* Dimensions of matrix 'b' */
   float **a, *b;    /* Two matrices being multiplied */
   float *c;         /* Product matrix */

   read_matrix ("matrix_a", &a, &m1, &n1);
   //print_matrix (a, m1, n1);
   //read_matrix ("matrix_b", &b, &m2, &n2);
   read_vector ("matrix_b", &b, &m2, &n2);
   //print_vector(b, m2);
   if (n1 != m2 || n2 != 1) rerror ("Incompatible matri/vector dimensions");
   c = malloc (m2 * sizeof(float));
    int t=0-clock();
   matrix_multiply (a, b, c, m1, n1);
    
    t+=clock();
   //print_vector(c, n1);
    
    printf("time: %f s\n", (float)t/1000000);
    return 0;
}
