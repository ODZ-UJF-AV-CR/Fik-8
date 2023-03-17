#!/bin/bash

#mount UUID=... /mnt

dirname=/home/pi/Documents/Fik8/data
newnum=$(ls -ld $dirname/run_* 2>/dev/null |wc -l)

tpx_data_dir=$dirname/run_${newnum}/Timepix
lbd_data_dir=$dirname/run_${newnum}/Labdos
spx_data_dir=$dirname/run_${newnum}/SpacePix3

mkdir -p $tpx_data_dir
mkdir -p $lbd_data_dir
mkdir -p $spx_data_dir

chown root $tpx_data_dir
chown root $lbd_data_dir
chown root $spx_data_dir

unix=$(date '+%s')
lbd_data_file=$lbd_data_dir/labdos_$unix
spx_data_file=$spx_data_dir/

echo $tpx_data_dir

/home/pi/Documents/Fik8/scripts/RP_manager/fik8_run_manager.sh 100 2 $tpx_data_dir &
/home/pi/Documents/Fik8/scripts/Labdos_scripts/log.py $lbd_data_file &





