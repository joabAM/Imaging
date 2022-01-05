#!/usr/bin/python
'''
Created on Oct 20, 2010

@author: roj-idl71
'''
usage = """
batch_imaging.py is a script used to process imaging data.

Required arguments:

    None

Optional arguments:
    
    --iniFile        :    set this to set the ini file
    --help           :    print help

Example:

    batch_imaging.py --iniFile=/home/myuser/imaging.ini
"""

import os, sys
import subprocess
import datetime, time
import getopt
import ConfigParser

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


class MyConfigParser(ConfigParser.ConfigParser):
    
    def getintList(self, section, option):
        
        str = self.get(section, option)
        list = str.split(',')
        intList = [int(x) for x in list]
        
        return intList
    
    def getfloatList(self, section, option):
        
        str = self.get(section, option)
        list = str.split(',')
        floatList = [float(x) for x in list]
        
        return floatList
    
class Imaging:
    
    CMDIMAGPROC = "procImaging"
    
    CMDIMAGGEN = "genImagPlots.py"
    
    OPTARGDICT = {'dpath':['Required', 'str'],
                  'ppath':['Required', 'str'],
                  'year':['Required', 'intList'],
                  'doy':['Required', 'intList'],
                  'enaproc':['Required', 'boolean'],
                  'enagen':['Required', 'boolean'],
                  'hour':['Optional', 'intList'],
                  'minute':['Optional', 'intList'],
                  'dbrange':['Optional', 'floatList'],
                  'heirange':['Optional', 'floatList'],
                  'online':['Optional', 'boolean'],
#                   'unattended':['Optional', 'boolean'],
#                   'onlineHour':['Optional', 'int'],
#                   'onlineMinute':['Optional', 'int'],
                  'set':['ImageProcessing', 'int'],
                  'label':['ImageProcessing', 'str'],
                  'navg':['ImageProcessing', 'int'],
                  'nbavg':['ImageProcessing', 'int'],
                  'factoravg':['ImageProcessing', 'float'],
                  'calibrationfile':['ImageProcessing', 'str'],
                  'snrth':['ImageProcessing', 'float'],
                  'nthreads':['ImageProcessing', 'int'],
                  'channels':['ImageProcessing', 'str'],
#                   'testcal':['ImageProcessing', 'int'],
#                   'chanindex':['ImageProcessing', 'int'],
#                   'autocal':['ImageProcessing', 'int'],
                  'show':['ImageGeneration', 'boolean'],
                  'save':['ImageGeneration', 'boolean'],
                  'mode':['ImageGeneration', 'int'],
#                   'quality':['ImageGeneration', 'str'],
                  'fullrtdi':['ImageGeneration', 'boolean'],
                  'filter':['ImageGeneration', 'boolean'],
                  'beamfilter':['ImageGeneration', 'boolean'],
                  'dcosxrange':['ImageGeneration', 'floatList'],
                  'dcosyrange':['ImageGeneration', 'floatList'],
#                   'nbeams':['ImageGeneration', 'str'],
#                   'heibeam':['ImageGeneration', 'str'],
#                   'ftpserver':['ImageGeneration', 'str'],
#                   'ftpuser':['ImageGeneration', 'str'],
#                   'ftppass':['ImageGeneration', 'str'],
#                   'ftppath':['ImageGeneration', 'str'],
#                   'ftpfile':['ImageGeneration', 'str'],
#                   'ftpprefix':['ImageGeneration', 'str'],
#                   'yoffset':['ImageGeneration', 'float']
                  }
    __WAIT = 5
    
    dpath = None
    ppath = None
    binpath = None
    sendFile = False
    
    def __init__(self, configfile="imaging.ini"):
        
        print
        print "Using the '%s' file as config file" %(configfile)
        
        self.configfile = configfile
        
        self.__getPlatform()
        self.defaultScriptState()
        self.setScriptState()
        self.verifyScriptState()
        
        if self.unattended:
            self.runonline()
        else:
            self.run()
    
    def __getPlatform(self):
        
        import platform
        
        self.__platform = platform.system().lower()
    
    def __getTerminal(self):
        
        if self.__platform == "linux":
            terminal = 'gnome-terminal -e '
        elif self.__platform == "darwin":
            terminal = 'xterm -e '
        else:
            raise ValueError, "Platform unknown %s" (self.__platform)
        
        return terminal
        
    def defaultScriptState(self):
        
        self.dpath = None
        self.ppath = None
        self.year = None
        self.doy = None
        self.enaproc = None
        self.enagen = None
        
        self.hour = [0, 24]
        self.minute = [0, 0]
        self.dbrange = None
        self.heirange = None
        
        self.unattended = False
        self.online = False
        self.plotting = True
        
        self.set = None
        self.label = ''
        self.navg = 1
        self.nbavg = 1
        self.factoravg = 1
        self.snrth = None
        self.nthreads = None
        self.calibrationfile = None
        self.channels = None
        self.testcal = None
        self.chanindex = None
        self.autocal = None
        
        self.show = True
        self.save = True
        self.plotrti = True
        self.plotspec= True
        self.quality = 'medium'
        self.fullrtdi = False
        self.filter = 0
        self.beamfilter = 0
        self.nbeams = None
        self.heibeam = None
        self.ftpserver = None
        self.ftpuser = None
        self.ftppass = None
        self.ftppath = None
        self.ftpfile = None
        self.ftpprefix = None
        self.yoffset = None
        
        
        self.sendFile = False

        self.onlineHour = None
        self.onlineMinute = None
        
        self.dcosxrange = None
        self.dcosyrange = None
        
    def setScriptState(self):

        config = MyConfigParser()
        config.read(self.configfile)
        
        # Set the third, optional argument
        self.dpath = config.get('Required', 'dpath')
        self.ppath = config.get('Required', 'ppath')
        self.year = config.getintList('Required', 'year')
        self.doy = config.getintList('Required', 'doy')
              
        # The optional arguments
        validNameList = []
        
        for name, format in self.OPTARGDICT.items():
            
            section = format[0]
            type = format[1]
            
            try:
                value = config.get(section, name)
            except:
                if section == 'Required':
                    raise ValueError, "The argument '%s' should be defined in the %s file" %(name, self.configfile)
                continue
            
            if value == '':
                if section == 'Required':
                    raise ValueError, "The value argument '%s' should defined" %(name)
                continue
            
            validNameList.append(name)
        
        for name, format in self.OPTARGDICT.items():
            
            if name not in validNameList:
                continue
            
            section = format[0]
            type = format[1]
            
            value = None
            
            if type == '' or type =='str':
                value = config.get(section, name)
            
            if type == 'boolean':
                value = config.getboolean(section, name)
                
            if type == 'int':
                value = config.getint(section, name)
            
            if type == 'float':
                value = config.getfloat(section, name)
            
            if type == 'intList':
                value = config.getintList(section, name)
            
            if type == 'floatList':
                value = config.getfloatList(section, name)
            
            if value == None:
                raise ValueError, "The data type '%s' of '%s' is not defined" %(type, name)
            
            setattr(self, name, value)
        
    def verifyScriptState(self):
        
        if not(os.path.exists(self.ppath)):
            os.mkdir(self.ppath)
        
        if self.enaproc:
            self.binpath = os.path.join(self.ppath,"BIN_FILES")
        else:
            self.binpath = self.dpath
        
        self.newterminal = self.enaproc and self.enagen
        
        if self.ftpserver != None and self.ftpuser != None and self.ftppass != None and (self.ftpfile or self.ftpprefix):
            self.sendFile = True
            
        if self.onlineHour == None:
            self.onlineHour = self.hour[0]
        
        if self.onlineMinute == None:
            self.onlineMinute = self.minute[0]
                
        self.iniTime = datetime.time(self.onlineHour, self.onlineMinute, 0)
    
    def runImageProcessing(self, year, doy):
        
        cmdProc = "%s --dpath=%s --ppath=%s --start-year=%d --end-year=%d \
--start-doy=%d --end-doy=%d --start-hour=%d --end-hour=%d --start-minute=%d --end-minute=%d \
--label='%s' --navg=%d --nbavg=%d --factor-avg=%f"  %(self.CMDIMAGPROC,
                                                      self.dpath,
                                                      self.binpath,
                                                      year[0],
                                                      year[-1],
                                                      doy[0],
                                                      doy[-1],
                                                      self.hour[0],
                                                      self.hour[-1],
                                                      self.minute[0],
                                                      self.minute[1],
                                                      self.label,
                                                      self.navg,
                                                      self.nbavg,
                                                      self.factoravg)
            
        if self.calibrationfile != None:
            cmdProc += " --config-file=%s" %(self.calibrationfile)
        
        if self.set != None:
            cmdProc += " --set=%d" %(self.set)
            
        if self.snrth != None:
            cmdProc += " --snrth=%f" %(self.snrth)
            
        if self.nthreads != None:
            cmdProc += " --nthreads=%d" %(self.nthreads)

        if self.channels:
            cmdProc += " --channels=%s" %(self.channels)
            
        if self.online:
            cmdProc += " --online"

        if self.heirange != None:
            cmdProc += " --min-hei=%d --max-hei=%d" %(self.heirange[0],
                                                   self.heirange[1])
            
        if self.newterminal:
            terminal = self.__getTerminal()
                
            cmdProc = terminal + '"' + cmdProc + '"'
        
        print "\n" + "*"*80 + "\n" 
        print cmdProc
        print "\n" + "*"*80 + "\n"
        
        p0 = subprocess.Popen(cmdProc, shell=True)
        
        return p0
    
    def runImageGeneration(self, year, doy):
    
        cmdGet = "%s --dpath=%s --ppath=%s --start-year=%d --end-year=%d \
--start-doy=%d --end-doy=%d --start-hour=%d --end-hour=%d --start-minute=%d --end-minute=%d \
--show=%d --save=%d \
--plot-rtdi=%d --plot-spec=%d" %(self.CMDIMAGGEN,
                                             self.binpath,
                                             self.ppath,
                                             year[0],
                                             year[-1],
                                             doy[0],
                                             doy[-1],
                                             self.hour[0],
                                             self.hour[-1],
                                             self.minute[0],
                                             self.minute[-1],
                                             self.show,
                                             self.save,
                                             self.plotrti,
                                             self.plotspec)
        
        if self.filter:
            cmdGet += " --filter"
        
        if self.beamfilter:
            cmdGet += " --beam-filter"
        
        if self.fullrtdi:
            cmdGet += " --full-rtdi"
            
        if self.dbrange != None:
            cmdGet += " --mindB=%d --maxdB=%d " %(self.dbrange[0],
                                                   self.dbrange[1])
        if self.heirange != None:
            cmdGet += " --min-hei=%4.2f --max-hei=%4.2f" %(self.heirange[0],
                                                   self.heirange[1])    
        
        if self.dcosxrange != None:
            cmdGet += " --min-dcosx=%4.3f --max-dcosx=%4.3f" %(self.dcosxrange[0],
                                                         self.dcosxrange[1])
             
        if self.dcosyrange != None:
            cmdGet += " --min-dcosy=%4.3f --max-dcosy=%4.3f" %(self.dcosyrange[0],
                                                         self.dcosyrange[1])
            
        if self.online:
            cmdGet += " --online"
            
        if self.sendFile:
            cmdGet += " --ftp-server=%s --ftp-user=%s --ftp-pass=%s --ftp-path=%s" %(self.ftpserver,
                                                                                 self.ftpuser,
                                                                                 self.ftppass,
                                                                                 self.ftppath)
            if self.ftpfile != None: cmdGet += " --ftp-file=%s" %(self.ftpfile)
            else: cmdGet += " --ftp-prefix=%s" %(self.ftpprefix)
            
        if self.newterminal and self.online:
            terminal = self.__getTerminal()
              
            cmdGet = terminal + '"' + cmdGet + '"'
            
        print "\n" + "*"*80 + "\n" 
        print cmdGet
        print "\n" + "*"*80 + "\n"
        
        p1 = subprocess.Popen(cmdGet, shell=True)
        
        return p1
    
    def getDoy(self):
        
        
        today = datetime.date.today()
        tt = datetime.timetuple(today)
        
        year = tt[0]
        doy = tt[7]
        
        return year, doy
 
    def runPerDay(self):
        
        year = self.year
        doy = self.doy
        
        startDate = datetime.datetime(self.year[0],1,1,0,0,0) + datetime.timedelta(self.doy[0]-1)
        endDate = datetime.datetime(self.year[-1],1,1,0,0,0) + datetime.timedelta(self.doy[-1]-1)
        thisDate = startDate
        
        while thisDate <= endDate:
            
            tt = thisDate.timetuple()
            year = [tt[0]]; doy = [tt[7]]
            
            #print "\n" + "*"*80 + "\n"
            
            if self.enaproc:
                p0 = self.runImageProcessing(year, doy)
                
                time.sleep(5)
        
            if self.enagen:
                p1 = self.runImageGeneration(year, doy)
        
            if self.enaproc:
                p0.wait()
        
            if self.enagen:
                p1.wait()
                
            thisDate += datetime.timedelta(1)
                
    def run(self):
        
        year = self.year
        doy = self.doy
        
        #print "\n" + "*"*80 + "\n" 
        
        if self.enaproc:
            p0 = self.runImageProcessing(year, doy)
            
            time.sleep(10)
    
        if self.enagen:
            p1 = self.runImageGeneration(year, doy)
    
        if self.enaproc:
            p0.wait()
            
            print 'process PROC finished'
    
        if self.enagen:
            p1.wait()
            
            print 'process GEN finished'
            
    
    def runonline(self):
        
        cls()
        gotoxy(1, 1)
        print "******************** Initializing the program in online mode *******************"

        strCount = '\|/-'
        index = 0
        previousDate = None
        
        while True:
            waitNewDate = False
            index += 1
            if index>3: index=0
            
            currentDatetime = datetime.datetime.today()
            currentDate = currentDatetime.date()
            currentTime = currentDatetime.time()

            if (currentDate == previousDate):
                waitNewDate = True
             
            if (currentTime <= self.iniTime):
                waitNewDate = True
            
            if waitNewDate:
                gotoxy(1,4)
                print "%s: Waiting new date until %s ... [%s]" %(currentDate, self.iniTime, strCount[index])
                time.sleep(self.__WAIT)
                continue
            
            
            gotoxy(1,4)
            print "************************************************************"
            print "Starting data processing: %s / %s" %(currentDate, currentTime)
            print "************************************************************"
            
            tt = datetime.date.timetuple(currentDate)
            self.year = [tt[0]]
            self.doy = [tt[7]]
            self.run()
            previousDate = currentDate
            cls()
            continue

            


            
                
    
if __name__ == '__main__':
    # parse command line
    arglist = ''
    longarglist = ['iniFile=', 'help']
    
    optlist, args = getopt.getopt(sys.argv[1:], arglist, longarglist)
    
    # set default values
    inifile = 'imaging.ini'
    
    for opt in optlist:
        if opt[0] == '--iniFile':
            inifile = opt[1]
        elif opt[0] == '--help':
            print usage
            sys.exit(0)
        else:
            raise ValueError, 'Illegal option %s\n%s' % (opt[0], usage)
        
    Imaging(configfile = inifile)