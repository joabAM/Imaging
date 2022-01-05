'''
Created on May 17, 2010

@author: murco
'''

import sys, os, glob
import numpy
import time, datetime
import traceback
import h5py

def gotoxy(x, y, arg=''):
    ESC = '\033'
    CSI = ESC + "["
    sys.stdout.write( CSI + str(y) + ";" + str(x) + 'H' + str(arg))
    
class readImaging:
    """
    classdocs: This class allow to read the bin file from dpath and to get the datablock in a numpy array. 

    Change history:

    Written by "Miguel Urco":mailto:murco@jro.igp.gob.pe  Set 9, 2010
    """

    def __init__(self):
        """
        Constructor
        """
        self.pathIsSet = False
        self.headerIsSet = False
        self.fileIsSet = False
        self.indFile = -1
        
        self.nx = None
        self.ny = None      
        self.nfft = None
        self.heis = None
        self.nheis = None
        self.dcosx = None
        self.dcosy = None
        self.angx = None
        
        self.nchan = 0
        
        self.nrows = None
        self.ncolumns = None
        
        self.ipp_seconds = None
        self.ncoh = 1
        self.incoh = 1
        
        self.bw = None
        self.vel_range = None
        
        self.version = None
        
        self.__yindex = [0]
        
        self.nPeaks_rList = numpy.arange(0)
        self.peakPower_rList = numpy.arange(0)
        
        self.nPeaks_gList = numpy.arange(0)
        self.peakPower_gList = numpy.arange(0)
        
        self.nPeaks_bList = numpy.arange(0)
        self.peakPower_bList = numpy.arange(0)

        self.nPeaksList = numpy.arange(0)
        self.peakPowerList = numpy.arange(0)
        self.peakPowerdBList = numpy.arange(0)
        
        self.peakPowerList0 = numpy.arange(0)
        self.peakPowerdBList0 = numpy.arange(0)
        
    def settings(self, dpath, startTime, endTime=None, online=False, ext=".h5", predir="BIN", set=None,
                 yOffset=0, rotate_image=True):
        """
        
        """
        if set==None: set=-1
        
        self.pathIsSet = False
        self.dpath = dpath
        self.predir = predir
        self.ext = ext
            
        #Reading the first header available
        thisTime = startTime
        if endTime == None: endTime = datetime.datetime.today() + datetime.timedelta(1)
        
        while(thisTime.date() <= endTime.date()):
            tt = thisTime.timetuple()
            hdrFile = self.readHeader(tt[0], tt[7], rotate_image=rotate_image)
            
            if hdrFile != None:
                break
            
            thisTime += datetime.timedelta(1)
        
        if hdrFile == None:
            #print "Any header file has not been found for this day"
            return 0
        
        filenameList, filedateList = self.getFileList(dpath, startTime, endTime=endTime)
        
        #If online is enabled to select last file only
        if online==True:
            if len(filenameList)>0:
                filenameList = [filenameList[set]]
                filedateList = [filedateList[set]]
        
        self.filenameList = filenameList
        self.filedateList = filedateList
        self.indFile = set
        self.nfilesRead = 0
        
        
        self.startTime = startTime
        self.endTime = endTime
        self.online = online
        self.pathIsSet = True
        
        return 1
    
    def __getDateFromPath(self, path):
        
        thisDate = path[-7:]
        year = int(thisDate[0:4])
        yday = int(thisDate[4:7])
        
        return year, yday
        
    def __getTimeFromFile(self, filename):
        
        file, ext = os.path.splitext(filename)
        lenf = len(file)
        
        thisTime = None
        
        if lenf == 13 or lenf == 9:  #imgHHMMSSiiii or imgHHMMSS
            thisTime = file[3:]
            
        if lenf == 22 or lenf == 17:  #imgYYYYDDD_HHMMSS_iiii or imgYYYYDDD_HHMMSS
            thisTime = file[11:]
        
        if thisTime == None:
            raise ValueError, "Unrecognized filename format '%s'" %filename
        
        hour = int(thisTime[0:2])
        min = int(thisTime[2:4])
        sec = int(thisTime[4:6])
        return hour, min, sec
    
    def getDatetime(self, thisFile):
        
        path, filename = os.path.split(thisFile)
        year, yday = self.__getDateFromPath(path)
        hour, min, sec = self.__getTimeFromFile(filename)        
        thisDatetime = datetime.datetime(year,1,1,hour,min,sec)+datetime.timedelta(yday-1)
        return(thisDatetime)

    def findFiles(self, dpath, thisTime):
        
        tt = thisTime.timetuple()
        year = tt[0]
        doy = tt[7]
        inputString = os.path.join(dpath, "%s%04d%03d" %(self.predir,year,doy), "img*%s" %self.ext)

        files = glob.glob(inputString)
        files.sort()
        
        return files
    
    def __getFilesIntoRange(self,
                            files,
                            startTime = datetime.datetime(datetime.MINYEAR,12,31,23,59,59),
                            endTime = datetime.datetime(datetime.MAXYEAR,12,31,23,59,59) ):
        
        filenameList = []
        filedateList = []
        for thisFile in files:
            thisDatetime = self.getDatetime(thisFile)
            if (thisDatetime>=startTime and thisDatetime<=endTime):
                filenameList.append(thisFile)
                filedateList.append(thisDatetime)
        
        return (filenameList, filedateList)       
       
    def getFileList(self, dpath, startTime, endTime=None):
        
        #searching only for this day
        
        fileList = []
        thisTime = startTime
        if endTime == None: endTime = datetime.datetime.today() + datetime.timedelta(1)
        
        #Searching files for everyday
        while(thisTime.date() <= endTime.date()):
            fileList.extend(self.findFiles(dpath, thisTime))
            thisTime += datetime.timedelta(1)
        
        fileList.sort()
        #Filtering files from startTime to endTime
        return self.__getFilesIntoRange(fileList, startTime, endTime)

    
    def __readH5Header_v2(self, filename, rotate_image=True):
           
        '''
        Reading the header file
        '''
        try:
            f = h5py.File(filename,"r")
        except:
            return None
        
        dset = f.get('Version')
        
        if dset == None:
            f.close()
            raise ValueError, "The file version has not been found"
        
        g = f.get("/metadata/Processing parameters", default=None)
        
        nx = g.get('Nx', default=None).value
        ny = g.get("Ny", default=None).value
        nheis = int(g.get("Number of heights", default=None).value)
        nfft  = g.get("Number of FFT points", default=None).value
        hrange = g.get("Height range", default=None).value
        rotangle = g.get("Rotation angle", default=None).value
        scalex = g.get("dcosx", default=None).value
        scaley = g.get("dcosy", default=None).value
        dcosy_offset = g.get("dcosy offset", default=None).value
        nchan  = g.get("Number of channels", default=None).value
        
        try:
            ncoh = f.get("NCOH", default=None).value
            nincoh = f.get("NINCOH", default=None).value
        except:
            ncoh = 1
            nincoh = 1
            
        g = f.get("/metadata/Array dimension", default=None)
        
        nrows = g.get("Rows", default=None).value
        ncolumns = g.get("Columns", default=None).value
        
        h = f.get("/metadata/Radar parameters",default=None)
        
        deltah = numpy.round(h.get("Delta H", default=None).value,4)
        wavelength = h.get("Wavelength", default=None).value
        ipp = h.get("IPP", default=None).value
        #Km -> seconds : 150Km -> 1ms
        ipp_seconds = ipp*1e-3/150
        
        f.close()
        
        if not rotate_image:
            rotangle = 0
            
        dcosx = numpy.outer((numpy.arange(nx) - nx/2.0), numpy.ones(ny))*scalex/nx
        dcosy = numpy.outer(numpy.ones(nx), (numpy.arange(ny) - ny/2.0))*scaley/ny + dcosy_offset
        
        #Rotation
        dcosxp = dcosx*numpy.cos(rotangle) - dcosy*numpy.sin(rotangle)
        dcosyp = dcosx*numpy.sin(rotangle) + dcosy*numpy.cos(rotangle)
        dcoszp = numpy.sqrt(1 - numpy.power(dcosxp,2) - numpy.power(dcosyp,2))
        
        heis = numpy.arange(nheis)*deltah + hrange[0]
        
        self.nx = nx
        self.ny = ny        
        self.nfft = nfft
        self.heis = heis
        self.nheis = nheis
        self.deltah = deltah
        self.dcosx = dcosx[:,ny/2]
        self.dcosy = dcosy[nx/2,:]
        
        self.dcosx2D = dcosxp
        self.dcosy2D = dcosyp
        self.dcosz2D = dcoszp
        
        self.x = numpy.multiply.outer(dcosxp, heis)
        self.y = numpy.multiply.outer(dcosyp, heis)
        self.z = numpy.multiply.outer(dcoszp, heis)
        
        self.nchan = nchan
        
        self.nrows = nrows
        self.ncolumns = ncolumns
        
        self.ipp_seconds = ipp_seconds
        self.ncoh = ncoh
        self.incoh = nincoh
        
        self.bw = 0.5/(ipp_seconds*ncoh)
        self.vel_range = self.bw*wavelength
        
        return filename
    
    def __readH5DataFile_v2(self, filename):
        '''
        Reading the binary data file
        
        Input:
        
            filename    :    input hdf5 file where the data is stored
            
            
        Output:
            
            marray    :    mask numpy array with the data
            
        '''
        self.fileIsSet = False
        if type(filename)==type([]): filename = filename[0]
        
        path, file = os.path.split(filename)
        
        #Reading header file
        if path != self.headPath:
            year, yday = self.__getDateFromPath(path)
            if self.readHeader(year, yday) == None: return None
        
        sys.stdout.write("\nReading %s\n" %filename)
        try:
            f = h5py.File(filename,"r")
        except:
            err = traceback.format_exception(sys.exc_info()[0],
                                             sys.exc_info()[1],
                                             sys.exc_info()[2])
            ee = ""
            for e in err: ee += e
            sys.stdout.write( ee+ '\n')
            sys.stdout.write( "Can't open the hdf5 file: %s\n" %filename)
            return None
        
