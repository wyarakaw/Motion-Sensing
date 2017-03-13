#!/bin/bash

if [ $1 ]; then
		threshold=$1
else
		threshold=6
fi

echo ""
echo ">>>>COMPILING<<<<"
make clean
make

t40s=4;
t30s=3;
t20s=2;
t10s=1;

NUM_INPUT_NEURONS=3;
NUM_OUTPUT_NEURONS=4;

echo ""
echo ">>>>EXECUTING extract_stride_data ON 40s ACCELERATION DATASET<<<<"
./extract_stride_data \
	walk_speed_1_40s_29m_2.csv \
	accel_x_40s_29m.csv \
	accel_y_40s_29m.csv \
	accel_z_40s_29m.csv \
	accel_xs_40s_29m.csv \
	accel_ys_40s_29m.csv \
	accel_zs_40s_29m.csv \
	$threshold \
	$t40s \
	training_40s.txt \
	testing_40s.txt

echo ""
echo ">>>>EXECUTING extract_stride_data ON 30s ACCELERATION DATASET<<<<"
./extract_stride_data \
	walk_speed_1_30s_29m_2.csv \
	accel_x_30s_29m.csv \
	accel_y_30s_29m.csv \
	accel_z_30s_29m.csv \
	accel_xs_30s_29m.csv \
	accel_ys_30s_29m.csv \
	accel_zs_30s_29m.csv \
	$threshold \
	$t30s \
	training_30s.txt \
	testing_30s.txt

echo ""
echo ">>>>EXECUTING extract_stride_data ON 20s ACCELERATION DATASET<<<<"
./extract_stride_data \
	walk_speed_1_20s_29m_2.csv \
	accel_x_20s_29m.csv \
	accel_y_20s_29m.csv \
	accel_z_20s_29m.csv \
	accel_xs_20s_29m.csv \
	accel_ys_20s_29m.csv \
	accel_zs_20s_29m.csv \
	$threshold \
	$t20s \
	training_20s.txt \
	testing_20s.txt

echo ""
echo ">>>>EXECUTING extract_stride_data ON 10s ACCELERATION DATASET<<<<"
./extract_stride_data \
	walk_speed_1_10s_29m_3.csv \
	accel_x_10s_29m.csv \
	accel_y_10s_29m.csv \
	accel_z_10s_29m.csv \
	accel_xs_10s_29m.csv \
	accel_ys_10s_29m.csv \
	accel_zs_10s_29m.csv \
	$threshold \
	$t10s \
	training_10s.txt \
	testing_10s.txt

echo ""
echo ">>>>COMBINING TRAINING AND TESTING FILES<<<<"
cat training_10s.txt training_20s.txt training_30s.txt training_40s.txt > training_t.txt
temp=$(expr "$(wc -l < training_t.txt)" / 2)
echo $temp $NUM_INPUT_NEURONS $NUM_OUTPUT_NEURONS > header.txt
cat header.txt training_t.txt >> training.txt
rm training_t.txt header.txt
cat testing_10s.txt testing_20s.txt testing_30s.txt testing_40s.txt >> testing.txt

echo ""
echo ">>>TRAINING THE NEURAL NETWORK<<<<"
./train_neural_net

echo ""
echo ">>>TESTING THE NEURAL NETWORK<<<<"
./test_neural_network \
	testing.txt
