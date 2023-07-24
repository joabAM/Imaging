#!/usr/bin/python2

"""
    Change history:

    Written by "Miguel Urco":mailto:murco@jro.igp.gob.pe  Jul 27, 2015
"""

import os, sys, glob
import optparse
import datetime
import numpy
import matplotlib.pyplot as plt
from matplotlib import cm
from mpl_toolkits.mplot3d import Axes3D

path = os.path.dirname(os.getcwd())
sys.path.insert(0, path)

# prepare to handle MadrigalError
import imagpy.iofiles.imgfile as imgfile
import imagpy.proc.imgproc as imgproc
import imagpy.graphs.imgplot as imgplot
import imagpy.utils.misc as misc

import imagpy.genImage as genImage

usage = """
    "This script generates Imaging Cuts from hdf5 files."

    Examples:

        python genImagCuts.py --dpath=/BINFILES --ppath=/graphs --start-year=2010 --start-doy=364 --mindB=0 --maxdB=30 --plot-xcut --plot-ycut --plot-zcut

        python genImagCuts.py --dpath=/BINFILES --ppath=/tmp/ --start-year=2011 --start-doy=355 --start-hour=10 --start-minute=0 --mindB=-5 --maxdB=25 --show=1 --save=1 --filter --plot-dcosxcut --plot-dcosycut --plot-rcut --find-layer

        python genImagCuts.py --dpath=/BINFILES --ppath=/tmp/ --start-year=2011 --start-doy=244 --start-hour=19 --end-hour=21 --mindB=-5 --maxdB=25 --nbins=8 --min-dcosx=-0.2 --max-dcosx=0.2 --min-dcosy=-0.2 --max-dcosy=0.2 --min-hei=90 --max-hei=110 --show=0 --save=1 --filter --plot-xcut --plot-ycut --plot-zcut

        python genImagCuts.py --dpath=/BINFILES --ppath=/tmp/ --start-year=2011 --start-doy=248 --start-hour=18 --end-hour=20 --mindB=3 --maxdB=35 --min-hei-avg=90 --max-hei-avg=110 --min-dcosx-avg=-0.2 --max-dcosx-avg=0.2 --min-dcosy-avg=-0.2 --max-dcosy-avg=0.2 --show=1 --save=1 --filter --plot-avg
"""

class ImagingCutGenerator(genImage.genImage):
    """
    classdocs: This class allow to read the hdf5 file from dpath and create some images from it.

    run() method should be defined!

    Change history:

    Written by "Miguel Urco":mailto:murco@jro.igp.gob.pe  Jul 27, 2015
    """

    def __init__(self):

        genImage.genImage.__init__(self)