#         version = None
#         
#         dset = f.get('Version')
#         
#         if dset != None:
#             version = round(float(dset.value), 1)
#             
#         if version != self.version:
#             raise ValueError, "The data file (v%3.2f) is not in agreement with the header file (v%3.2f)" %(version, self.version)
        
        g = f.get("/metadata/Original Array dimensions", default=None)
        
        nrows = g.get("Rows", default=None).value
        ncolumns = g.get("Columns", default=None).value
        
        #Comparing data size from the header file and array size from the data file
        if (nrows, ncolumns) != (self.nrows, self.ncolumns):
            raise ValueError, "The data size from the data file (%d, %d) is not in agreement with \
            the data size from the header file (%d, %d)" %(nrows,
                                                           ncolumns,
                                                           self.nrows,
                                                           self.ncolumns) 
        
        g = f.get("/metadata/", default=None)
        
        row_index = g.get("Row index array", default=None).value
        
        g = f.get("/data/", default=None)
        d = g.get("image", default=None)
        
        array = numpy.zeros((nrows,ncolumns)) + numpy.NAN
        
        if d == None:
            f.close()
            return array
        
        subarray = d.value
        
        f.close()
        
        #Data saved in dBs 
        subarray = numpy.where(subarray==-99.0, numpy.NAN, subarray)
        
        array[row_index,:] = subarray
        
        array = numpy.reshape(array,(self.nfft,self.nheis,self.ny,self.nx))
        array = numpy.transpose(array,(3,2,1,0))
        
        self.fileIsSet = True
        
        self.array = array
        
        return array
    
    
    def getHeaderFilename(self, year, doy):
        '''
        Setting the header
        '''
        self.headerIsSet = False
        
        headPath = os.path.join(self.dpath,"%s%04d%03d" %(self.predir, year, doy))
        file = "i%04d%03d.hdr" %(year,doy)
        file = "head%04d%03d.h5" %(year,doy)
        
        filename = os.path.join(headPath,file)
        
        exists = os.path.exists(filename)
        
        if not(exists):
            return None
        
        self.headYear = year
        self.headDoy = doy
        self.headFile = file
        self.headPath = headPath
        self.headFilename = filename
        self.headerIsSet = True
        
        return filename
    
    def getFileVersion(self, filename):
        
        version = None
        
        f = h5py.File(filename,"r")
        
        dset = f.get('Version', default=None)
                
        if dset != None:
            try:
                version = str(dset.value[0])
            except:
                version = str(dset.value)
            
            
            version = round(float(version[0:3]))
                
            return version
        
        #File version 1.0
        dset = f.get('imaging', default=None)
        
        if dset != None:
            version = 1.0
            return version
        
        raise ValueError, "The header file is not a recognized header file version"
    
    def readHeader(self, year, doy, rotate_image=True):
        
        filename = self.getHeaderFilename(year, doy)
        
        if filename == None:
            return None
        
        self.version = self.getFileVersion(filename)
        
        if int(self.version) == 2.0:
            return self.__readH5Header_v2(filename, rotate_image=rotate_image)
        
        raise ValueError, "The file version is not recognized %f" %(self.version)
    
    def readDataFile(self, filename):
        
        if int(self.version) == 2.0:
            return self.__readH5DataFile_v2(filename)
        
        raise ValueError, "The file version is not recognized %f" %(self.version)
    
    def readNextBlock(self, seconds=10, xsreen=0, yscreen=10, test3D=False, test_cal=0):
        
        """
        return:
            4D array_like 
            
        """
        if self.pathIsSet==False: return None
        
        filename, filedate = self.findNextFile(seconds, xsreen, yscreen)
        
        if not(filename): return None
        
        datablk = self.readDataFile(filename)
        self.filename = filename
        self.filedate = filedate
        self.utc = (filedate - datetime.datetime(1970,1,1)).total_seconds()
        self.image = datablk
        
        if test3D:
            self.testImage()
        
        if test_cal:
            print "testing cal enable"
            self.analyzeData(datablk)
            
        return datablk
    
    def findNextFile(self, seconds, xsreen, yscreen):
        
        #Number of files found on the data path
        ncurrentfiles = len(self.filenameList)
        
        #File index of the last read file 
        self.indFile += 1
        
        #Verifying number of read files
        if self.indFile < ncurrentfiles:
            
            filename = self.filenameList[self.indFile]
            filedate = self.filedateList[self.indFile]        

            return filename, filedate
        
        nTimes = 1
        nomoreFiles = False
        msg = "|/-\\"
        
        while(1):
            
            newfilenameList, newfiledateList = self.getFileList(self.dpath, self.startTime, self.endTime)
            
            ncurrentfiles = len(newfilenameList)
            
            if self.indFile < ncurrentfiles:
                
                #Find out index of last file on current list
                ind = newfilenameList.index(self.filenameList[self.indFile-1])
                if ind < 0:
                    sys.stdout.write( "some files have been deleted\n")
                    exit(-1)
                
                self.filenameList.extend(newfilenameList[ind+1:])
                self.filedateList.extend(newfiledateList[ind+1:])
                
                break
            
            if nTimes > 3:
                nonomoreFiles = True
                break
            
            if not self.__isProcRunning(nTimes, seconds):
                nomoreFiles = True
                break
            
