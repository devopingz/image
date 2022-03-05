#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.141592
#define N 8
#define SIZE 512

unsigned char input[SIZE][SIZE];
double temp[SIZE][SIZE];
double output2[SIZE][SIZE];
unsigned char dct_output[SIZE][SIZE];
unsigned char idct_output[SIZE][SIZE];
unsigned char q_output[SIZE][SIZE];
unsigned char iq_output[SIZE][SIZE];
double temp2[SIZE][SIZE];
double temp3[SIZE][SIZE];
double temp4[SIZE][SIZE];

/*int q_table[8][8] = { 16, 11, 10, 16, 24,  40,  51,  61,
12, 12, 14, 19, 26,  58,  60,  55,
14, 13, 16, 24, 40,  57,  69,  56,
14, 17, 22, 29, 51,  87,  80,  62,
18, 22, 37, 56, 68,  109, 103, 77,
24, 35, 55, 64, 81,  104, 113, 92,
49, 64, 78, 87, 103, 121, 120, 101,
72, 92, 95, 98, 112, 100, 103, 99 };*/

void read_file(FILE *in, unsigned char input[SIZE][SIZE])
{
	int i, j;
	for (i = 0; i < SIZE; i++)
		for (j = 0; j < SIZE; j++)
			input[i][j] = fgetc(in);
}

void write_file(FILE *out, unsigned char out_data[SIZE][SIZE])
{
	int  i, j;
	for (i = 0; i < SIZE; i++)
		for (j = 0; j < SIZE; j++)
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

					temp_out[a][b] += au * av*(input[i + c][j + d] - 128)*cos(((2 * c + 1)*a*PI) / (2 * N))*cos(((2 * d + 1)*b*PI) / (2 * N));
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

					temp_output[a][b] += au * av*(temp[i + c][j + d])*cos(((2 * a + 1)*c*PI) / (2 * N))*cos(((2 * b + 1)*d*PI) / (2 * N));
				}

			temp_output[a][b] += 128.0;
			output2[i + a][j + b] = temp_output[a][b];

			if (output2[i + a][j + b] < 0)	idct_output[i + a][j + b] = 0;
			else if (output2[i + a][j + b] > 255) idct_output[i + a][j + b] = 255.0;
			else idct_output[i + a][j + b] = temp_output[a][b];
		}
	}
}

void Q(int i, int j)
{
	int a, b;
	for (a = 0; a < N; a++)
		for (b = 0; b < N; b++)
		{
			temp[i + a][j + b] = round(temp[i + a][j + b] / 139.95);

			if (temp[i + a][j + b] < 0)
				q_output[i + a][j + b] = 0;
			else if (temp[i + a][j + b] > 255)
				q_output[i + a][j + b] = 255;
			else
				q_output[i + a][j + b] = temp[i + a][j + b];
		}
}

void IQ(int i, int j)
{
	int a, b;
	for (a = 0; a < N; a++)
		for (b = 0; b < N; b++)
		{
			temp[i + a][j + b] = temp[i + a][j + b] * 139.95;

			if (temp[i + a][j + b] < 0)
				iq_output[i + a][j + b] = 0;
			else if (temp[i + a][j + b] > 255)
				iq_output[i + a][j + b] = 255;
			else
				iq_output[i + a][j + b] = temp[i + a][j + b];
		}
}


double MSE(unsigned char a[512][512], unsigned char b[512][512]) {

	int i, j;
	double result, sum = 0;

	for (i = 0; i < SIZE; i++) {

		for (j = 0; j < SIZE; j++) {

			if (a[i][j] == b[i][j] + 1) b[i][j] += 1;
		//	printf("%d %d\n", a[i][j], b[i][j]);
			sum += (a[i][j] - b[i][j])*(a[i][j] - b[i][j]);
		}
	}

	result = (double)sum / (double)(SIZE*SIZE);
	return result;
}

void PSNR(unsigned char a[512][512], unsigned char b[512][512]) {

	double M, psnr;

	M = MSE(a, b);
	if (M == 0) printf("\n\n	PSNR === infinity \n\n");
	else {

		psnr = 10 * log10((255 * 255) / M);
		printf("\n\n	PSNR === %.2lf\n\n", psnr);
	}
}

void main(void)
{
	int  i, j;
	FILE  *in, *DCT, *IDCT, *Q_IDCT;
	in = fopen("lena.img", "rb");

	if (in == NULL)
	{
		printf("File is not found!\n");
	}

	DCT = fopen("DCT.img", "wb");
	IDCT = fopen("IDCT.img", "wb");
	Q_IDCT = fopen("Q_IDCT.img", "wb");

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
	write_file(IDCT, idct_output);

	PSNR(input, idct_output);

	for (i = 0; i < SIZE; i += N) {
		for (j = 0; j < SIZE; j += N) {
			Q(i, j);
		}
	}

	for (i = 0; i < SIZE; i += N) {
		for (j = 0; j < SIZE; j += N) {
			IQ(i, j);
		}
	}

	for (i = 0; i < SIZE; i += N) {
		for (j = 0; j < SIZE; j += N) {
			do_IDCT(i, j);
		}
	}
	write_file(Q_IDCT, idct_output);

	PSNR(input, idct_output);

	fclose(DCT);
	fclose(IDCT);
	fclose(Q_IDCT);

	system("pause");
}