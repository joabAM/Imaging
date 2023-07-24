#!/usr/bin/python2

"""
    Change history:

    Written by "Miguel Urco":mailto:murco@jro.igp.gob.pe  Set 9, 2010
"""

import os, sys, glob
import optparse
import time, datetime
import ftplib
import numpy


path = os.path.dirname(os.getcwd())
sys.path.insert(0, path)

# prepare to handle MadrigalError
import imagpy.iofiles.imgfile as imgfile
import imagpy.proc.imgproc as imgproc
import imagpy.graphs.imgplot as imgplot
import imagpy.utils.mkMovie as mkMovie
import imagpy.utils.misc as misc

import imagpy.genImage as genImage

DESCRIPTION = """ """

USAGE = """
    "This script generates RGB plots from spectra data"

    [user@domain ~]$ genImagPlots.py <options>

    For more information of <options>:

    [user@domain ~]$ genImagPlots.py --help

    Example:
            python genImagPlots.py --dpath=/users/myuser/data/BINFILES --ppath=/users/myuser/temp --start-year=2010 --start-doy=364 --start-hour=20 --end-hour=23 --mindB=0 --maxdB=30 --plot-spec=1 --plot-rtdi=1 --full-rtdi

            python genImagPlots.py --dpath=/home/roj-idl71/BIN_FILES --ppath=/tmp/ --start-year=2011 --start-doy=355 --start-hour=10 --end-hour=17 --start-minute=0 --end-minute=0 --mindB=-5 --maxdB=25

            python genImagPlots.py --dpath=/home/BIN_FILES --ppath=/tmp/ --start-year=2011 --start-doy=244 --start-hour=19 --end-hour=21 --mindB=-5 --maxdB=25 --min-hei=90 --max-hei=110 --min-dcosx=-0.1 --max-dcosx=0.1

            python genImagPlots.py --dpath=/home/BIN_FILES --ppath=/tmp/ --start-year=2011 --start-doy=248 --start-hour=18 --mindB=3 --maxdB=35 --min-hei=90 --max-hei=140 --filter --beam-filter --fft-shift
"""

def gotoxy(x, y, arg=''):
    ESC = '\033'
    CSI = ESC + "["
    sys.stdout.write( CSI + str(y) + ";" + str(x) + 'H' + str(arg))

def cls():
    if os.name == "posix":
        # Unix/Linux/MacOS/BSD/etc
        os.system('clear')
    elif os.name in ("nt", "dos", "ce"):
        # DOS/Windows
        os.system('CLS')
    else:
        # Fallback for other operating systems.
        print '\n' * 80

class GenImagPlots(genImage.genImage):
    """
    classdocs: This class allow to read the bin file from dpath and
    plot data.

    run() method should be defined!

    Change history:

    Written by "Miguel Urco":mailto:murco@jro.igp.gob.pe  Set 9, 2010
    """

    __RTI_SCALE = 1

    def __init__(self):

        genImage.genImage.__init__(self)

    def setOptions(self):
        '''
        Using default arguments from genImage.genImage
        '''

        genImage.genImage.setOptions(self, USAGE)

        self.parser.add_option("", "--plot-spec", dest="plot_spec", metavar="<int>",
                          type="int", default=1, help="Enable plotting of instantaneous Spectra Beam, 1 enabled and 0 disabled. By default is 1")

        self.parser.add_option("", "--plot-rtdi", dest="plot_rtdi", metavar="<int>",
                          type="int", default=1, help="Enable plotting of range time doppler intensity (RTDI), 1 enabled and 0 disabled. By default is 1")

        self.parser.add_option("", "--zcut", metavar="<int>",
                          type="int", default=0, help="Enable height averaging to get RTDI, 1 enabled and 0 disabled. By default is 0")

        self.parser.add_option("", "--full-rtdi", dest="full_rtdi", metavar="<int>",
                          action="store_true", default=False, help="Enable plotting of all RTDI from the beginning, 1 enabled and 0 disabled. By default is 0")

        self.parser.add_option("", "--std-plot", dest="std_plot", metavar="<int>",
                          type="int", default=0, help="Get image plots for Jicamarca reports, in spanish and new dimensions.")
