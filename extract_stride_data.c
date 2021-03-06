/* for file and terminal I/O */
#include <stdio.h>
/* for string manip */
#include <string.h>
/* for exit() */
#include <stdlib.h>
/* for fabsf() */
#include <math.h>
#include <unistd.h>
#include "floatfann.h"
#include "fann.h"
#include "extract_stride_data.h"

#define BUFF_SIZE 1024
#define NORM_CONSTANT 10

/*
 * sets first <n> values in <*arr> to <val>
 */
void clear_buffer(float *arr, float val, int n) 
{
	int i;
	for (i = 0; i < n; i++) {
		arr[i] = val;
	}
}

/*
 * Calculates mean of first <n> samples in <*arr>
 */
float calculate_mean(float *arr, int n)
{
	float total;
	int i;

	total = 0.0f;
	for (i = 0; i < n; i++) {
		total += arr[i];
	}

	return total/((float) n);
}

int 
find_peaks_and_troughs(
		float *arr, 	// signal 
		int n_samples, 	// number of samples present in the signal
		float E, 	// threshold for peak detection
		// arrays that will store the indicies of the located
		// peaks and troughs
		float *P, float *T,
		// number of peaks (n_P) and number of troughs (n_T)
		// found in the data set *arr
		int *n_P, int *n_T
		)
{
	int a, b, i, d, _n_P, _n_T;

	i = -1; d = 0; a = 0; b = 0;
	_n_P = 0; _n_T = 0;

	clear_buffer(P, 0.0f, n_samples);
	clear_buffer(T, 0.0f, n_samples);

	while (i != n_samples) {
		i++;
		if (d == 0) {
			if (arr[a] >= (arr[i] + E)) {
				d = 2;
			} else if (arr[i] >= (arr[b] + E)) {
				d = 1;
			}
			if (arr[a] <= arr[i]) {
				a = i;
			} else if (arr[i] <= arr[b]) {
				b = i;
			}
		} else if (d == 1) {
			if (arr[a] <= arr[i]) {
				a = i;
			} else if (arr[a] >= (arr[i] + E)) {
				/*
				 * Peak has been detected.
				 * Add index at detected peak
				 * to array of peak indicies
				 * increment count of peak indicies
				 */
				P[_n_P] = a;
				_n_P++;
				b = i;
				d = 2;
			}
		} else if (d == 2) {
			if (arr[i] <= arr[b]) {
				b = i;
			} else if (arr[i] >= (arr[b] + E)) {
				/*
				 * Trough has been detected.
				 * Add index at detected trough
				 * to array of trough indicies
				 * increment count of trough indicies
				 */
				T[_n_T] = b;
				_n_T++;
				a = i;
				d = 1;
			}
		}
	}

	(*n_P) = _n_P;
	(*n_T) = _n_T;
	return 0;
}


/*
 * HOW A STRIDE IS DEFINED FOR OUR ALGORITHM:
 * We defined a stride as the time it takes for the 9DOF to travel from one trough
 * to another.
 */
