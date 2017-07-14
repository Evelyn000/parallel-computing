#include <stdio.h>
#include <stdlib.h>
void rerror (const char *s)
{
   printf ("%s\n", s);
   exit (-1);
}


void read_matrix (
   const char    *s,          /* File name */
   float **subs,       /* 2D submatrix indices */
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
   *subs=(float*)malloc((*m)*(*n)*sizeof(float));
   //allocate_matrix (subs, *m, *n);
   fread (*subs, sizeof(float), *m * *n, fptr);
   fclose (fptr);
   return;
}

void read_vector(const char *s, float **subs, int *m, int *n)
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
   *subs = (float*)malloc (*m * sizeof(float));
   fread (*subs, sizeof(float), *m, fptr);
   fclose (fptr);
   return;
}

void print_matrix (float *a, int rows, int cols)
{
   int i, j;

   for (i = 0; i < rows; i++) {
      for (j = 0; j < cols; j++)
	 printf ("%6.2f ", a[i*cols+j]);
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