#
#         self.parser.add_option("", "--ftpServer", metavar="<server>", type="string",
#                           default=None, help="ftp server where the graphics will be sent")
#
#         self.parser.add_option("", "--ftpUser", metavar="<user>", type="string",
#                           default=None, help="ftp user")
#
#         self.parser.add_option("", "--ftpPass", metavar="<pass>", type="string",
#                           default=None, help="ftp password")
#
#         self.parser.add_option("", "--ftpPath", metavar="<path>", type="string",
#                           default=None, help="Directory where the images will be sent")
#
#         self.parser.add_option("", "--ftpFile", metavar="<filename>", type="string",
#                           default=None, help="All files will be to send to server using this filename")
#
#         self.parser.add_option("", "--ftpPrefix", metavar="<file prefix>", type="string",
#                           default=None, help="If ftpFile is not set, ftpPrefix can be used as filename prefix followed of year and doy")


    def setScriptState(self):

        genImage.genImage.setScriptState(self)

        (options, args) = self.parser.parse_args()

        self.plot_spec = options.plot_spec
        self.plot_rtdi = options.plot_rtdi
        self.plot_full_rtdi = options.full_rtdi
        self.plot_stdOut = options.std_plot
#
#         self.ftpServer = options.ftpServer
#         self.ftpUser = options.ftpUser
#         self.ftpPass = options.ftpPass
#         self.ftpPath = options.ftpPath
#         self.ftpFile = options.ftpFile
#         self.ftpPrefix = options.ftpPrefix
#         self.ftpExt = '.png'

        self.zcut = options.zcut

    def createDoyPath(self, year, doy, delPNGFiles = True):

        #Default
        folderId = "%03d" %(self.readerObj.nfft)

        prefix ="iii"

        if self.plot_spec:
            prefix = "spc"

        if self.plot_rtdi:
            prefix = "rti"

        if self.plot_spec and self.plot_rtdi:
            prefix = "s_r"

        doypath = os.path.join(self.imgpath,"%s%04d%03d_%s" %(prefix,
                                                             year,
                                                             doy,
                                                             folderId))
        if not(os.path.exists(doypath)): os.mkdir(doypath)

        rtifile = os.path.join(self.rtipath, "rti%04d%03d_%s.h5" %(year,
                                                                   doy,
                                                                   folderId))

        # moviefile = os.path.join(self.moviepath, "movie%04d%03d_%s.avi" %(year,
        #                                                                   doy,
        #                                                                   folderId))
        moviefile = os.path.join(self.moviepath, "%04d%03d.mp4" %(year,
                                                                doy))

        self.doypath = doypath
        self.rtifile = rtifile
        self.moviefile = moviefile

        if not self.save:
            return

        if delPNGFiles:
            self.removeFiles(self.doypath, ext=self.ext)

    def getFullRTDI(self, nx, ny, nz, ix_min, ix_max, iy_min, iy_max, ih_min, ih_max):

        print "Reading Full RTDI ..."
        nc = 3

        trti1D = numpy.empty(shape=(0), dtype=numpy.float)
        rgb_rti3D = numpy.empty(shape=(0,nz,nc), dtype=numpy.int)

        ind_colors = imgproc.gen_color_indexes(nFFTPoints=self.readerObj.nfft, nbins=self.nbins)

        while(1):
            #Dimensions (nx, ny, nheis, ncolors)
            datadB4D = self.readerObj.readNextBlock()

            if datadB4D is None:
                break

            #Filtering on x, y and z.
            datadB4D = datadB4D[ix_min:ix_max+1, iy_min:iy_max+1, ih_min:ih_max+1, :]

            linear_data4D = imgproc.dBData2Linear(datadB4D, remove_interf=False, remove_dc=False, fft_shift=self.fft_shift,
                                                  smooth=self.filter, percentile = self.percentile)

            #Selecting fft points to use
            #input (nx, nheis, nffts)
            #output (nx, nheis, ncolors)
            linear_data4D = imgproc.binFilter(linear_data4D, ind_colors=ind_colors)

            #############SPECTRA##################
            #Averaging on Y axis. Getting YCUT (nx, nheis, ncolors).
            linear_data3D = imgproc.mask_average(linear_data4D, axis=1)

            if self.beam_filter:
                linear_data3D = (linear_data3D.T * self.beam_pattern).T

            ################RTI###################
            #Getting average on x (nheis, ncolors)
            avg_linear_data2D = imgproc.mask_average(linear_data3D, axis=0)
            avg_linear_data2D *= self.__RTI_SCALE

            #Getting rgb profile (nheis, ncolors)
            rgb_profile2D = imgplot.linearData2RGB(avg_linear_data2D, mindB=self.mindB, maxdB=self.maxdB)

            trti1D = numpy.append(trti1D, self.readerObj.utc)
            rgb_rti3D = numpy.append(rgb_rti3D, rgb_profile2D.reshape(1,nz,nc), axis = 0)

        self.readerObj.resetCounters()

        return trti1D, rgb_rti3D

    def plotData(self, startDatetime, endDatetime=None):

        sts = self.readerObj.settings(self.dpath,
                                      startDatetime,
                                      endDatetime,
                                      online=self.online,
                                      rotate_image=False)

        if not sts:
            print("No data was found in %s between %s - %s" %(self.dpath, startDatetime, endDatetime))
            return 0

        tt = startDatetime.timetuple()
        year = tt[0]; doy = tt[7]

        startSeconds = misc.toseconds(startDatetime)
        endSeconds = misc.toseconds(endDatetime)

        self.createDoyPath(year, doy)

        ix_min, ix_max = misc.findMinMaxIndex(self.readerObj.dcosx, self.mindcosx, self.maxdcosx)
        iy_min, iy_max = misc.findMinMaxIndex(self.readerObj.dcosy, self.mindcosy, self.maxdcosy)
        ih_min, ih_max = misc.findMinMaxIndex(self.readerObj.heis, self.hmin, self.hmax)

        if (self.xmin != None) and (self.xmax != None):
            ix_min, ix_max = misc.findMinMaxIndex(self.readerObj.x[:,self.readerObj.ny/2,-1],
                                                  self.xmin, self.xmax)

        if (self.ymin != None) and (self.ymax != None):
            iy_min, iy_max = misc.findMinMaxIndex(self.readerObj.y[self.readerObj.nx/2,:,-1],
                                                  self.ymin, self.ymax)

        nx = ix_max + 1 - ix_min
        ny = iy_max + 1 - iy_min
        nz = ih_max + 1 - ih_min
        nc = 3

        #Filtering on x, y and z
