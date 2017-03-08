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
	$t40s \
	training_40s.txt \
	$threshold \
	training2.txt
	

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
	$t30s \
	training_30s.txt \
	$threshold \
	training2.txt

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
	$t20s \
	training_20s.txt \
	$threshold \
	training2.txt

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
	$t10s \
	training_10s.txt \
	$threshold \
	training2.txt

echo ""
echo "Lines in x accel and decel output.csv"
wc -l acceleration_x_output.csv
wc -l acceleration_y_output.csv
wc -l acceleration_z_output.csv