#             gotoxy(xsreen, yscreen)
            sys.stdout.write("%d: waiting %d seconds to read the next file [%s]\n" % (nTimes, seconds, msg[nTimes % 4]))
            time.sleep(seconds)
            
            nTimes += 1
            
        
#         gotoxy(xsreen, yscreen)
#         sys.stdout.write(" "*50 + '\n')
        
        if nomoreFiles:
            return None, None
        
        filename = self.filenameList[self.indFile]
        filedate = self.filedateList[self.indFile]        

        return filename, filedate
    
    def __isProcRunning(self, nTimes=None, seconds=None):
        
        maxNTimes = 3
#         print "maxNTimes = ", maxNTimes
        if nTimes >= maxNTimes:
            print "\n"*3
            print "many times waiting for new data %d" %maxNTimes
            return False
        
        
        return True
    
    def resetCounters(self):
        
        self.indFile = -1
        
    def testImage(self, shift=True):
        
        import visual
        import visual.graph
        
        mindB = 5
        maxdB = 35
        
        array = self.array.copy()
        
        array = 10*numpy.log10(array)
        array = numpy.clip(array, mindB, maxdB)
        array = numpy.where(numpy.isnan(array), mindB, array)
        
        array = (array - mindB)/(maxdB-mindB)
        
        ev = visual.scene.mouse.getclick()
        
        for i in range(self.nx/2-80, self.nx/2+80):
            for j in range(10) + self.__yindex[0]-5:
                for k in range(self.nheis):
                    if shift == True:
                        color1 = array[i,j,k,:].take([-1, 0, 1])
                    else:
                        color1 = array[i,j,k,self.nfft/2-1:self.nfft/2-1+3]
                    
                    if color1.any() == False:
                        continue
                    
                    pos1 = (i-self.nx/2, k-self.nheis/2, (j-self.__yindex[0])*20)
                    visual.graph.points(pos=pos1,
                                    size=5,
                                    color=color1)
    
    def __getPeaks(self, data):
        
        data_r = data.flatten()
        peakind_r = numpy.r_[True, data_r[1:] > data_r[:-1]] & numpy.r_[data_r[:-1] > data_r[1:], True]
        nPeaks_r = len( numpy.where(peakind_r)[0] )
        totalPower_r = numpy.sum(data_r)
        
        return nPeaks_r, totalPower_r
        
    def analyzeData(self, datablk):
        