#         dcosx = self.readerObj.dcosx[ix_min:ix_max+1]
#         dcosy = self.readerObj.dcosy[iy_min:iy_max+1]
#         heis = self.readerObj.heis[ih_min:ih_max+1]

        x = self.readerObj.x[ix_min:ix_max+1, iy_min:iy_max+1, ih_min:ih_max+1]
        y = self.readerObj.y[ix_min:ix_max+1, iy_min:iy_max+1, ih_min:ih_max+1]
        z = self.readerObj.z[ix_min:ix_max+1, iy_min:iy_max+1, ih_min:ih_max+1]


        xmin = None
        xmax = None
        hmin = None
        hmax = None

        if self.hmin != None:
            hmin = self.hmin
        else:
            hmin = numpy.min(z)

        if self.hmax != None:
            hmax = self.hmax
        else:
            hmax = numpy.max(z)

        if self.xmin != None:
            xmin = self.xmin
        elif self.mindcosx != None:
            xmin = self.mindcosx*hmax

        if self.xmax != None:
            xmax = self.xmax
        elif self.mindcosx != None:
            xmax = self.maxdcosx*hmax


        if self.nbins == None:
            self.nbins = self.readerObj.nfft

        ind_colors = imgproc.gen_color_indexes(nFFTPoints=self.readerObj.nfft, nbins=self.nbins)

        self.beam_pattern = numpy.sin(numpy.arange(1,nx+1)*numpy.pi/(nx+1))

        #RTI
        trti1D = None
        rgb_rti3D = None

        if self.plot_rtdi:

            if not self.plot_full_rtdi:
                trti1D = numpy.empty(shape=(0), dtype=numpy.float)
                rgb_rti3D = numpy.empty(shape=(0,nz,nc), dtype=numpy.int)
            else:
                trti1D, rgb_rti3D = self.getFullRTDI(nx, ny, nz, ix_min, ix_max, iy_min, iy_max, ih_min, ih_max)

        while(1):
            #Dimensions (nx, ny, nheis, ncolors)
            datadB4D = self.readerObj.readNextBlock()

            if datadB4D is None:
                break



            #Filtering on x, y and z.
            datadB4D = datadB4D[ix_min:ix_max+1, iy_min:iy_max+1, ih_min:ih_max+1, :]

            linear_data4D = imgproc.dBData2Linear(datadB4D, remove_interf=False, remove_dc=False, fft_shift=self.fft_shift,
                                                  smooth=self.filter, percentile = self.percentile)

            #Selecting fft points to use
            #input (nx, nheis, nffts)
            #output (nx, nheis, ncolors)
            linear_data4D = imgproc.binFilter(linear_data4D, ind_colors=ind_colors)

            #############SPECTRA##################
            #Averaging on Y axis. Getting YCUT (nx, nheis, ncolors).
            linear_data3D = imgproc.mask_average(linear_data4D, axis=1)

            if self.beam_filter:
                linear_data3D = (linear_data3D.T * self.beam_pattern).T
            #Getting RGB Spectra (nx, nheis, ncolors)
            rgb_spec3D = imgplot.linearData2RGB(linear_data3D, mindB=self.mindB, maxdB=self.maxdB)

            ################RTI###################
            if self.plot_rtdi:

                if not self.plot_full_rtdi:
                    #Getting average on x (nheis, ncolors)
                    avg_linear_data2D = imgproc.mask_average(linear_data3D, axis=0)
                    avg_linear_data2D *= self.__RTI_SCALE

                    #Getting rgb profile (nheis, ncolors)
                    rgb_profile2D = imgplot.linearData2RGB(avg_linear_data2D, mindB=self.mindB, maxdB=self.maxdB)

                    trti1D = numpy.append(trti1D, self.readerObj.utc)
                    rgb_rti3D = numpy.append(rgb_rti3D, rgb_profile2D.reshape(1,nz,nc), axis = 0)
                else:
                    pass

            vmax = (self.readerObj.vel_range*self.nbins/self.readerObj.nfft)/2

            imgplot.plotSpecAndRTI(xspec2D = x[:,ny/2,:],
                                    yspec2D = z[:,ny/2,:],
                                    rgb_data3D = rgb_spec3D,
                                    ddatetime = self.readerObj.filedate,
                                    trti1D = trti1D,
                                    yrti1D = z[nx/2,ny/2,:],
                                    rti3D = rgb_rti3D,
                                    xmin = xmin,
                                    xmax = xmax,
                                    ymin = hmin,
                                    ymax = hmax,
                                    tmin = startSeconds,
                                    tmax = endSeconds,
                                    std_plots = self.plot_stdOut,
                                    plot_spec = self.plot_spec,
                                    plot_rtdi = self.plot_rtdi,
                                    show=self.show,
                                    save=self.save,
                                    img_path = self.imgpath,
                                    fpath = self.doypath,
                                    year = year, doy = doy,
                                    ext = self.ext,
                                    plot_colormap=True,
                                    cmap_xmin = -vmax,
                                    cmap_xmax = vmax,
                                    cmap_ymin = self.mindB,
                                    cmap_ymax = self.maxdB)

#             if not(pngfile): break


#             self.__sendFileByFtp(pngfile, startDatetime)

        imgplot.close_all()

        print "%d files were processed" %(self.readerObj.indFile)

        if self.readerObj.indFile == 0:
            return 0

#         rtiFile.writeRtiFile(self.rtifile, self.rti, self.times)

#         self.__sendFileByFtp(pngfile)

        print "Making movie"

        if self.ext == ".png":
            mkMovie.mkMovie(self.doypath, self.moviefile)

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

    GenImagPlots()
