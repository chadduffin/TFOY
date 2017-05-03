#include "yendor.h"
#include "globals.h"

const int gradient[12][3] = {
	{1, 1, 0}, {1, -1, 0}, {-1, 1, 0}, {-1, -1, 0},
	{0, 1, 1}, {0, 1, -1}, {0, -1, 1}, {0, -1, -1},
	{1, 0, 1}, {1, 0, -1}, {-1, 0, 1}, {-1, 0, -1}
};

double DotVec2(int Index, double X, double Y) {
	return ((gradient[Index][0]*X)+(gradient[Index][1]*Y));
}

int QuickFloor(double Value) {
	return (Value > 0) ? (int)(Value) : (int)((Value)-1);
}

void G_FillChunk(G_SceneChunk* chunk, int id) {

}

double G_NoiseOctaveSum(int X, int Y) {
  int Octaves = 8;
  double Scale = 0.0075, Persistence = 0.6;
	double noise = 0.0,
		amplitude = 1.0,
		maxAmplitude = 0.0;
	
	for (int i = 0; i < Octaves; i += 1) {
		noise += G_Noise(X*Scale, Y*Scale)*amplitude;
		maxAmplitude += amplitude;
		amplitude *= Persistence;
		Scale *= 2;
	}

	noise /=maxAmplitude;
	noise = (noise+1)/2;
	return noise;
}

double G_Noise(double X, double Y) {
	double n0,
		n1,
		n2;
	
	double F2 = .5*(sqrt(3.0)-1.0);
	double s = (X+Y)*F2;
	int i = QuickFloor(X+s);
	int j = QuickFloor(Y+s);
	
	double G2 = (3.0-sqrt(3.0))/6.0;
	double t = (i+j)*G2;
	double X0 = i-t;
	double Y0 = j-t;
	double x0 = X-X0;
	double y0 = Y-Y0;

	int i1,
		j1;
	if (x0 > y0) {
		i1 = 1;
		j1 = 0;
	} else {
		i1 = 0;
		j1 = 1;
	}

	double x1 = x0-i1+G2;
	double y1 = y0-j1+G2;
	double x2 = x0-1.0+(2.0*G2);
	double y2 = y0-1.0+(2.0*G2);

	int ii = i & 255;
	int jj = j & 255;
	int gi0 = permutations[ii+permutations[jj]]%12;
	int gi1 = permutations[ii+i1+permutations[jj+j1]]%12;
	int gi2 = permutations[ii+1+permutations[jj+1]]%12;
	
	double t0 = .5-(x0*x0)-(y0*y0);
	if (t0 < 0) {
		n0 = 0.0;
	} else {
		t0 *= t0;
		n0 = t0*t0*DotVec2(gi0, x0, y0);
	}
	double t1 = .5-(x1*x1)-(y1*y1);
	if (t1 < 0) {
		n1 = 0.0;
	} else {
		t1 *= t1;
		n1 = t1*t1*DotVec2(gi1, x1, y1);
	}
	double t2 = .5-(x2*x2)-(y2*y2);
	if (t2 < 0) {
		n2 = 0.0;
	} else {
		t2 *= t2;
		n2 = t2*t2*DotVec2(gi2, x2, y2);
	}

	return 70.0 * (n0 + n1 + n2);
}