void collect_trough_data(
		FILE *fp, 		// file we want to output to
		char *ofile_name,	// name of output file
		float *t,		// buffer for times
		int n_T,		// number of troughs
		float *T_i		// indexes of troughs
		)
{
	int i, n_T_adjusted, t_index1, t_index2;

	/* 
	 * We check if we have an odd number of troughs; if this is the case, we ignore the last
 	 * trough when calculating the time for strides. Notice also that with an even number of 
 	 * troughs, we will have an odd number of sampled stride times (e.g. 10 troughs mean 9 
 	 * trough to trough values). If we have an odd number of troughs, we ignore the last
 	 * trough from our calculation of stride times.
 	 */
	if (n_T % 2 == 1)
		n_T_adjusted = n_T - 2;
	else
		n_T_adjusted = n_T - 1;

	printf("n_T = %d, n_T_adjusted = %d\n", n_T, n_T_adjusted);

	float accel_strides_t[n_T_adjusted];

	for (i = 0; i < n_T_adjusted; i++){
		t_index1 = (int) T_i[i];
		t_index2 = (int) T_i[i+1];

		//printf("t_index1: t[%d] = %20.10lf\n", t_index_accel1, t[t_index_accel1]);
		//printf("t_index2: t[%d] = %20.10lf\n", t_index_accel2, t[t_index_accel2]);

		accel_strides_t[i] = t[t_index2] - t[t_index1];
	}

	/* Open our file for writing */
	fp = fopen(ofile_name, "w");
	if (fp == NULL) {
		fprintf(stderr, 
				"Failed to write to file \'%s\'.\n", 
				ofile_name
		       );
		exit(EXIT_FAILURE);
	}

	printf("Number of troughs in data sheet: = %d\n", n_T_adjusted);
	for (i = 0; i < n_T_adjusted; i++){
		
		printf("Time between trough %d and trough %d: %20.10lf\n", i+1, i+2, accel_strides_t[i]);
		fprintf(fp, "%20.10lf\n", accel_strides_t[i]);
		//printf("x_accel_strides_t[%d] = %20.10lf\n", i, x_accel_strides_t[i]);
	}

	fprintf(fp, "Mean:\n%20.10lf", calculate_mean(accel_strides_t, n_T_adjusted-1));

	fclose(fp);
	printf("\n");
}

void write_to_file(
		FILE *fp, char *ofile_name,	// file we want to output to
		int n_P, int n_T, 		// number of peaks and troughs
		float *t, 			// buffer for times
		float *accel,			// buffer holding acceleration values
		float *P_i, float *T_i		// buffer for peak and trough indexes
		)
{
	int i, idx;	// generic variables

	/* open the output file to write the peak and trough data */
	printf("Attempting to write to file \'%s\'.\n", ofile_name);
	fp = fopen(ofile_name, "w");
	if (fp == NULL) {
		fprintf(stderr, 
				"Failed to write to file \'%s\'.\n", 
				ofile_name
		       );
		exit(EXIT_FAILURE);
	}

	//fprintf(fp1, "P_i,P_t,P_x,T_i,T_t,T_p\n");
	for (i = 0; i < n_P || i < n_T; i++) {
		/* Only peak data if there is peak data to write */
		if (i < n_P) {
			idx = (int) P_i[i];
			fprintf(fp, "%d,%20.10lf,%lf,",
					idx,
					t[idx],
					accel[idx]
			       );
		} else {
			fprintf(fp, ",,,");
		}
		/* Only trough data if there is trough data to write */
		if (i < n_T) {
			idx = (int) T_i[i];
			fprintf(fp, "%d,%20.10lf,%lf\n",
					idx,
					t[idx],
					accel[idx]
			       );
		} else {
			fprintf(fp, ",,\n");
		}
	}
	fclose(fp);
}

