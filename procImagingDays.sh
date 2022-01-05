#!/bin/bash
#Script to process multiple days for Imaging experiment, config-file is necessary
#please kill all existing processes related to Imaging before running this script
#You need to stablish the pdata location, ensure the DOY folder had the right format
PATHDATA="/media/soporte/DATA/IMAGING/PDATA"
#PATHDATA="/media/soporte/DATA/ISR"
PATHOUT="/home/soporte/Documentos/Imaging/OUTPUTmp4"

YEAR=2019
START=243
END=243

INITHOUR=18
ENDHOUR=7
DBMAX=35
NAVG=4		#4 para Julia, 2 para ISR a veces
MIN_H=50
MAX_H=780	#hasta 950km/30 o 780km
DCOSX=0.2	# 0.15 o 0.2 Julia, ISR a 0.4 o 0.3
#*********************************************************************************
#*********************************************************************************
#*********************************************************************************
DAY=$START
NEXTDAY=$(($DAY+1))
NDAY=$(($END-$START))
#PATHCONFIG=$(pwd)"/config-files/"
FAIL_LIST=""

echo $PATHDATA

procImaging_partOne (){
	local pathfile=_config$YEAR$(printf "%03d" $DAY).txt
	#echo $pathfile
	yes n | procImaging --dpath=$PATHDATA --ppath=$PATHOUT --start-year=$YEAR --start-doy=$DAY --start-hour=$INITHOUR --navg=$NAVG  --config-file=$pathfile --procKey=135
}

procImaging_partTwo (){
	local pathfile=_config$YEAR$(printf "%03d" $DAY).txt
	#echo $pathfile
	yes n | procImaging --dpath=$PATHDATA --ppath=$PATHOUT --start-year=$YEAR --start-doy=$NEXTDAY --start-hour=0 --end-hour=$ENDHOUR --navg=$NAVG  --config-file=$pathfile --procKey=145
}

kill_procImaging() {
	killall  procImaging
}

kill_python (){
	killall  python
}

genVideo (){
	yes "\n" | nohup genImagPlots.py --dpath=$PATHOUT --ppath=$PATHOUT --start-year=$YEAR --start-doy=$DAY  --start-hour=$INITHOUR --start-min=05 --end-hour=30 --nbins=8 --min-hei=$MIN_H --max-hei=$MAX_H  --save=1 --filter --fft-shift --maxdB=$DBMAX --min-dcosx=-$DCOSX --max-dcosx=$DCOSX &
	sleep 5s
	echo Plotting...$DAY
}
#*********************************************************************************
#*********************************************************************************
#*********************************************************************************
kill_procImaging
while [ $DAY -le $END ]; do
	procImaging_partOne
	if ["$?" -ne "0"]	##not 0? something fail
	then
		FAIL_LIST=$FAIL_LIST $DAY
		kill_procImaging

	else		#equal 0, then it's OK
		sleep 2s
		procImaging_partTwo
		if ["$?" -ne "0"] # fail second half?
		then
			kill_procImaging
			procImaging_partTwo
			if ["$?" -ne "0"] # fail second half again?
			then
				kill_procImaging
				FAIL_LIST=$FAIL_LIST $DAY
			fi
		else
			kill_procImaging
			kill_python
			sleep 2s
			genVideo
		fi
	fi
	DAY=$(($DAY+1))
	NEXTDAY=$(($DAY+1))
done
echo Finish...
echo processed Imaging data from $START to $END of $YEAR
echo failed processing days $FAIL_LIST
