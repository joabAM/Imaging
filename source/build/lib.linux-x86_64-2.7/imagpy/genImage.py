#!/usr/bin/python

"""
    Change history:

    Written by "Miguel Urco":mailto:murco@jro.igp.gob.pe  Jul 27, 2015

    This module is used as Parent Class by other scripts.
    "plotData" method should be implemented.
"""

import os, sys, glob
import optparse
import datetime
import numpy

path = os.path.dirname(os.getcwd())
sys.path.insert(0, path)

# prepare to handle MadrigalError
import imagpy.iofiles.imgfile as imgfile
import imagpy.proc.imgproc as imgproc
import imagpy.graphs.imgplot as imgplot

DESCRIPTION = """ """

USAGE = """
    "This script generates ....."


    python genImage.py <options>

    Examples:

        python genImage.py --dpath=/BINFILES --ppath=/graphs --start-year=2010 --start-doy=364 --mindB=0 --maxdB=30 --show=0 --save=1

"""

class genImage():
    """
    classdocs: This class allow to read the hdf5 file from dpath and create some images from it.

    Change history:

    Written by "Miguel Urco":mailto:murco@jro.igp.gob.pe  Jul 27, 2015
    """

    def __init__(self):

        self.setOptions()

        self.setScriptState()

        self.verifyParameters()

        self.createObjects()

        self.createDirectories()

        self.run()

    def setOptions(self, usageIn = None):

        if not usageIn:
            usageIn = USAGE

        self.parser = optparse.OptionParser(usage=usageIn)

        self.parser.add_option("-d", "--dpath", dest="dpath", metavar="<PATH>",
                          type="string", help="Data directory where HDF5 files were stored")

        self.parser.add_option("-p", "--ppath", dest="ppath", metavar="<PATH>",
                          type="string", help="Resource directory where graphics will be stored")

        self.parser.add_option("", "--start-year", dest="startYear", metavar="<YYYY>",
                          type="int", default=None, help="initial year")

        self.parser.add_option("", "--end-year", dest="endYear", metavar="<YYYY>",
                          type="int", default=None, help="end year")

        self.parser.add_option("", "--start-doy", dest="startDoy", metavar="<DDD>",
                          type="int", default=None, help="initial day of year")

        self.parser.add_option("", "--end-doy", dest="endDoy", metavar="<DDD>",
                          type="int", default=None, help="end day of year")

        self.parser.add_option("", "--start-hour", dest="startHour", metavar="<HH>",
                          type="float", default=00, help="initial hour. 00 by default")

        self.parser.add_option("", "--end-hour", dest="endHour", metavar="<HH>",
                          type="float", default=24, help="end hour. 24 by default")

        self.parser.add_option("", "--start-minute", dest="startMin", metavar="<MM>",
                          type="int", default=00, help="initial minute")

        self.parser.add_option("", "--end-minute", dest="endMin", metavar="<MM>",
                          type="int", default=00, help="end minute. 00 by default")


        self.parser.add_option("", "--mindB", dest="mindB", metavar="<float>",
                          type="int", default=5, help="minimum dB value")

        self.parser.add_option("", "--maxdB", dest="maxdB", metavar="<float>",
                          type="float", default=40, help="maximum dB value")

        self.parser.add_option("", "--nbins", dest="nbins", metavar="<int>",
                          type="int", default=None, help="number of fft points around DC used to generate graphics")

        self.parser.add_option("", "--min-dcosx", dest="mindcosx", metavar="<float>",
                          type="float", default=None, help="minimum dcosx")

        self.parser.add_option("", "--max-dcosx", dest="maxdcosx", metavar="<float>",
                          type="float", default=None, help="maximum dcosx")

        self.parser.add_option("", "--min-dcosy", dest="mindcosy", metavar="<float>",
                          type="float", default=None, help="minimum dcosy")

        self.parser.add_option("", "--max-dcosy", dest="maxdcosy", metavar="<float>",
                          type="float", default=None, help="maximum dcosy")

        self.parser.add_option("", "--min-x", dest="xmin", metavar="<float>",
                          type="float", default=None, help="minimum x in Km")

        self.parser.add_option("", "--max-x", dest="xmax", metavar="<float>",
                          type="float", default=None, help="maximum x in Km")

        self.parser.add_option("", "--min-y", dest="ymin", metavar="<float>",
                          type="float", default=None, help="minimum y in Km")

        self.parser.add_option("", "--max-y", dest="ymax", metavar="<float>",
                          type="float", default=None, help="maximum y in Km")

        self.parser.add_option("", "--min-hei", dest="hmin", metavar="<float>",
                          type="float", default=None, help="minimum height in km")

        self.parser.add_option("", "--max-hei", dest="hmax", metavar="<float>",
                          type="float", default=None, help="maximum height in Km")

        self.parser.add_option("", "--percentile", metavar="<float>",
                          type="float", default=None, help="enable filter using percentile threshold")

        self.parser.add_option("", "--ext", metavar="<.ext>",
                          type="string", default=".png", help="file extension")

        self.parser.add_option("", "--show", dest="show", metavar="<int>",
                          type="int", default=1, help="Set to 1 if you want to display graphics on screen, 0 otherwise. By default is 1")

        self.parser.add_option("", "--save", dest="save", metavar="<int>",
                          type="int", default=1, help="Set to 1 if you want to store graphics on disk, 0 otherwise.  By default is 1")

        self.parser.add_option("", "--filter",
                          action="store_true", default=False, help="Enable data filter")

        self.parser.add_option("", "--beam-filter", dest="beam_filter",
                          action="store_true", default=False, help="Enable antenna beam filter")

        self.parser.add_option("", "--online",
                          action="store_true", default=False, help="Enable online data access")

        self.parser.add_option("", "--fft-shift", dest="fft_shift",
                          action="store_true", default=False, help="Enable fft shift")

        self.parser.set_description(DESCRIPTION)

    def setScriptState(self):

        (options, args) = self.parser.parse_args()

        self.dpath = options.dpath
        self.ppath = options.ppath
        self.startYear = options.startYear
        self.startDoy = options.startDoy
        self.endYear = options.endYear
        self.endDoy = options.endDoy
        self.hour = [options.startHour, options.endHour]
        self.minute = [options.startMin, options.endMin]

        self.mindB = options.mindB
        self.maxdB = options.maxdB

        self.mindcosx = options.mindcosx
        self.maxdcosx = options.maxdcosx
        self.mindcosy = options.mindcosy
        self.maxdcosy = options.maxdcosy

        self.xmin = options.xmin
        self.xmax = options.xmax
        self.ymin = options.ymin
        self.ymax = options.ymax
        self.hmin = options.hmin
        self.hmax = options.hmax

        self.filter = options.filter
        self.beam_filter = options.beam_filter

        self.show = options.show
        self.save = options.save

        self.nbins = options.nbins
        self.online = options.online

        self.fft_shift = options.fft_shift
        self.percentile = options.percentile

        self.ext = options.ext

    def verifyParameters(self):

        if self.dpath == None:
            self.parser.print_help()
            print
            print '--dpath argument required'
            sys.exit(-1)

        if self.ppath == None:
            self.parser.print_help()
            print
            print '--ppath argument required'
            sys.exit(-1)

        if self.startYear==None:
            self.parser.print_help()
            print
            print '--startYear argument required'
            sys.exit(-1)

        if self.startDoy==None:
            self.parser.print_help()
            print
            print '--startDoy argument required'
            sys.exit(-1)

        if self.endYear ==None:
            self.endYear = self.startYear

        if self.endDoy==None:
            self.endDoy = self.startDoy

        #Verifying parameters
        if not(os.path.exists(self.dpath)):
            print "Data directory '%s' does not exist" %self.dpath
            sys.exit(-1)

        if not(os.path.exists(self.ppath)):
            os.mkdir(self.ppath)

        if not(os.path.exists(self.ppath)):
            print "Process directory '%s cannot be created" %self.ppath
            sys.exit(-1)

        if self.startYear<1900 or self.startYear>3000:
            print "startYear out of range"
            sys.exit(-1)

        if self.startDoy<1 or self.startDoy>366:
            print "startDoy out of range"
            sys.exit(-1)

        if self.endYear<1900 or self.endYear>3000:
            print "endYear out of range"
            sys.exit(-1)

        if self.endDoy<1 or self.endDoy>366:
            print "endDoy out of range"
            sys.exit(-1)

        if min(self.hour)<0 or max(self.hour)>50:
            print "hour out of range"
            sys.exit(-1)

        if min(self.minute)<0 or max(self.minute)>60:
            print "minute out of range"
            sys.exit(-1)

        if self.mindB<-200 or self.maxdB>200:
            print "dB range out of scale, it should be between -200 and 200"
            sys.exit(-1)

    def createObjects(self):

        self.readerObj = imgfile.readImaging()

    def createDirectories(self):

        ##################################################################################################
        ############################## Creating directories ##############################################
        ##################################################################################################

        imgpath = os.path.join(self.ppath,"FIG_FILES")
        if not(os.path.exists(imgpath)):  os.mkdir(imgpath)

        rtipath = os.path.join(self.ppath,"RTI_FILES")
        if not(os.path.exists(rtipath)): os.mkdir(rtipath)

        moviepath = os.path.join(self.ppath,"MOVIES")
        if not(os.path.exists(moviepath)): os.mkdir(moviepath)

        cutpath = os.path.join(self.ppath,"CUTS_FILES")
        if not(os.path.exists(cutpath)):  os.mkdir(cutpath)

        self.imgpath = imgpath
        self.rtipath = rtipath
        self.moviepath = moviepath
        self.cutpath = cutpath

    def createDoyPath(self, year, doy, delPNGFiles = True):

        #Default
        folderId = "%03d" %(self.readerObj.nfft)

        doypath = os.path.join(self.imgpath,"i%04d%03d_%s" %(year, doy, folderId))

        if not(os.path.exists(doypath)):
            os.mkdir(doypath)

        rtifile = os.path.join(self.rtipath, "rti%04d%03d_%s.h5" %(year, doy, folderId))

        moviefile = os.path.join(self.moviepath, "movie%04d%03d_%s.avi" %(year, doy, folderId))

        doycutpath = os.path.join(self.cutpath,"i%04d%03d_%s" %(year, doy, folderId))

        if not(os.path.exists(doycutpath)):
            os.mkdir(doycutpath)

        self.doypath = doypath
        self.rtifile = rtifile
        self.moviefile = moviefile
        self.doycutpath = doycutpath

        if not self.save:
            return

        if delPNGFiles:
            self.removeFiles(self.doypath, ext=self.ext)
            self.removeFiles(self.doycutpath, ext=self.ext)

        return



    def removeFiles(self, path, ext=".png", delforced = True):

        fileList = glob.glob1(path, "*%s" %ext)

        if len(fileList) < 1:
            return

        if delforced == False:
            print "Some %s files were found into the directory %s" %(ext, path)

            answer = raw_input("If you continue they will be deleted. Do you want to continue (y/n)?: ")

            if answer.lower() != 'y':
                print "The program has been cancelled"
                sys.exit(0)

        #Deleting old files
        #print "Deleting old png files from %s" %(doypath)
        for thisFile in fileList:
            thisFilename = os.path.join(path, thisFile)
            os.remove(thisFilename)

        return

    def plotData(self, startDatetime, endDatetime, filter = False):

        raise NotImplementedError, "This method should be implemented"

        return 1

    def run(self):

        initialDoy = datetime.datetime(self.startYear,1,1,0,0,0) + datetime.timedelta(self.startDoy-1)
        finalDoy = datetime.datetime(self.endYear,1,1,23,59,59) + datetime.timedelta(self.endDoy-1)
        thisDoy = initialDoy

        while thisDoy.date() <= finalDoy.date():

            startDatetime = thisDoy + datetime.timedelta(0,0,0,0,self.minute[0],self.hour[0])
            endDatetime = thisDoy + datetime.timedelta(0,0,0,0,self.minute[-1],self.hour[-1])

            sts = self.plotData(startDatetime, endDatetime)

            if not sts:
                break

            thisDoy += datetime.timedelta(1)

##########################################
##########################################
##########################################
##### THE MAIN SCRIPT STARTS HERE ########
##########################################
##########################################
##########################################

if __name__ == '__main__':

    genImage()
