#ifndef EXTRACT_STRIDE_DATA_H_ 
#define EXTRACT_STRIDE_DATA_H_

/*
 * This data structure will hold three arrays, with the ith index corresponding to a set of
 * peak time, trough time, and stride time. This information will be enterd to test the neural
 * network once the neural network has been trained.
 */
typedef struct {
	float *peak;
	float *trough;
	float *stride;
	int size;
} pts;

/* Finds peaks and troughs, populates them into *P and *T */
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
		);

/* 
 * Populates our buffer for neural network, which holds buffers for peaks troughs, and 
 * strides. 
 */
void populate_pts(pts *neural_pts, char *ifile_name);

/*
 * Collect trough data in order to calculate stride times, along with mean of stride times.
 * We defined a stride as the time it takes for the 9DOF to travel from one trough
 * to another.
 */
void collect_trough_data(
		FILE *fp, 		// file we want to output to
		char *ofile_name,	// name of output file
		float *t,		// buffer for times
		int n_T,		// number of troughs
		float *T_i		// indexes of troughs
		);

/* 
 * Create file for peak and trough values, in the format:
 * P_i,P_t,P_x,T_i,T_t,T_p
 */
void write_to_file(
		FILE *fp, char *ofile_name,	// file we want to output to
		int n_P, int n_T, 		// number of peaks and troughs
		float *t, 			// buffer for times
		float *accel,			// buffer holding acceleration values
		float *P_i, float *T_i		// buffer for peak and trough indexes
		);

/* 
 * Used to create our training file and testing file, and also to populate our pts class.
 * To choose between training or testing file:
 * 	test_flag = 0 --> Create training file
 *	test_flag = 1 --> Create testing file
 */
void create_t_file(
		FILE *fp, char *ofile_name,	// file we want to output to
		int n_P, int n_T, 		// number of peaks and troughs
		float *t, 			// buffer for times
		float *accel,			// buffer for acceleartions
		float *P_i, float *T_i,		// buffer for peak and trough indexes
		int speed_file,			// for +1 and -1 outputs
		int test_flag,			// for creating test files for neural network
		pts *neural_pts			// to hold our pts values
		);



#endif // EXTRACT_STRIDE_DATA_H_
