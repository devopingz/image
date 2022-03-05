#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.141592
#define N 16
#define SIZE 512
#define DFT_N 256

unsigned char input[SIZE][SIZE];	
unsigned char output[SIZE][SIZE];
double temp[SIZE][SIZE];			
double output2[SIZE][SIZE];	   
double temp2[SIZE][SIZE];
double temp3[SIZE][SIZE];
double temp4[SIZE][SIZE];
double real[DFT_N][DFT_N];
double img[DFT_N][DFT_N];
double idft_real[DFT_N][DFT_N];

void read_file(FILE *in)
{
	int i, j;
	for (i = 0; i<SIZE; i++)
		for (j = 0; j<SIZE; j++)
			input[i][j] = fgetc(in);
}

void read_file_mini(FILE *in) {

	int i, j;
	for (i = 0; i < DFT_N; i++) {

		for (j = 0; j < DFT_N; j++) {

			input[i][j] = fgetc(in);
		}
	}
}

void write_file_mini(FILE *out) {

	int i, j;
	for (i = 0; i < DFT_N; i++) {

		for (j = 0; j < DFT_N; j++) {

			fputc(output[i][j], out);
		}
	}
}

void write_file(FILE *out)
{
	int  i, j;
	for (i = 0; i<SIZE; i++)
		for (j = 0; j<SIZE; j++)
			fputc(output[i][j], out);
}

void do_DFT(int i, int j) {

	int a, b, c, d;  double c1, coef, result, dc_value;

	for (a = 0; a < DFT_N; a++) {

		for (b = 0; b < DFT_N; b++) {

			for (c = 0; c < DFT_N; c++) {

				for (d = 0; d < DFT_N ; d++) {

					real[a - DFT_N/2][b - DFT_N/2]+= pow(-1,c+d)*input[c][d] * cos(2 * PI *((b- DFT_N/2)* d + (a- DFT_N/2)*c) / DFT_N);
					img[a- DFT_N/2][b- DFT_N/2] += (-1) * pow(-1,c+d)* input[c][d] * sin(2 * PI*((b - DFT_N/2) *d + (a - DFT_N/2) *c) / DFT_N);
				}
			}

			real[a- DFT_N/2][b- DFT_N/2] = real[a- DFT_N/2][b- DFT_N/2] / (DFT_N  * DFT_N);
			img[a- DFT_N/2][b- DFT_N/2] = img[a- DFT_N/2][b- DFT_N/2] / (DFT_N * DFT_N);
		}
	}

	dc_value = hypot(real[DFT_N/2][DFT_N/2], img[DFT_N/2][DFT_N/2]);

	for (a = 0; a < DFT_N; a++) {

		for (b = 0; b < DFT_N; b++) {

			coef = hypot(real[a][b], img[a][b]);
			result =  255.0 * log(coef + 1.0) / log(dc_value+1.0);
			if (result > 255.0) result = 255;
			if (result < 0.0) result = 0;
			output[a][b] = result;
		}
	}
}

void do_IDFT(int i, int j) {

	int a, b, c, d;  double result;

	for (a = 0; a < DFT_N; a++) {

		for (b = 0; b < DFT_N; b++) {

			for (c = 0; c < DFT_N; c++) {

				for (d = 0; d < DFT_N; d++) {

					idft_real[a][b] += real[c- DFT_N / 2][d- DFT_N / 2] * cos(2 * PI *(b*(d- DFT_N / 2) + a*(c- DFT_N / 2)) / DFT_N) -
						img[c- DFT_N / 2][d- DFT_N / 2] * sin(2 * PI *(b*(d- DFT_N / 2) + a*(c- DFT_N / 2)) / DFT_N);
				}
			}
		}
	}

	for (a = 0; a <DFT_N; a++) {

		for (b = 0; b < DFT_N ; b++) {
		
			result = idft_real[a][b] * pow(-1, a+b);
			output[a][b] = result;
			//printf("%d\n", output[a][b]);		
		}
	}
}

void do_DCT(int i, int j)
{
	int a, b, c, d;
	double au, av; double dct_output[N][N] = { 0, };

	for (a = 0; a < N; a++) {

		for (b = 0; b < N; b++)
		{

			if (a == 0) au = (1.0) / sqrt(N);
			else au = sqrt(2.0 / N);
			if (b == 0) av = (1.0) / sqrt(N);
			else av = sqrt(2.0 / N);

			for (c = 0; c < N; c++) {
				for (d = 0; d < N; d++) {

					dct_output[a][b] += au*av*(input[i + c][j + d] - 128)*cos(((2 * c + 1)*a*PI) / (2 * N))*cos(((2 * d + 1)*b*PI) / (2 * N));
				}
			}

			temp[i + a][j + b] = dct_output[a][b];

			if (temp[i + a][j + b] < 0)	output[i + a][j + b] = 0;
			else if (temp[i + a][j + b] > 255) output[i + a][j + b] = 255.0;
			else output[i + a][j + b] = dct_output[a][b];
		}
	}
}

