#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mraa/i2c.h>
#include "LSM9DS0.h"
#include <sys/time.h>
#include <signal.h>

#define MILLION 1000000.0 // 1,000,000.0

sig_atomic_t volatile run_flag = 1;

/* SIGINT, for stopping code when ctrl-C is pushed */
void do_when_interrupted(int sig){
	if (sig == SIGINT)
		run_flag = 0;
}

int main(int argc, char** argv) {

	/* Variables for 9DOF */
	mraa_i2c_context accel, gyro;
	float a_res, g_res;
	data_t ad, gd;

	/* For calculating times */
	struct timeval start, end, start_prog;
	double start_epoch, end_epoch, start_prog_epoch;

	/* For our output csv file */
	FILE *fp;
	char *file_name;

	if (argc != 2){
		fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	file_name = argv[1];
	fp = fopen(file_name, "w");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open file \'%s\'. Exiting.\n", file_name);
		exit(EXIT_FAILURE);
	}

	/* Set accelerometer parameters, scale and resolution */
	accel = accel_init();
	set_accel_scale(accel, A_SCALE_16G);	// Check for clipping
	//set_accel_ODR(accel, A_ODR_160);	// ODR = output data rate
	a_res = calc_accel_res(A_SCALE_16G);

	/* Set gyroscope parameters, scale and resolution */
	gyro = gyro_init();
	set_gyro_scale(gyro, G_SCALE_500DPS);
	//set_gyro_ODR(gyro, G_ODR_760_BW_100);
	g_res = calc_gyro_res(G_SCALE_500DPS);

	signal(SIGINT, &do_when_interrupted);

	/* Starting time for recording our data (to obtain sampling time) */
	gettimeofday(&start_prog, NULL);
	start_prog_epoch = start_prog.tv_sec + start_prog.tv_usec/MILLION;

	/* Start collecting data */
	while (run_flag) {

		gettimeofday(&start, NULL);	// timestamp_before

		ad = read_accel(accel, a_res);
		gd = read_gyro(gyro, g_res);

		gettimeofday(&end, NULL);	// timestamp_after

		start_epoch = start.tv_sec + start.tv_usec/MILLION;
		end_epoch = end.tv_sec + end.tv_usec/MILLION;

		fprintf(fp, "%10.6lf, %10.6lf, %10.6lf, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f\n",
			       	end_epoch - start_prog_epoch,
				start_epoch,
				end_epoch,
			       	ad.x, ad.y, ad.z,
			       	gd.x, gd.y, gd.z);

		usleep(5000);
		
		//printf("Current time is: %10.6lf\n", end_epoch - start_prog_epoch);
	}

	fclose(fp);

	return 0;
}
