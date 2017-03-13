all: extract_stride_data train_neural_network test_neural_network

extract_stride_data: extract_stride_data.c
	gcc -Wall -o extract_stride_data extract_stride_data.c -lm -lmraa -lfann

train_neural_network: train_neural_network.c
	gcc -Wall -o train_neural_network train_neural_network.c -lmraa -lfann

test_neural_network: test_neural_network.c
	gcc -Wall -o test_neural_network test_neural_network.c -lmraa -lfann

clean:
	rm -rf extract_stride_data *~
	rm -rf train_neural_network *~
	rm -rf test_neural_network *~
	rm -rf training *~
	rm -rf testing *~
