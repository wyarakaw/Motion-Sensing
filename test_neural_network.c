#include <unistd.h>
#include <mraa/aio.h>
#include <stdio.h>
#include <string.h>
#include "floatfann.h"
#include "fann.h"
#include "extract_stride_data.h"

#define LINE_SIZE 61

void populate_pts(pts *neural_pts, char *ifile_name){
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int i, N_SAMPLES, rv;
	float *peak, *trough, *stride;

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
	N_SAMPLES = 0;
	while ((read = getline(&line, &len, fp)) != -1) {
		N_SAMPLES++;
	}
	
	neural_pts->size = N_SAMPLES;
	/* go back to the start of the file so that the data can be read */
	rewind(fp);

	/* start reading the data from the file into the data structures */
	i = 0;
	peak = (float *) malloc(sizeof(float) * N_SAMPLES);
	trough = (float *) malloc(sizeof(float) * N_SAMPLES);
	stride = (float *) malloc(sizeof(float) * N_SAMPLES);

	while ((read = getline(&line, &len, fp)) != -1) {
		/* parse the data */
		rv = sscanf(line, "%f\t%f\t%f\n", 
				&peak[i],
			        &trough[i],
			       	&stride[i]
			);

		if (rv != 3) {
			fprintf(stderr,
					"Failed to read line. Exiting\n"
			       );
			exit(EXIT_FAILURE);
		}
		i++;
	}

	neural_pts->peak = peak;
	neural_pts->trough = trough;
	neural_pts->stride = stride;
	fclose(fp);

	/*for (i=0; i<neural_pts->size; i++){
		printf("neural_pts.peak[%d] = %f\nneural_pts.trough[%d] = %f\nneural_pts.stride[%d] = %f\n",
				i, neural_pts->peak[i],
				i, neural_pts->trough[i],
				i, neural_pts->stride[i]
		      );
	}*/

}

int main(int argc, char** argv)
{	
    int i, j, speed;
    float max;			// comparison for results from fann_run
    fann_type *calc_out;	// will hold result for fann_run
    fann_type input[3];		// for 3 classifiers (in this case, peak, trough, and stride)
    struct fann *ann;		// what we will use to test our neural network
    pts neural_pts;		// buffer for peaks, troughs, and strides
    char *ifile_name;		// will hold string for input file

    ifile_name = argv[1];
    populate_pts(&neural_pts, ifile_name);
    
    ann = fann_create_from_file("TRAINING.net");

    for (i=0; i < neural_pts.size; i++) {
	max = -100;
	    
        input[0] = (float) neural_pts.peak[i];
        input[1] = (float) neural_pts.trough[i];
        input[2] = (float) neural_pts.stride[i];

	// calc_out returns an array of outputs, the number of whichbeing equal to number of neurons
	// in output layer
        calc_out = fann_run(ann, input);

        for (j = 0; j < 4; j++) {
	
            if (calc_out[j] > max) {
                max = calc_out[j];
                speed = j;
            }

        }
	
		
	printf("Peak, trough, and stride time values:: %f, %f, %f -> walking speed is %d\n", neural_pts.peak[i], neural_pts.trough[i], neural_pts.stride[i], speed);
    }

    fann_destroy(ann);
    return 0;
}
