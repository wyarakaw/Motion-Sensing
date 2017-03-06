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

echo ""
echo ">>>>EXECUTING extract_stride_data ON ACCELERATION DATASET<<<<"
./extract_stride_data \
	Acceleration_Walk_Dataset.csv \
	acceleration_x_output.csv \
	acceleration_y_output.csv \
	acceleration_z_output.csv \
	acceleration_x_strides.csv \
	acceleration_y_strides.csv \
	acceleration_z_strides.csv \
	$threshold

echo ""
echo "Lines in x accel and decel output.csv"
wc -l acceleration_x_output.csv
wc -l acceleration_y_output.csv
wc -l acceleration_z_output.csv

echo ""
echo "Tail of accel_x_output.csv"
tail acceleration_output.csv

echo ""
echo "Tail of accel_y_output.csv"
tail acceleration_y_output.csv

echo ""
echo "Tail of accel_z_output.csv"
tail acceleration_z_output.csv