#         self.setOptions()
#
#         self.setScriptState()
#
#         self.verifyParameters()
#
#         self.createObjects()
#
#         self.createDirectories()
#
#         self.run()

    def setOptions(self):
        '''
        Using default arguments from genImage.genImage
        '''
        genImage.genImage.setOptions(self, usage)

        self.parser.add_option("", "--min-dcosx-avg", dest="dcosxmin_avg", metavar="<float>",
                          type="float", default=-0.055, help="minimum dcosx used for avg plot. By default is -0.055")

        self.parser.add_option("", "--max-dcosx-avg", dest="dcosxmax_avg", metavar="<float>",
                          type="float", default=+0.055, help="maximum dcosx used for avg plot. By default is 0.055")

        self.parser.add_option("", "--min-dcosy-avg", dest="dcosymin_avg", metavar="<float>",
                          type="float", default=-0.055, help="minimum dcosy used for avg plot. By default is -0.055")

        self.parser.add_option("", "--max-dcosy-avg", dest="dcosymax_avg", metavar="<float>",
                          type="float", default=+0.055, help="maximum dcosy used for avg plot. By default is 0.055")

        self.parser.add_option("", "--min-hei-avg", dest="hmin_avg", metavar="<float>",
                          type="float", default=None, help="minimum altitude used for avg plot")

        self.parser.add_option("", "--max-hei-avg", dest="hmax_avg", metavar="<float>",
                          type="float", default=None, help="maximum altitude used for avg plot")

        self.parser.add_option("", "--plot-dcosxcut", dest="plot_dcosxcut",
                          action="store_true", default=False, help="Plot dcosx cuts")

        self.parser.add_option("", "--plot-dcosycut", dest="plot_dcosycut",
                          action="store_true", default=False, help="Plot dcosy cuts")

        self.parser.add_option("", "--plot-rcut", dest="plot_rcut",
                          action="store_true", default=False, help="Plot range cuts")

        self.parser.add_option("", "--plot-xcut", dest="plot_xcut",
                          action="store_true", default=False, help="Plot xcuts")

        self.parser.add_option("", "--plot-ycut", dest="plot_ycut",
                          action="store_true", default=False, help="Plot ycuts")

        self.parser.add_option("", "--plot-zcut", dest="plot_zcut",
                          action="store_true", default=False, help="Plot zcuts")

        self.parser.add_option("", "--zstep", dest="zstep", metavar="<float>",
                          type="float", default=None, help="z step in Km used for zcut plotting. By default is 0.3Km")

        self.parser.add_option("", "--plot-avg", dest="plot_avg",
                          action="store_true", default=False, help="Plot avg")

        self.parser.add_option("", "--nx-subplot", dest="nx_subplot", metavar="<int>",
                          type="int", default=1, help="number of subplots on x axis when X, Y, Dcosx or Dcosy Cut is selected")

        self.parser.add_option("", "--ny-subplot", dest="ny_subplot", metavar="<int>",
                          type="int", default=3, help="number of subplots on y axis when X, Y, Dcosx or Dcosy Cut is selected")

        self.parser.add_option("", "--nx-subplot-z", dest="nx_subplot_z", metavar="<int>",
                          type="int", default=3, help="number of subplots on x axis when Z or Range Cut is selected")

        self.parser.add_option("", "--ny-subplot-z", dest="ny_subplot_z", metavar="<int>",
                          type="int", default=1, help="number of subplots on y axis when Z or Range Cut is selected")

        self.parser.add_option("", "--display-size", dest="display_size", metavar="""<"640x480+100+200">""",
                          type="string", default=None, help="display size used to plot X, Y, docsx or dcosy cuts")

        self.parser.add_option("", "--display-size-z", dest="display_size_z", metavar="""<"750x200+100+200">""",
                          type="string", default=None, help="display size used to plot Z or range cut")

        self.parser.add_option("", "--xcuts", metavar="<int list>",
                          type="str", default=None, help="int list, comma separated '1,3,9': x index list used for plotting x and dcosx cuts")

        self.parser.add_option("", "--ycuts", metavar="<int list>",
                          type="str", default=None, help="int list, comma separated '1,3,9': y index list used for plotting y and docsy cuts")

        self.parser.add_option("", "--zcuts", metavar="<int list>",
                          type="str", default=None, help="int list, comma separated '1,3,9': z index list used for plotting z cuts")

        self.parser.add_option("", "--rcuts", metavar="<int list>",
                          type="str", default=None, help="int list, comma separated '1,3,9': range index list used for plotting range cuts")

        self.parser.add_option("", "--find-layer", dest="find_layer",
                          action="store_true", default=False, help="Find a ionospheric layer in data")


    def setScriptState(self):

        genImage.genImage.setScriptState(self)

        (options, args) = self.parser.parse_args()

        self.dcosxmin_avg = options.dcosxmin_avg
        self.dcosxmax_avg = options.dcosxmax_avg
        self.dcosymin_avg = options.dcosymin_avg
        self.dcosymax_avg = options.dcosymax_avg
        self.hmin_avg = options.hmin_avg
        self.hmax_avg = options.hmax_avg

        self.plot_dcosxcut = options.plot_dcosxcut
        self.plot_dcosycut = options.plot_dcosycut
        self.plot_rcut = options.plot_rcut

        self.plot_xcut = options.plot_xcut
        self.plot_ycut = options.plot_ycut
        self.plot_zcut = options.plot_zcut

        self.zstep = options.zstep

        self.plot_avg = options.plot_avg

        self.nx_subplot = options.nx_subplot
        self.ny_subplot = options.ny_subplot
        self.nx_subplot_z = options.nx_subplot_z
        self.ny_subplot_z = options.ny_subplot_z

        self.display_size = options.display_size
        self.display_size_z = options.display_size_z

        self.buffer = None
        self.ibuffer = 0

        self.find_layer = options.find_layer

        if self.display_size is None:
            self.display_size = "%dx%d" %(self.nx_subplot*750, self.ny_subplot*250)

        if self.display_size_z is None:
            self.display_size_z = "%dx%d" %(self.nx_subplot_z*250, self.ny_subplot_z*250)

        if options.xcuts == None:
            self.my_xindex = None
        else:
            self.my_xindex = [int(i) for i in options.xcuts.split(",")]

        if options.ycuts == None:
            self.my_yindex = None
        else:
            self.my_yindex = [int(i) for i in options.ycuts.split(",")]

        if options.zcuts == None:
            self.my_zindex = None
        else:
            self.my_zindex = [int(i) for i in options.zcuts.split(",")]

        if options.rcuts == None:
            self.my_rindex = None
        else:
            self.my_rindex = [int(i) for i in options.rcuts.split(",")]

        if not (self.plot_avg or self.plot_dcosxcut or self.plot_dcosycut or self.plot_rcut or \
            self.plot_xcut or self.plot_ycut or self.plot_zcut):
            #Any plot has been selected
            self.parser.print_help()
            print("Any plot has been selected. Please select one kind of plot")
            sys.exit(-1)

    def findLayer(self, linear_data4D):

        len_buffer = 8

        #Average in colors
        avgData = imgproc.mask_average(linear_data4D, axis=3)
        #Average in Y
        avgData = imgproc.mask_average(avgData, axis=1)
        #Average in X
        avgData = imgproc.mask_average(avgData, axis=0)

        if self.buffer is None:
            self.buffer = numpy.empty( ( len_buffer, len(avgData) ) )
            self.ibuffer = 0

        self.buffer[self.ibuffer % len_buffer] = avgData

        mid_layer = imgproc.max_index(self.buffer[:self.ibuffer+1])
        print "my mid layer (index) is %d"  %mid_layer
        self.ibuffer += 1

        return numpy.arange(mid_layer-4, mid_layer+5)

    def plotData(self, startDatetime, endDatetime, filter = False):

        sts = self.readerObj.settings(self.dpath, startDatetime, endDatetime)

        if not sts:
            print("No data was found in %s between %s - %s" %(self.dpath, startDatetime, endDatetime))
            return 0

        tt = startDatetime.timetuple()
        year = tt[0]; doy = tt[7]

        self.createDoyPath(year, doy)

        ix_min, ix_max = misc.findMinMaxIndex(self.readerObj.dcosx, self.mindcosx, self.maxdcosx)
        iy_min, iy_max = misc.findMinMaxIndex(self.readerObj.dcosy, self.mindcosy, self.maxdcosy)
        ih_min, ih_max = misc.findMinMaxIndex(self.readerObj.heis, self.hmin, self.hmax)

        #Filtering on x, y and z
        dcosx = self.readerObj.dcosx[ix_min:ix_max+1]
        dcosy = self.readerObj.dcosy[iy_min:iy_max+1]
        heis = self.readerObj.heis[ih_min:ih_max+1]

        x = self.readerObj.x[ix_min:ix_max+1, iy_min:iy_max+1, ih_min:ih_max+1]
        y = self.readerObj.y[ix_min:ix_max+1, iy_min:iy_max+1, ih_min:ih_max+1]
        z = self.readerObj.z[ix_min:ix_max+1, iy_min:iy_max+1, ih_min:ih_max+1]

        if self.zstep is None:
            self.zstep = 2*(heis[1] - heis[0])

        ind_colors = imgproc.gen_color_indexes(nFFTPoints=self.readerObj.nfft, nbins=self.nbins)

        if self.plot_zcut:
            #Filtering on x and y, not z
            new_heis = imgproc.genZaxis(zmin=heis[0], zmax=heis[-1], zstep=self.zstep)
            new_x = numpy.multiply.outer(self.readerObj.dcosx2D, new_heis)[ix_min:ix_max+1, iy_min:iy_max+1,:]
            new_y = numpy.multiply.outer(self.readerObj.dcosy2D, new_heis)[ix_min:ix_max+1, iy_min:iy_max+1,:]

            if self.my_zindex == None:
                my_zindex = range(len(new_heis))
            else:
                my_zindex = self.my_zindex

        if self.my_xindex == None:
            my_xindex = range(len(dcosx))
        else:
            my_xindex = self.my_xindex

        if self.my_yindex == None:
            my_yindex = range(len(dcosy))
        else:
            my_yindex = self.my_yindex

        if self.my_rindex == None:
            my_rindex = range(len(heis))
        else:
            my_rindex = self.my_rindex

        while(1):

            datadB_4D = self.readerObj.readNextBlock()

            if datadB_4D is None:
                break

            #Filtering on x and y, not z. Z is used for zcuts
            datadB_4D = datadB_4D[ix_min:ix_max+1, iy_min:iy_max+1, :, :]

            linear_data_4D = imgproc.dBData2Linear(datadB_4D, remove_interf=False, remove_dc=False, fft_shift=self.fft_shift,
                                                   smooth=self.filter, percentile = self.percentile)

            linear_data_4D = imgproc.binFilter(linear_data_4D, ind_colors=ind_colors)

            linear_data_4D_filtered = linear_data_4D[:, :, ih_min:ih_max+1, :]

            if self.find_layer:
                my_rindex = self.findLayer(linear_data_4D_filtered)
                if self.plot_zcut:
                    my_zindex = (heis[my_rindex] - new_heis[0])/self.zstep
                    my_zindex = numpy.unique(my_zindex.astype(int))

                print "Plotting heights: ", heis[my_rindex]


            #Filtering on z
            rgb_data = imgplot.linearData2RGB(linear_data_4D_filtered, mindB=self.mindB, maxdB=self.maxdB)

            if self.plot_avg:
                imgplot.plotAverage( x1D = dcosx,
                                     y1D = dcosy,
                                     z1D = heis,
                                     linear_data = linear_data_4D_filtered,
                                    mindB = self.mindB,
                                    maxdB = self.maxdB,
                                    xmin = self.dcosxmin_avg,
                                    xmax = self.dcosxmax_avg,
                                    ymin = self.dcosymin_avg,
                                    ymax = self.dcosymax_avg,
                                    zmin = self.hmin_avg,
                                    zmax = self.hmax_avg,
                                     show=self.show,
                                     save=self.save,
                                     fpath = self.doycutpath,
                                     ddatetime = self.readerObj.filedate,
                                     ext = self.ext)

            if self.plot_dcosxcut:
                imgplot.plotDcosxCut(x1D = dcosx[my_xindex],
                                 y1D = dcosy,
                                 z1D = heis,
                                 rgb_data = rgb_data[my_xindex, :, :, :],
                                 show=self.show,
                                 save=self.save,
                                 fpath = self.doycutpath,
                                 ddatetime = self.readerObj.filedate,
                                 ext = self.ext,
                                 nx_subplot = self.nx_subplot,
                                 ny_subplot = self.ny_subplot,
                                 display_size = self.display_size)

            if self.plot_dcosycut:
                imgplot.plotDcosyCut(x1D = dcosx,
                                 y1D = dcosy[my_yindex],
                                 z1D = heis,
                                 rgb_data = rgb_data[:, my_yindex, :, :],
                                 show=self.show,
                                 save=self.save,
                                 fpath = self.doycutpath,
                                 ddatetime = self.readerObj.filedate,
                                 ext = self.ext,
                                 nx_subplot = self.nx_subplot,
                                 ny_subplot = self.ny_subplot,
                                 display_size = self.display_size)

            if self.plot_rcut:
                imgplot.plotRangeCut(x1D = dcosx,
                                 y1D = dcosy,
                                 z1D = heis[my_rindex],
                                 rgb_data = rgb_data[:, :, my_rindex, :],
                                 show=self.show,
                                 save=self.save,
                                 fpath = self.doycutpath,
                                 ddatetime = self.readerObj.filedate,
                                 ext = self.ext,
                                 nx_subplot = self.nx_subplot_z,
                                 ny_subplot = self.ny_subplot_z,
                                 display_size = self.display_size_z)

            if self.plot_xcut:
                imgplot.plotXCut(x3D = x[my_xindex, :, :],
                                 y3D = y[my_xindex, :, :],
                                 z3D = z[my_xindex, :, :],
                                 rgb_data = rgb_data[my_xindex, :, :, :],
                                 show=self.show,
                                 save=self.save,
                                 fpath = self.doycutpath,
                                 ddatetime = self.readerObj.filedate,
                                 ext = self.ext,
                                 nx_subplot = self.nx_subplot,
                                 ny_subplot = self.ny_subplot,
                                 display_size = self.display_size)

            if self.plot_ycut:
                imgplot.plotYCut(x3D = x[:, my_yindex, :],
                                 y3D = y[:, my_yindex, :],
                                 z3D = z[:, my_yindex, :],
                                 rgb_data = rgb_data[:, my_yindex, :, :],
                                 show=self.show,
                                 save=self.save,
                                 fpath = self.doycutpath,
                                 ddatetime = self.readerObj.filedate,
                                 ext = self.ext,
                                 nx_subplot = self.nx_subplot,
                                 ny_subplot = self.ny_subplot,
                                 display_size = self.display_size)

            if self.plot_zcut:
                #Filtering on x and y, not z. linear_data_4D is already filtered on x and y
                ztemp = self.readerObj.z[ix_min:ix_max+1, iy_min:iy_max+1,:]

                new_linear_data_4D = imgproc.translateSphereArray2XYZ(ztemp, linear_data_4D, new_heis)
                rgb_data_t = imgplot.linearData2RGB(new_linear_data_4D, mindB=self.mindB, maxdB=self.maxdB)

                imgplot.plotZCut(x3D = new_x[:, :, my_zindex],
                                 y3D = new_y[:, :, my_zindex],
                                 z1D = new_heis[my_zindex],
                                 rgb_data = rgb_data_t[:, :, my_zindex, :],
                                 show=self.show,
                                 save=self.save,
                                 fpath = self.doycutpath,
                                 ddatetime = self.readerObj.filedate,
                                 ext = self.ext,
                                 nx_subplot = self.nx_subplot_z,
                                 ny_subplot = self.ny_subplot_z,
                                 display_size = self.display_size_z)

            # fig = plt.figure("Test 3D")
            # ax = fig.gca(projection='3d')
            #fig = plt.figure()
            #ax = fig.add_subplot(111, projection='3d')
            #print "shape 3D ",numpy.shape(rgb_data)
            #plt.imshow(rgb_data)
            #print "shape 3D x",x
            #print "shape 3D y",y # numpy.shape(dcosy)
            #print "shape 3D z",heis #numpy.shape(heis)
            # X,Y = numpy.meshgrid(self.readerObj.dcosx,self.readerObj.dcosy)
            # x = numpy.asarray(dcosx)
            # y = numpy.asarray(dcosy)
            # z = numpy.asarray(heis)
            # x = dcosx.T
            # y = dcosy.T
            # z = heis.T
            #X = x[:,:,0]
            #Y = y[:,:,0]
            #Z = z

            #a, b, c, d =

            #fig = go.Figure(data = [trace], layout = layout)
            #a, b, c = np.meshgrid(datadB_4D[], datadB_4D, datadB_4D)
            #fig = go.Figure(data=[go.Surface(z=datadB_4D[:,:,:,0])])
            #fig.show()
            #ax.scatter(x[:,],y,z)
            #plt.plot(x[:,:,0])
            #ax.scatter(rgb_data, rgb_data, rgb_data, c = rgb_data.flat )
            #surf = ax.plot_surface(x,y,self.readerObj.heis)
            #ax.plot_surface(X,Y,Z)
            #surf = ax.plot_trisurf(x, y, z, cmap=cm.jet, linewidth=0.1)
            #ax.scatter(dcosx, dcosy, heis)
            # Plot the surface.
            #plt.show()

        imgplot.close_all()

        print "%d files were processed" %(self.readerObj.indFile)

        if self.readerObj.indFile == 0:
            return 0

        return 1

    def run(self):

        initialDoy = datetime.datetime(self.startYear,1,1,0,0,0) + datetime.timedelta(self.startDoy-1)
        finalDoy = datetime.datetime(self.endYear,1,1,23,59,59) + datetime.timedelta(self.endDoy-1)
        thisDoy = initialDoy

        while thisDoy.date() <= finalDoy.date():

            startDatetime = thisDoy + datetime.timedelta(0,0,0,0,self.minute[0],self.hour[0])
            endDatetime = thisDoy + datetime.timedelta(0,0,0,0,self.minute[-1],self.hour[-1])

            sts = self.plotData(startDatetime, endDatetime)

            thisDoy += datetime.timedelta(1)

##########################################
##########################################
##########################################
##### THE MAIN SCRIPT STARTS HERE ########
##########################################
##########################################
##########################################

if __name__ == '__main__':

    ImagingCutGenerator()
