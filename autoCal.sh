#!/bin/bash
#Script to process multiple days for Imaging experiment, config-file is necessary
#please kill all existing processes related to Imaging before running this script
#You need to stablish the pdata location, ensure the DOY folder had the right format
PATHDATA="/mnt/DATA/pdata"
#PATHDATA="/media/soporte/DATA/ISR"
PATHOUT="/mnt/DATA/imaging/h5data"


CONFIG_FILE="calibration.txt"

YEAR=2018
DOY=252

INITHOUR=20
STARTMIN=00
MIN_H=250
MAX_H=300	

ENDHOUR=24
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##********************************************************************************
##+++++++++++++++++++++++++ CHANNEL 2 CALIBRATION ++++++++++++++++++++++++++++++++
DCOSX=0.1
DCOSY=0.1
NX=32
NY=32
yes y | calibrateImaging --dpath=$PATHDATA --ppath=$PATHOUT --start-year=$YEAR --start-doy=$DOY --start-hour=$INITHOUR --start-min=$STARTMIN --end-hour=$ENDHOUR --channels=0,2  --min-hei=$MIN_H --max-hei=$MAX_H  --config-file=$CONFIG_FILE --nthreads=16 --dcosxM=$DCOSX --dcosyM=$DCOSY --nxPoints=$NX --nyPoints=$NY --showProcess=0 > calibrationOutput.txt


##+++++++++++++++++++++++++ CHANNEL 1 CALIBRATION ++++++++++++++++++++++++++++++++
DCOSX=0.2
DCOSY=0.2
NX=64
NY=32
yes y | calibrateImaging --dpath=$PATHDATA --ppath=$PATHOUT --start-year=$YEAR --start-doy=$DOY --start-hour=$INITHOUR --start-min=$STARTMIN --end-hour=$ENDHOUR --channels=0,1,2 --cal-channel=1  --min-hei=$MIN_H --max-hei=$MAX_H  --config-file=$CONFIG_FILE --nthreads=16 --dcosxM=$DCOSX --dcosyM=$DCOSY --nxPoints=$NX --nyPoints=$NY --showProcess=0 >> calibrationOutput.txt




##+++++++++++++++++++++++++ CHANNEL 3 CALIBRATION ++++++++++++++++++++++++++++++++
DCOSX=0.1
DCOSY=0.05
NX=64
NY=16
yes | calibrateImaging --dpath=$PATHDATA --ppath=$PATHOUT --start-year=$YEAR --start-doy=$DOY --start-hour=$INITHOUR --start-min=$STARTMIN --end-hour=$ENDHOUR --channels=0,1,2,3   --min-hei=$MIN_H --max-hei=$MAX_H  --config-file=$CONFIG_FILE --nthreads=16 --dcosxM=$DCOSX --dcosyM=$DCOSY --nxPoints=$NX --nyPoints=$NY --showProcess=0 >> calibrationOutput.txt



##+++++++++++++++++++++++++ CHANNEL 4 CALIBRATION ++++++++++++++++++++++++++++++++
DCOSX=0.1
DCOSY=0.05
NX=64
NY=16
yes | calibrateImaging --dpath=$PATHDATA --ppath=$PATHOUT --start-year=$YEAR --start-doy=$DOY --start-hour=$INITHOUR --start-min=$STARTMIN --end-hour=$ENDHOUR --channels=0,1,2,3,4   --min-hei=$MIN_H --max-hei=$MAX_H  --config-file=$CONFIG_FILE --nthreads=16 --dcosxM=$DCOSX --dcosyM=$DCOSY --nxPoints=$NX --nyPoints=$NY --showProcess=0 >> calibrationOutput.txt


##+++++++++++++++++++++++++ CHANNEL 5 CALIBRATION ++++++++++++++++++++++++++++++++
DCOSX=0.1
DCOSY=0.04
NX=64
NY=16
yes | calibrateImaging --dpath=$PATHDATA --ppath=$PATHOUT --start-year=$YEAR --start-doy=$DOY --start-hour=$INITHOUR --start-min=$STARTMIN --end-hour=$ENDHOUR --channels=0,1,2,3,4,5   --min-hei=$MIN_H --max-hei=$MAX_H  --config-file=$CONFIG_FILE --nthreads=16 --dcosxM=$DCOSX --dcosyM=$DCOSY --nxPoints=$NX --nyPoints=$NY --navg=2 --showProcess=0 >> calibrationOutput.txt


##+++++++++++++++++++++++++ CHANNEL 6 CALIBRATION ++++++++++++++++++++++++++++++++
DCOSX=0.1
DCOSY=0.04
NX=64
NY=16
yes | calibrateImaging --dpath=$PATHDATA --ppath=$PATHOUT --start-year=$YEAR --start-doy=$DOY --start-hour=$INITHOUR --start-min=$STARTMIN --end-hour=$ENDHOUR --channels=0,1,2,3,4,5,6   --min-hei=$MIN_H --max-hei=$MAX_H  --config-file=$CONFIG_FILE --nthreads=16 --dcosxM=$DCOSX --dcosyM=$DCOSY --nxPoints=$NX --nyPoints=$NY --navg=2 --showProcess=0 >> calibrationOutput.txt



##+++++++++++++++++++++++++ CHANNEL 7 CALIBRATION ++++++++++++++++++++++++++++++++
DCOSX=0.1
DCOSY=0.04
NX=64
NY=16
yes | calibrateImaging --dpath=$PATHDATA --ppath=$PATHOUT --start-year=$YEAR --start-doy=$DOY --start-hour=$INITHOUR --start-min=$STARTMIN --end-hour=$ENDHOUR --channels=0,1,2,3,4,5,6,7   --min-hei=$MIN_H --max-hei=$MAX_H  --config-file=$CONFIG_FILE --nthreads=16 --dcosxM=$DCOSX --dcosyM=$DCOSY --nxPoints=$NX --nyPoints=$NY --navg=2 --showProcess=0 >> calibrationOutput.txt