void create_t_file(
		FILE *fp, char *ofile_name,	// file we want to output to
		int n_P, int n_T, 		// number of peaks and troughs
		float *t, 			// buffer for times
		float *accel,			// buffer for acceleartions
		float *P_i, float *T_i,		// buffer for peak and trough indexes
		int speed_file,			// for +1 and -1 outputs
		int test_flag,			// 0 for training file, 1 for testing file
		pts *neural_pts			// to hold our pts values
		)
{
	int i, 			
	    p_idx, t_idx, 	// holds indexes to peaks and troughs
	    t_time1, t_time2,	// to help calculate stride times
	    NUM_SAMPLES,	// each peak should be paired with a trough
	    n_T_adjusted;	// should have an even number of troughs

	int outputs[4];		// our output neurons; +1 for correct speed, -1 for incorrect speed
	
	/* open the output file to write the peak and trough data */
	printf("Attempting to write to file \'%s\'.\n", ofile_name);
	fp = fopen(ofile_name, "w");
	if (fp == NULL) {
		fprintf(stderr, 
				"Failed to write to file \'%s\'.\n", 
				ofile_name
		       );
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < 4; i++){
		outputs[i] = -1;
	}
	outputs[speed_file - 1] = 1;

	if (n_T % 2 == 1)
		n_T_adjusted = n_T - 2;
	else
		n_T_adjusted = n_T - 1;

	float accel_strides_t[n_T_adjusted];

	for (i = 0; i < n_T_adjusted; i++){
		t_time1 = (int) T_i[i];
		t_time2 = (int) T_i[i+1];
		accel_strides_t[i] = t[t_time2] - t[t_time1];
	}

	if (n_P < n_T)
		NUM_SAMPLES = n_P;
	else
		NUM_SAMPLES = n_T;

	float peak_array[NUM_SAMPLES];
	float trough_array[NUM_SAMPLES];
	float stride_array[NUM_SAMPLES];

	if (test_flag == 0){
		for (i = 0; i < 4; i++){
			outputs[i] = -1;
		}
		outputs[speed_file - 1] = 1;

		/* Initialize our pts struct as defined earlier */
		neural_pts->peak = peak_array;
		neural_pts->trough = trough_array;
		neural_pts->stride = stride_array;
		neural_pts->size = NUM_SAMPLES;
	}

	/* We start at i = 1 because we defined strides as trough-to-trough. */
	for (i = 1; i < NUM_SAMPLES; i++) {
		p_idx = (int) P_i[i];
		t_idx = (int) T_i[i];

		/*
		 * Filter out irregular data samples (i.e. incorrect normalization values).
		 * Once irregular data is found, continue to next iteration. 
		 * */
		if (abs(accel[p_idx]/NORM_CONSTANT) >= 1 ||
				abs(accel[t_idx]/NORM_CONSTANT) >= 1 ||
				abs(accel_strides_t[i-1]/NORM_CONSTANT >= 1))
			continue;
		
		/* Divide each value by 10 for normalization purposes (between -1 and 1) */
		fprintf(fp, "%f\t%f\t%f\n",
				accel[p_idx]/NORM_CONSTANT,
				accel[t_idx]/NORM_CONSTANT,
				accel_strides_t[i-1]/NORM_CONSTANT
		       );

		if (test_flag == 0){
			neural_pts->peak[i-1] = accel[p_idx]/NORM_CONSTANT;
			neural_pts->trough[i-1] = accel[t_idx]/NORM_CONSTANT;
			neural_pts->stride[i-1] = accel_strides_t[i-1]/NORM_CONSTANT;

			fprintf(fp, "%d\t%d\t%d\t%d\n",
					outputs[0],
					outputs[1],
					outputs[2],
					outputs[3]
				);
		}
	}
	/*for (i=0; i<neural_pts.size; i++){
		printf("neural_pts.peak[%d] = %f\nneural_pts.trough[%d] = %f\nneural_pts.stride[%d] = %f\n",
				i, neural_pts.peak[i],
				i, neural_pts.trough[i],
				i, neural_pts.stride[i]
		      );
	}*/
	fclose(fp);
}


