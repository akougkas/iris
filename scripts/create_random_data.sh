#!/usr/bin/env bash
var=$2
var=`expr $var - 1`;
var2=`expr $3 - 1`;
sudo rm /tmp/datafile &> /tmp/dd.log
dd if=/dev/zero of=/tmp/datafile count=1024 bs=$var2 &> /tmp/dd.log
sudo rm -r ${1}*  &> /tmp/dd.log
for i in $(seq 0 $var)
do
	cmd="/opt/orangefs/bin/pvfs2-cp /tmp/datafile ${1}_$i";
        #echo "${1}_${i} copied"
        $cmd
done