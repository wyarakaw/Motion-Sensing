all: extract_stride_data

extract_stride_data: extract_stride_data.c
	gcc -Wall -o extract_stride_data extract_stride_data.c -lm -lmraa -lfann

clean:
	rm -rf extract_stride_data *~
