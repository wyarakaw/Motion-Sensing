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
echo ">>>>EXECUTING extract_stride_data ON 40s ACCELERATION DATASET<<<<"
./extract_stride_data \
	walk_speed_1_40s_29m_1.csv \
	accel_x_40s_29m.csv \
	accel_y_40s_29m.csv \
	accel_z_40s_29m.csv \
	accel_xs_40s_29m.csv \
	accel_xs_40s_29m.csv \
	accel_xs_40s_29m.csv \
	$threshold

echo ""
echo ">>>>EXECUTING extract_stride_data ON 30s ACCELERATION DATASET<<<<"
./extract_stride_data \
	walk_speed_1_30s_29m_1.csv \
	accel_x_30s_29m.csv \
	accel_y_30s_29m.csv \
	accel_z_30s_29m.csv \
	accel_xs_30s_29m.csv \
	accel_xs_30s_29m.csv \
	accel_xs_30s_29m.csv \
	$threshold

echo ""
echo ">>>>EXECUTING extract_stride_data ON 20s ACCELERATION DATASET<<<<"
./extract_stride_data \
	walk_speed_1_20s_29m_1.csv \
	accel_x_20s_29m.csv \
	accel_y_20s_29m.csv \
	accel_z_20s_29m.csv \
	accel_xs_20s_29m.csv \
	accel_xs_20s_29m.csv \
	accel_xs_20s_29m.csv \
	$threshold

echo ""
echo ">>>>EXECUTING extract_stride_data ON 10s ACCELERATION DATASET<<<<"
./extract_stride_data \
	walk_speed_1_10s_29m_1.csv \
	accel_x_10s_29m.csv \
	accel_y_10s_29m.csv \
	accel_z_10s_29m.csv \
	accel_xs_10s_29m.csv \
	accel_xs_10s_29m.csv \
	accel_xs_10s_29m.csv \
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
