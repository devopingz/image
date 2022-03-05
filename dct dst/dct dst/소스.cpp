#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.141592
#define N 16
#define SIZE 512

double input[SIZE][SIZE];
double temp[SIZE][SIZE];
double output2[SIZE][SIZE];
double dct_output[SIZE][SIZE];
double idct_output[SIZE][SIZE];
double dst_output[SIZE][SIZE];
double idst_output[SIZE][SIZE];
double temp2[SIZE][SIZE];
double temp3[SIZE][SIZE];
double temp4[SIZE][SIZE];

void read_file(FILE *in, double input[SIZE][SIZE])
{
	int i, j;
	for (i = 0; i<SIZE; i++)
		for (j = 0; j<SIZE; j++)
			input[i][j] = fgetc(in);
}

void write_file(FILE *out, double out_data[SIZE][SIZE])
{
	int  i, j;
	for (i = 0; i<SIZE; i++)
		for (j = 0; j<SIZE; j++)
			fputc(out_data[i][j], out);
}

void do_DCT(int i, int j)
{
	int a, b, c, d;
	double au, av;  double temp_out[N][N] = { 0, };

	for (a = 0; a < N; a++) {

		for (b = 0; b < N; b++)
		{

			if (a == 0) au = (1.0) / sqrt(N);
			else au = sqrt(2.0 / N);
			if (b == 0) av = (1.0) / sqrt(N);
			else av = sqrt(2.0 / N);

			for (c = 0; c < N; c++) {
				for (d = 0; d < N; d++) {

					temp_out[a][b] += au*av*(input[i + c][j + d] - 128)*cos(((2 * c + 1)*a*PI) / (2 * N))*cos(((2 * d + 1)*b*PI) / (2 * N));
				}
			}

			temp[i + a][j + b] = temp_out[a][b];
			temp_out[a][b] *= 7;
			if (temp[i + a][j + b] < 0)	dct_output[i + a][j + b] = 0;
			else if (temp[i + a][j + b] > 255) dct_output[i + a][j + b] = 255.0;
			else dct_output[i + a][j + b] = temp_out[a][b];
		}
	}
}

void do_IDCT(int i, int j)
{
	int a, b, c, d;
	double au, av;
	double temp_output[N][N] = { 0, };

	for (a = 0; a < N; a++) {
		for (b = 0; b < N; b++)
		{
			for (c = 0; c < N; c++)
				for (d = 0; d < N; d++)
				{

					if (c == 0) au = (1.0) / sqrt(N);
					else au = sqrt(2.0 / N);
					if (d == 0) av = (1.0) / sqrt(N);
					else av = sqrt(2.0 / N);

					temp_output[a][b] += au*av*(temp[i + c][j + d])*cos(((2 * a + 1)*c*PI) / (2 * N))*cos(((2 * b + 1)*d*PI) / (2 * N));
				}

			temp_output[a][b] += 128.0;
			output2[i + a][j + b] = temp_output[a][b];

			if (output2[i + a][j + b] < 0)	idct_output[i + a][j + b] = 0;
			else if (output2[i + a][j + b] > 255) idct_output[i + a][j + b] = 255.0;
			else idct_output[i + a][j + b] = temp_output[a][b];
		}
	}
}

void do_DST(int i, int j){

	int a, b, c, d;
	double temp_output[N][N] = { 0, };

	for (a = 0; a < N; a++) {
		for (b = 0; b < N; b++)
		{
			for (c = 0; c < N; c++) {
				for (d = 0; d < N; d++) {

					temp_output[a][b] += (input[i + c][j + d]-80)*sin(((c + 1)*(a + 1)*PI) / (N + 1))*sin(((d + 1)*(b + 1)*PI) / (N + 1));
				}
			}
			temp_output[a][b] *= (2.0) / (N + 1);
			temp2[i + a][j + b] = temp_output[a][b];
			temp_output[a][b] *= 8;

			if (temp2[i + a][j + b] < 0) dst_output[i + a][j + b] = 0;
			else if (temp2[i + a][j + b] > 255) dst_output[i + a][j + b] = 255.0;
			else dst_output[i + a][j + b] = temp_output[a][b];
		}
	}
}

void do_IDST(int i, int j)
{
	int a, b, c, d;
	double temp_output[N][N] = { 0, };
	for (a = 0; a<N; a++)
		for (b = 0; b<N; b++)
		{
			for (c = 0; c<N; c++)
				for (d = 0; d<N; d++)
				{
					temp_output[a][b] += (temp2[i + c][j + d])*sin(((c + 1)*(a + 1)*PI) / (N + 1))*sin(((d + 1)*(b + 1)*PI) / (N + 1));
				}

			temp_output[a][b] *= 2.0 / (N + 1);
			temp_output[a][b] += 80.0;

			output2[i + a][j + b] = temp_output[a][b];

			if (output2[i + a][j + b] < 0)	idst_output[i + a][j + b] = 0;
			else if (output2[i + a][j + b] > 255) idst_output[i + a][j + b] = 255.0;
			else idst_output[i + a][j + b] = temp_output[a][b];
		}
}


void main(void)
{
	int  i, j;
	FILE  *in, *DCT, *IDCT, *DST, *IDST;
	in = fopen("lena.img", "rb");

	if (in == NULL)
	{
		printf("File is not found!\n");
	}

	DCT = fopen("DCT.img", "wb");
	IDCT = fopen("IDCT.img", "wb");
	DST = fopen("DST.img", "wb");
	IDST = fopen("IDST.img", "wb");

	read_file(in, input);

	for (i = 0; i < SIZE; i += N) {
		for (j = 0; j < SIZE; j += N) {
			do_DCT(i, j);
		}
	}

	write_file(DCT, dct_output);

	for (i = 0; i < SIZE; i += N) {
		for (j = 0; j < SIZE; j += N) {
			do_IDCT(i, j);
		}
	}

	write_file(IDCT,idct_output);

	for (i = 0; i < SIZE; i+=N) {
		for (j = 0; j < SIZE; j+=N){

			do_DST(i, j);
		}
	}
	write_file(DST,dst_output);

	for (i = 0; i < SIZE; i += N) {
		for (j = 0; j < SIZE; j += N) {
			do_IDST(i, j);
		}
	}
	write_file(IDST, idst_output);

	fclose(DCT);
	fclose(IDCT);
	fclose(DST);
	fclose(IDST);
}