void do_IDCT(int i, int j)
{
	int a, b, c, d;
	double au, av;
	double idct_output[N][N] = { 0, };

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

					idct_output[a][b] += au*av*(temp[i + c][j + d])*cos(((2 * a + 1)*c*PI) / (2 * N))*cos(((2 * b + 1)*d*PI) / (2 * N));
				}

			idct_output[a][b] += 128.0;
			output2[i + a][j + b] = idct_output[a][b];

			if (output2[i + a][j + b] < 0)	output[i + a][j + b] = 0;
			else if (output2[i + a][j + b] > 255) output[i + a][j + b] = 255.0;
			else output[i + a][j + b] = idct_output[a][b];
		}
	}
}

void do_DST(int i, int j){

	int a, b, c, d;
	double dst_output[N][N] = { 0, };

	for (a = 0; a < N; a++) {
		for (b = 0; b < N; b++)
		{
			for (c = 0; c < N; c++) {
				for (d = 0; d < N; d++) {

					dst_output[a][b] += (input[i + c][j + d] - 128)*sin(((c + 1)*(a + 1)*PI) / (N + 1))*sin(((d + 1)*(b + 1)*PI) / (N + 1));
				}
			}
			dst_output[a][b] *= (2.0) / (N + 1);
			temp2[i + a][j + b] = dst_output[a][b];

			if (temp2[i + a][j + b] < 0) output[i + a][j + b] = 0;
			else if (temp2[i + a][j + b] > 255) output[i + a][j + b] = 255.0;
			else output[i + a][j + b] = dst_output[a][b];
		}
	}
}

void do_IDST(int i, int j)
{
	int a, b, c, d;
	double idst_output[N][N] = { 0, };
	for (a = 0; a<N; a++)
		for (b = 0; b<N; b++)
		{
			for (c = 0; c<N; c++)
				for (d = 0; d<N; d++)
				{
					idst_output[a][b] += (temp2[i + c][j + d])*sin(((c + 1)*(a + 1)*PI) / (N + 1))*sin(((d + 1)*(b + 1)*PI) / (N + 1));
				}

			idst_output[a][b] *= 2.0 / (N + 1);
			idst_output[a][b] += 128.0;

			output2[i + a][j + b] = idst_output[a][b];

			if (output2[i + a][j + b] < 0)	output[i + a][j + b] = 0;
			else if (output2[i + a][j + b] > 255) output[i + a][j + b] = 255.0;
			else output[i + a][j + b] = idst_output[a][b];
		}
}


void main(void)
{
	int  i, j;
	FILE  *in, *DFT, *IDFT, *DCT, *IDCT, *DST, *IDST;
	in = fopen("lena.img", "rb");

	if (in == NULL)
	{
		printf("File is not found!\n");
	}

	DFT = fopen("DFT.img", "wb");
	IDFT = fopen("IDFT.img", "wb");
	DCT = fopen("DCT.img", "wb");
	IDCT = fopen("IDCT.img", "wb");
	DST = fopen("DST.img", "wb");
	IDST = fopen("IDST.img", "wb");

	read_file(in);

	for (i = 0; i < SIZE; i += N) {
		for (j = 0; j < SIZE; j += N) {
			do_DCT(i, j);
		}
	}
	write_file(DCT);

	for (i = 0; i < SIZE; i += N) {
		for (j = 0; j < SIZE; j += N) {
			do_IDCT(i, j);
		}
	}
	write_file(IDCT);

	for (i = 0; i < SIZE; i += N) {
		for (j = 0; j < SIZE; j += N){

			do_DST(i, j);
		}
	}
	write_file(DST);

	for (i = 0; i < SIZE; i += N) {
		for (j = 0; j < SIZE; j += N) {
			do_IDST(i, j);
		}
	}
	write_file(IDST);

	in = fopen("lena256.raw", "rb");
	read_file_mini(in);

	do_DFT(0,0);
	write_file_mini(DFT);
	
	do_IDFT(0,0);
	write_file_mini(IDFT);

	fclose(DCT);
	fclose(IDCT);
	fclose(DST);
	fclose(IDST);
	fclose(DFT);
	fclose(IDFT);
}