int main(int argc, char **argv)
{
	/* Generic variables */
	int i;// idx;
	int rv1, rv2, rv3;
	/* Variables for reading file line by line */
	char *ifile_name, *ofile_pt_x_name, *ofile_pt_y_name, *ofile_pt_z_name,
	     *ofile_sx_name, *ofile_sy_name, *ofile_sz_name, *ofile_training,
	     *ofile_testing;
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int N_SAMPLES;

	/* 
	 * Correlates to which speed file we are analyzing, i.e.
	 * speed_file = 1 for 10s
	 * speed_file = 2 for 20s
	 * speed_file = 3 for 30s
	 * speed_file = 4 for 40s;
	 */
	int speed_file;

	/* Variables for storing the data and storing the return values */
	float *t, *t_start_epoch, *t_end_epoch,
	      *accel_x, *accel_y, *accel_z,
	      *gyro_x, *gyro_y, *gyro_z; 	// variables for data collected from input file
	float pk_threshold;	// pk-threshold value
       	/* Variables for peak-trough detection */	
	float *P_i_ax, *P_i_ay, *P_i_az; 	// indicies of each peak found by peak detection
	float *T_i_ax, *T_i_ay, *T_i_az; 	// indicies of each trough found by trough detection
	int n_P_ax, n_P_ay, n_P_az; 	// number of peaks
	int n_T_ax, n_T_ay, n_T_az; 	// number of troughs
	
	/*if (argc != 9)
       	{
		ifile_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(ifile_name, 0, BUFF_SIZE);
		snprintf(ifile_name, 
				BUFF_SIZE, 
				"Acceleration_Walk_Dataset.csv"
			);
		ofile_pt_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(ofile_pt_name, 0, BUFF_SIZE);
		snprintf(ofile_pt_name, BUFF_SIZE, "acceleration_output.csv");
		ofile_st_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(ofile_st_name, 0, BUFF_SIZE);
		snprintf(ofile_st_name, BUFF_SIZE, "acceleration_strides.csv");
		pk_threshold = 6.7;
	}*/ 

	ifile_name = argv[1];
	ofile_pt_x_name = argv[2];
	ofile_pt_y_name = argv[3];
	ofile_pt_z_name = argv[4];
	ofile_sx_name = argv[5];
	ofile_sy_name = argv[6];
	ofile_sz_name = argv[7];
	pk_threshold = atof(argv[8]);
	speed_file = atoi(argv[9]);
	ofile_training = argv[10];
	ofile_testing = argv[11];

	printf("Arguments used:\n\t%s=%s\n\t%s=%s\n\t%s=%s\n\t%s=%s\n\t%s=%s\n\t%s=%s\n\t%s=%s\n\t%s=%f\n\t%s=%d\n\t%s=%s\n\t%s=%s\n\n",
			"ifile_name", ifile_name,
			"ofile_pt_x_name", ofile_pt_x_name,
			"ofile_pt_y_name", ofile_pt_y_name,
			"ofile_pt_z_name", ofile_pt_z_name,
			"ofile_sx_name", ofile_sx_name,
			"ofile_sy_name", ofile_sy_name,
			"ofile_sz_name", ofile_sz_name,
			"pk_threshold", pk_threshold,
			"speed_file", speed_file,
			"ofile_training", ofile_training,
			"ofile_testing", ofile_testing
	      );

	/* open the input file */
	printf("Attempting to read from file \'%s\'.\n\n", ifile_name);
	fp = fopen(ifile_name, "r");
	if (fp == NULL) {
		fprintf(stderr, 
				"Failed to read from file \'%s\'.\n", 
				ifile_name
		       );
		exit(EXIT_FAILURE);
	}

	/* count the number of lines in the file */
	read = getline(&line, &len, fp); //discard header of file
	N_SAMPLES = 0;
	while ((read = getline(&line, &len, fp)) != -1) {
		N_SAMPLES++;
	}

	/* go back to the start of the file so that the data can be read */
	rewind(fp);
	read = getline(&line, &len, fp); //discard header of file

	/* start reading the data from the file into the data structures */
	i = 0;
	t = (float *) malloc(sizeof(float) * N_SAMPLES);
	t_start_epoch = (float *) malloc(sizeof(float) * N_SAMPLES);
	t_end_epoch = (float *) malloc(sizeof(float) * N_SAMPLES);
	accel_x = (float *) malloc(sizeof(float) * N_SAMPLES);
	accel_y = (float *) malloc(sizeof(float) * N_SAMPLES);
	accel_z = (float *) malloc(sizeof(float) * N_SAMPLES);
	gyro_x = (float *) malloc(sizeof(float) * N_SAMPLES);
	gyro_y = (float *) malloc(sizeof(float) * N_SAMPLES);
	gyro_z = (float *) malloc(sizeof(float) * N_SAMPLES);

	while ((read = getline(&line, &len, fp)) != -1) {
		/* parse the data */
		rv1 = sscanf(line, "%f,%f,%f,%f,%f,%f,%f,%f,%f\n", 
				&t[i],
			        &t_start_epoch[i],
			       	&t_end_epoch[i],
			       	&accel_x[i], &accel_y[i], &accel_z[i],
		   		&gyro_x[i], &gyro_y[i], &gyro_z[i]);	

		if (rv1 != 9) {
			fprintf(stderr,
					"%s %d \'%s\'. %s.\n",
					"Failed to read line",
					i,
					line,
					"Exiting"
			       );
			exit(EXIT_FAILURE);
		}
		i++;
	}
	fclose(fp);

	/* 
	 * From selected thresholds, 
	 * find indicies of peaks for seperate accelerations
	 * find indicies of troughs for seperate accelerations
	 */
	P_i_ax = (float *) malloc(sizeof(float) * N_SAMPLES);
	P_i_ay = (float *) malloc(sizeof(float) * N_SAMPLES);
	P_i_az = (float *) malloc(sizeof(float) * N_SAMPLES);

	T_i_ax = (float *) malloc(sizeof(float) * N_SAMPLES);
	T_i_ay = (float *) malloc(sizeof(float) * N_SAMPLES);
	T_i_az = (float *) malloc(sizeof(float) * N_SAMPLES);
	
	rv1 = find_peaks_and_troughs(
			accel_x, 
			N_SAMPLES, 
			pk_threshold, 
			P_i_ax, T_i_ax, 
			&n_P_ax, &n_T_ax);

	rv2 = find_peaks_and_troughs(
			accel_y, 
			N_SAMPLES, 
			pk_threshold, 
			P_i_ay, T_i_ay, 
			&n_P_ay, &n_T_ay);

	rv3 = find_peaks_and_troughs(
			accel_z, 
			N_SAMPLES, 
			pk_threshold, 
			P_i_az, T_i_az, 
			&n_P_az, &n_T_az);

	if (rv1 < 0) {
		fprintf(stderr, "rv1: find_peaks_and_troughs failed\n");
		exit(EXIT_FAILURE);
	} else if (rv2 < 0) {
		fprintf(stderr, "rv2: find_peaks_and_troughs failed\n");
		exit(EXIT_FAILURE);
	} else if (rv3 < 0) {
		fprintf(stderr, "rv3: find_peaks_and_troughs failed\n");
		exit(EXIT_FAILURE);
	}


	printf("TROUGH DATA FOR X ACCELERATION:\n");
	collect_trough_data(
		fp,
		ofile_sx_name,
		t,
		n_T_ax,
		T_i_ax);

	printf("TROUGH DATA FOR Y ACCELERATION:\n");
	collect_trough_data(
		fp,
		ofile_sy_name,
		t,
		n_T_ay,
		T_i_ay);

	printf("TROUGH DATA FOR Z ACCELERATION:\n");
	collect_trough_data(
		fp,
		ofile_sz_name,
		t,
		n_T_az,
		T_i_az);


	/* Writing to output file for x-acceleration */
	write_to_file(fp, ofile_pt_x_name,
	       	n_P_ax, n_T_ax,
	       	t,
		accel_x,
		P_i_ax, T_i_ax);	

	/* Writing to output file for y-acceleration */
	write_to_file(fp, ofile_pt_y_name,
	       	n_P_ay, n_T_ay,
	       	t,
		accel_y,
		P_i_ay, T_i_ay);

	/* Writing to output file for z-acceleration */
	write_to_file(fp, ofile_pt_z_name,
		       	n_P_az, n_T_az,
		       	t,
			accel_z,
			P_i_az, T_i_az);

	/* Create the x-acceleration testing file for our neural network */
	pts neural_pts;
	create_t_file(fp, ofile_training,
			n_P_ax, n_T_ax,
			t,
			accel_x,
			P_i_ax, T_i_ax,
			speed_file,
			0,
			&neural_pts);

	create_t_file(fp, ofile_testing,
			n_P_ax, n_T_ax,
			t,
			accel_x,
			P_i_ax, T_i_ax,
			speed_file,
			1,
			&neural_pts);
	// lOOK AT YOUR INPUT FILE; MISSING HEADER, PUT THAT IN

	printf("extract_stride_data completed successfully. Exiting.\n");

	return 0;
}
