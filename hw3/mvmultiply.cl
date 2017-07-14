__kernel void mvmult(__global const float* a, __global const float* b, __global float* c, const int h, const int w)
{
	int row=get_global_id(0);
	//for (int row=0; row<h; row++)
	//{
	float sum=0.0;
    int pos=row*w;
	for (int i=0; i<w; i++)
		sum+=a[pos+i]*b[i];
	c[row]=sum;
	//}
}
