#define _CRT_SECURE_NO_WARNINGS
#define X_SIZE 512
#define Y_SIZE 512
#define X_ZOOMOUT_SIZE 256
#define Y_ZOOMOUT_SIZE 256
#define X_ZOOMIN_SIZE 1024
#define Y_ZOOMIN_SIZE 1024 
#define PI 3.141592

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int in_data[X_SIZE][Y_SIZE];
int hpf_vertical_data[X_SIZE][Y_SIZE];
int hpf_horizontal_data[X_SIZE][Y_SIZE];
int low_pass_data[X_SIZE][Y_SIZE];
int zoom_in_data[X_ZOOMIN_SIZE][Y_ZOOMIN_SIZE];
int zoom_out_data[X_ZOOMOUT_SIZE][Y_ZOOMOUT_SIZE];
int translation_data[X_SIZE][Y_SIZE];
int rotation_data[X_SIZE][Y_SIZE];
int histogram_slide[X_SIZE][Y_SIZE];
int histogram_data[X_SIZE][Y_SIZE];
int histogram[512];
int sum_hist[512];

void Read_img(int data[X_SIZE][Y_SIZE], FILE *in)
{
	int  i, j;

	for (i = 0; i < Y_SIZE; i++)
		for (j = 0; j < X_SIZE; j++)
			data[j][i] = fgetc(in);
}

void Write_file_small_size(FILE *out, int data[X_ZOOMOUT_SIZE][Y_ZOOMOUT_SIZE])
{
	int  i, j;

	for (i = 0; i < Y_ZOOMOUT_SIZE; i++)
		for (j = 0; j < X_ZOOMOUT_SIZE; j++)
		{
			fputc(data[j][i], out);
		}
}

void Write_file(FILE *out, int data[X_SIZE][Y_SIZE])
{
	int  i, j;

	for (i = 0; i < Y_SIZE; i++)
		for (j = 0; j < X_SIZE; j++)
		{
			fputc(data[j][i], out);
		}
}

void Write_file_big_size(FILE *out, int data[X_ZOOMIN_SIZE][Y_ZOOMIN_SIZE])
{
	int  i, j;

	for (i = 0; i < Y_ZOOMIN_SIZE; i++)
		for (j = 0; j < X_ZOOMIN_SIZE; j++)
		{
			fputc(data[j][i], out);
		}
}

void do_zoom_in(int in_data[X_SIZE][Y_SIZE], int out_data[X_ZOOMIN_SIZE][Y_ZOOMIN_SIZE])
{
	int i, j;

	for (i = 0; i < X_ZOOMIN_SIZE; i++) {

		for (j = 0; j < Y_ZOOMIN_SIZE; j++) {

			out_data[i][j] = 0;
		}
	}

	for (i = 0; i < X_ZOOMIN_SIZE; i++)
	{
		for (j = 0; j < Y_ZOOMIN_SIZE; j++)
		{
			out_data[i][j] = in_data[i / (X_ZOOMIN_SIZE/X_SIZE)][j / (Y_ZOOMIN_SIZE/Y_SIZE)];
		}
	}
}

void do_zoom_out(int in_data[X_SIZE][Y_SIZE], int out_data[X_ZOOMOUT_SIZE][Y_ZOOMOUT_SIZE])
{
	int i, j;

	for (i = 0; i < X_ZOOMOUT_SIZE; i++) {

		for (j = 0; j < Y_ZOOMOUT_SIZE; j++) {

			out_data[i][j] = 0;
		}
	}

	for (i = 0; i < X_ZOOMOUT_SIZE; i++)
	{
		for (j = 0; j < Y_ZOOMIN_SIZE; j++)
		{
			out_data[i][j] = in_data[(X_SIZE/X_ZOOMOUT_SIZE) * i][(Y_SIZE/Y_ZOOMOUT_SIZE) * j];
		}
	}
}

void do_vertical_hpf(int in_data[X_SIZE][Y_SIZE], int out_data[X_SIZE][Y_SIZE])
{
	int i, j;
	int get_value = 0;
	for (i = 1; i < X_SIZE - 1; i++)
	{
		for (j = 1; j < Y_SIZE - 1; j++)
		{
			get_value = in_data[i - 1][j - 1] + (in_data[i - 1][j] * 2) + in_data[i][j + 1] - in_data[i + 1][j - 1] - (in_data[i + 1][j] * 2) - in_data[i + 1][j + 1];
			if (get_value < 0)
			{
				get_value = 0;
			}
			else if (get_value > 255)
			{
				get_value = 255;
			}
			out_data[i][j] = get_value;
		}
	}
}

void do_horizontal_hpf(int in_data[X_SIZE][Y_SIZE], int out_data[X_SIZE][Y_SIZE])
{
	int i, j;
	int get_value = 0;
	for (i = 1; i < X_SIZE - 1; i++)
	{
		for (j = 1; j < Y_SIZE - 1; j++)
		{
			get_value = in_data[i - 1][j - 1] + (in_data[i][j - 1] * 2) + in_data[i + 1][j - 1] - in_data[i - 1][j + 1] - (in_data[i][j + 1] * 2) - in_data[i + 1][j + 1];
			if (get_value < 0)
			{
				get_value = 0;
			}
			else if (get_value > 255)
			{
				get_value = 255;
			}
			out_data[i][j] = get_value;
		}
	}
}