#         shape = (self.nx,self.nheis,self.nfft)
        
        mindB = -50
        maxdB = 50
        
        totalPower0 = numpy.nanmean(datablk)
 
        dataDB = 10*numpy.log10(datablk)
        totalPowerdB0 = numpy.nansum(dataDB)
        
        data = numpy.clip(datablk, numpy.power(10.0,mindB/10.0), numpy.power(10.0,maxdB/10.0))
        
        data = numpy.where(numpy.isnan(data), numpy.power(10.0,mindB/10.0), data)
        
#         totalPowerX = numpy.mean(data)
        
        dataDB = 10*numpy.log10(data)
        totalPowerdB = numpy.sum(dataDB)
        
#         data = numpy.convolve(data, numpy.ones(4), mode="same")
# 
        nPeaks_r, totalPower_r = self.__getPeaks(dataDB[:,:,-1])
         
        nPeaks_g, totalPower_g = self.__getPeaks(dataDB[:,:,0])
         
        nPeaks_b, totalPower_b = self.__getPeaks(dataDB[:,:,1])
 
        nPeaks_m, totalPower_m = self.__getPeaks(dataDB[:,:,2])
#         
#         self.nPeaks_rList = numpy.append(self.nPeaks_rList, nPeaks_r)
#         self.peakPower_rList = numpy.append(self.peakPower_rList, totalPower_r)
#         
#         self.nPeaks_gList = numpy.append(self.nPeaks_gList, nPeaks_g)
#         self.peakPower_gList = numpy.append(self.peakPower_gList, totalPower_g)
#         
#         self.nPeaks_bList = numpy.append(self.nPeaks_bList, nPeaks_b)
#         self.peakPower_bList = numpy.append(self.peakPower_bList, totalPower_b)
#         
        self.nPeaksList = numpy.append(self.nPeaksList, nPeaks_r + nPeaks_g + nPeaks_b + nPeaks_m)
        
        self.peakPowerdBList = numpy.append(self.peakPowerdBList, totalPowerdB)
        
