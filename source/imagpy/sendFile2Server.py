import os, sys
import ftplib

def isAImagingPath(path):

    if not path[0] == 'i' :
        return False

    try:
        year = int(path[1:5])
    except:
        return False

    try:
        doy = int(path[5:8])
    except:
        return False

    try:
        set = int(path[9:12])
    except:
        return False

    return True

def isRTDIFile(filename):
    if not filename[-3:] == 'png':
        return False

    try:
        doy = int(filename[4:6])
    except:
        return False

    if int(doy)>365 or int(doy)<0:
        return False

    return True
def isVideoFile(filename):
    if not filename[-3:] == 'mp4':
        return False

    try:
        doy = int(filename[11:13])
    except:
        return False

    if int(doy)>365 or int(doy)<0:
        return False

    return True

def isAImagingFile(filename):

    if not filename[0:3] == 'img':
        return False

    try:
        doy = int(filename[3:6])
    except:
        return False

    try:
        time = int(filename[7:13])
    except:
        return False

    if not filename[13:17] == '.png':
        return False

    return True

def getDateImage(path):

    year = int(path[0:4])
    doy = int(path[4:7])

    return year, doy

def getDateFromPath(path):

    year = int(path[1:5])
    doy = int(path[5:8])
    set = int(path[9:12])

    return year, doy


def getFtpFilename(year, doy, ftppath, ftpprefix='ccf-spc', ftpext='.png'):

    newfilename =""

    thisfile = "%s%04d%03d%s" %(ftpprefix, year, doy, ftpext)

    ftpfilename = os.path.join(ftppath, '%s' %year, thisfile)
    return ftpfilename



def getImagingFileList(path, ftppath):

    directoryList = os.listdir(path)
    directoryList.sort()
    print directoryList
    fileList = []
    ftpFileList = []

    print "Searching imaging files into %s ..." %(path)
    for doyPath in directoryList:

        if not isAImagingPath(doyPath):
            continue

        files = os.listdir(os.path.join(path, doyPath))

        print "%d files were found into %s" %(len(files), doyPath)

        validfiles = []

        for thisfile in files:

            if not isAImagingFile(thisfile):
                continue

            validfiles.append(thisfile)

        if validfiles == []:
            continue

        print "%d imaging files were found into %s" %(len(validfiles), doyPath)

        if len(validfiles) < 300:
            print "too few files: discarding this directory (%s)" %(doyPath)
            continue

        validfiles.sort()

        #Tomar el ultimo archivo y enviarlo
        thisfile = validfiles[-1]

        year, doy = getDateFromPath(doyPath)

        ftpfile = getFtpFilename(year, doy, ftppath)

        fileList.append(os.path.join(path, doyPath, thisfile))
        ftpFileList.append(ftpfile)

    print
    print "%d files were selected to send to the server" %(len(fileList))

    return fileList, ftpFileList

def getImagingFileList2(path, ftppath):
    imagePath = path+"FIG_FILES/"
    videoPath = path+"MOVIES/"
    directoryImageList = os.listdir(imagePath)
    directoryImageList.sort()
    directoryVideoList = os.listdir(videoPath)
    directoryVideoList.sort()

    videoList = []
    imagList = []
    ftpImageFileList = []
    ftpVideoFileList = []

    days = max(len(directoryImageList), len(directoryVideoList))
    print("Total days: ",days)
    for n in range(days):
        if  n < len(directoryImageList):
            if  isRTDIFile(directoryImageList[n]):
                try:
                        year, doy = getDateImage(directoryImageList[n])
                        ftpfile = getFtpFilename(year, doy, ftppath)
                        imagList.append(os.path.join(imagePath, directoryImageList[n]))
                        ftpImageFileList.append(ftpfile)
                except:
                    print("Image files are less than videos")

        if  n < len(directoryVideoList) :

            if  isVideoFile(directoryVideoList[n]):
                try:
                        videoList.append(os.path.join(videoPath, directoryVideoList[n]))
                        videoName = directoryVideoList[n]
                        year = videoName[7:11]
                        ftpVideoFileList.append(ftppath+year+"/"+videoName)
                except:
                    print("Video files are less than image")

    return videoList, imagList,ftpImageFileList, ftpVideoFileList


def sendFile2Server(filename, ftpfilename, ftpserver):

    print 'Sending %s as %s to server' %(os.path.split(filename)[-1],
                                     os.path.split(ftpfilename)[-1])

    f = open(filename,'rb')                     # file to send
    ftpserver.storbinary('STOR ' + ftpfilename, f)      # Send the file
    f.close()                                       # Close file and FTP

def sendFiles2Server(fileList, ftpFileList, ftpServer, ftpUser, ftpPass):

    #Opening the ftpserver
    try:
        server = ftplib.FTP(ftpServer, ftpUser, ftpPass) # Connect
    except:
        print 'Unable to connect to Server'
        sys.exit(-1)

    print "Sending %d files to the ftp server (%s) ..." %(len(fileList), ftpServer)
    for i in range(len(fileList)):

        filename = fileList[i]
        ftpfilename = ftpFileList[i]
	print 'sending',ftpfilename
        sendFile2Server(filename, ftpfilename, server)


if __name__ == "__main__":

    path = "/home/soporte/Documentos/Imaging/OUTPUTmp4/"
    ftpServer = "jro-db.igp.gob.pe"
    ftpUser = "wmaster"
    ftpPass = "mst2013vhf"
    ftpPath = "database/juliadata/julia_imaging/"

    movieList, rtiList, ftpImageList, ftpVideoFileList = getImagingFileList2(path, ftpPath)

    print("Sending images...")
    sendFiles2Server(rtiList, ftpImageList, ftpServer, ftpUser, ftpPass)
    print("...")
    print("Sending videos...")
    sendFiles2Server(movieList, ftpVideoFileList, ftpServer, ftpUser, ftpPass)