void do_low_pass_filter(int in_data[X_SIZE][Y_SIZE], int out_data[X_SIZE][Y_SIZE])
{
	int i, j;
	int get_value = 0;
	for (i = 1; i < X_SIZE - 1; i++)
	{
		for (j = 1; j < Y_SIZE - 1; j++)
		{
			get_value =
				(
					in_data[i - 1][j - 1] + in_data[i][j - 1] + in_data[i + 1][j - 1] +
					in_data[i - 1][j] + in_data[i][j] + in_data[i + 1][j] +
					in_data[i - 1][j + 1] + in_data[i][j + 1] + in_data[i + 1][j + 1]
					) / 9;
			if (get_value < 0)
			{
				get_value = 0;
			}
			else if (get_value > 255)
			{
				get_value = 255;
			}
			out_data[i][j] = get_value;
		}
	}
}

void do_translation(int in_data[X_SIZE][Y_SIZE], int out_data[X_SIZE][Y_SIZE])
{
	int i, j;
	int x, y;

	for (i = 0; i < X_SIZE; i++)
	{
		for (j = 0; j < Y_SIZE; j++)
		{
			out_data[i / 2 + 50][j / 2 + 50] = in_data[i][j];
		}
	}
}

void do_rotation(int in_data[X_SIZE][Y_SIZE], int out_data[X_SIZE][Y_SIZE])
{
	int i, j; int right = 0, left = 0;
	double x, y;
	double theta;
	int x_t, y_t;
	int x_r, y_r;
	theta = (-0.5);

	for (i = 0; i < X_SIZE; i++) {

		for (j = 0; j < Y_SIZE; j++) {

			x_r = (int)(i / 2 + 20)  * cos(theta) - (j / 2 + 200) * sin(theta);
			y_r = (int)(i / 2 + 20) * sin(theta) + (j / 2 + 200) * cos(theta);
			out_data[x_r][y_r] = in_data[i][j];
		}
	}

	for (i = 1; i < X_SIZE-1; i++) {

		for (j = 1; j < Y_SIZE-1; j++) {

			if (out_data[i][j] == 0) {
				out_data[i][j] = (out_data[i + 1][j] + out_data[i - 1][j] + out_data[i][j + 1] + out_data[i][j - 1]) / 4;
			}
		}
	}
}

void do_histogram_slide_mapping(int in_data[X_SIZE][Y_SIZE], int slide_data[X_SIZE][Y_SIZE], int out_data[X_SIZE][Y_SIZE])
{
	int get_size[256];
	int get_accumulation[256];
	float get_color[256];
	int i, x, y;

	for (i = 0; i <= 255; i++)
	{
		get_size[i] = 0;
		for (x = 0; x < X_SIZE; x++)
		{
			for (y = 0; y < Y_SIZE; y++)
			{
				if (i == in_data[x][y])
				{
					get_size[i]++;
				}
			}
		}
	}

	for (x = 0; x <= 255; x++)
	{
		get_accumulation[x] = 0;
		for (y = 0; y <= 255; y++)
		{
			if (x >= y)
			{
				get_accumulation[x] += get_size[y];
			}
		}
		//printf("%d -> %d\n", get_accumulation[x], get_size[x]);
	}

	for (i = 0; i <= 255; i++)
	{
		for (x = 0; x < X_SIZE; x++)
		{
			for (y = 0; y < Y_SIZE; y++)
			{
				if (i == in_data[x][y])
				{
					slide_data[x][y] = get_accumulation[i];
				}
			}
		}
	}

	for (x = 0; x <= 255; x++)
	{
		get_color[x] = get_accumulation[x] * 255 / (X_SIZE * Y_SIZE);

		//printf("%d -> %lf\n", x, get_color[x]);
	}

	for (i = 0; i <= 255; i++)
	{
		for (x = 0; x < X_SIZE; x++)
		{
			for (y = 0; y < Y_SIZE; y++)
			{
				if (i == in_data[x][y])
				{
					out_data[x][y] = get_color[i];
				}
			}
		}
	}
}

int main(void)
{
	FILE *in = fopen("lena.img", "rb");
	FILE *zoom_out = fopen("zoom_out.img", "wb");
	FILE *zoom_in = fopen("zoom_in.img", "wb");
	FILE *hpf_v = fopen("hpf_vertical_lena.img", "wb");
	FILE *hpf_h = fopen("hpf_horizontal_lena.img", "wb");
	FILE *lpf = fopen("low_pass_filtering.img", "wb");
	FILE *translation = fopen("translation.img", "wb");
	FILE *rotation = fopen("rotation.img", "wb");
	FILE *slide = fopen("slide.img", "wb");
	FILE *histogram = fopen("histogram.img", "wb");

	if (in == NULL)
	{
		printf("File not found!!\n");
		return 0;
	}

	Read_img(in_data, in);

	do_zoom_out(in_data, zoom_out_data);
	Write_file_small_size(zoom_out, zoom_out_data);

	do_zoom_in(in_data, zoom_in_data);
	Write_file_big_size(zoom_in, zoom_in_data);

	do_low_pass_filter(in_data, low_pass_data);
	Write_file(lpf, low_pass_data);

	do_horizontal_hpf(in_data, hpf_horizontal_data);
	Write_file(hpf_h, hpf_horizontal_data);

	do_vertical_hpf(in_data, hpf_vertical_data);
	Write_file(hpf_v, hpf_vertical_data);

	do_translation(in_data, translation_data);
	Write_file(translation, translation_data);

	do_rotation(in_data, rotation_data);
	Write_file(rotation, rotation_data);

	do_histogram_slide_mapping(in_data, histogram_slide, histogram_data);
	Write_file(histogram, histogram_data);
	Write_file(slide, histogram_slide);


	fclose(in);
	system("pause");
}