#         self.peakPowerList = numpy.append(self.peakPowerList, totalPowerX)
        
        self.peakPowerdBList0 = numpy.append(self.peakPowerdBList0, totalPowerdB0)
        
        self.peakPowerList0 = numpy.append(self.peakPowerList0, totalPower0)
    
    def __normalize(self, array):
        
        array2 = (array - numpy.min(array))
        array2 = array2/numpy.max(array2)
        
        return array2
        
    def plotAnalyzedData(self):
        
        import matplotlib
        matplotlib.use("TkAgg")
        import pylab as pl

        if len(self.peakPowerdBList) < 1:
            return
        
#         pl.subplot(221)
#         pl.plot(self.nPeaks_rList, 'r')
#         pl.plot(self.nPeaks_gList, 'g')
#         pl.plot(self.nPeaks_bList, 'b')
#         
#         pl.subplot(223)
#         pl.plot(self.peakPower_rList, 'r')
#         pl.plot(self.peakPower_gList, 'g')
#         pl.plot(self.peakPower_bList, 'b')
        phase_step = numpy.round(2.0*numpy.pi/len(self.peakPowerdBList) , 2)
        phase = numpy.arange(-numpy.pi, numpy.pi, phase_step)
        
        fig = pl.figure(1)
        
        ax = fig.add_subplot(211)
        ax.plot(phase, self.nPeaksList, 'mx-', label="Number of peaks")
        ax.legend()
        
#         ax.set_title('Number of Peaks')
#         ax.set_xlabel('Phase -PI to PI')
        ax.set_ylabel('Units')

        ax = fig.add_subplot(212)
        ax.plot(phase, self.__normalize(self.peakPowerdBList), 'mx-', label="Total Power")
#         pl.plot(phase, self.__normalize(self.peakPowerList), 'yx-')
        
        ax.plot(phase, self.__normalize(self.peakPowerdBList0), 'gx-', label = "Power over snrth")
#         pl.plot(phase, self.__normalize(self.peakPowerList0), 'bo-')
        
        ax.legend()
#         ax.set_title('Power')
        ax.set_xlabel('Phase -PI to PI')
        ax.set_ylabel('Normalized')
        
        #pl.show()
        figfile = os.path.join(self.dpath,"%s_%04d%02d%02d_%02d.png" %("ImagingStadistics",
                                                                   self.filedate.year, 
                                                                   self.filedate.month,
                                                                   self.filedate.day,
                                                                   self.nchan))
        fig.show()
        fig.savefig(figfile)
        