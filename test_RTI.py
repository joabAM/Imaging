from schainpy.controller import Project

def main():


    desc = "test"
    filename = "file.xml"
    xmin = '15'
    xmax = '23' #-> el plot genera +1 en la hora, es decir aparece 18 como máximo
    ymin = '100'
    ymax = '840'
    dbmin = '0' 
    dbmax = '10' #'70' #'55'
    showSPC = '0' #view plot Spectra
    showRTI = '1' #view plot RTI
    showNOISE = '0' #view plot NOISE
    localtime='0' #para ajustar el horario en las gráficas '0' para dejar en utc
 

    today = time.strftime("%Y/%m/%d")
    #startDate=today
    #endDate=today
    startDate='2009/11/4'
    endDate='2009/11/4'
    #inPath= '/home/soporte/dataAMISR_test/'
    inPath= '/home/soporte/Documentos/Imaging/PDATA'
    #inPath = '/mnt/data_amisr'
    outPath = '/home/soporte/Data/EEJ'


    ##.......................................................................................
    ##.......................................................................................
    controllerObj = Project()
    controllerObj.setup(id = '11', name='proc', description=desc)
    ##.......................................................................................
    ##.......................................................................................
    readUnitConfObj = controllerObj.addReadUnit(datatype='SpectraReader',
                                                path=inPath,
                                                startDate=startDate,#startDate, #'2014/10/07',
                                                endDate=endDate, #endDate '2014/10/07',
                                                startTime='19:01:30',#'07:00:00',
                                                endTime='22:00:00',#'15:00:00',
                                                walk=1,
						time='lt',
                                                online=0)

   


    ##.......................................................................................
    ##.......................................................................................

    procUnitConfObj1 = controllerObj.addProcUnit(datatype='SpectraProc', inputId=readUnitConfObj.getId())
    #procUnitConfObj1.addParameter(name='nFFTPoints', value='16', format='int')


    opObj11 = procUnitConfObj1.addOperation(name='IncohInt', optype='other')
    opObj11.addParameter(name='n', value='10', format='int') #300?

    op1 = procUnitConfObj1.addOperation(name='selectChannels', optype='self')
    op1.addParameter(name='channelList', value='0,2', format='intlist')
    ## Remove DC signal
    #opObj11 = procUnitConfObj1.addOperation(name='removeDC')
    ##.......................................................................................

    ##.......................................................................................

    opObj15 = procUnitConfObj1.addOperation(name='RTIPlot', optype='external')
    opObj15.addParameter(name='id', value='2', format='int')
    opObj15.addParameter(name='localtime', value=1,format='int')
    opObj15.addParameter(name='wintitle', value='RTI', format='str')
    opObj15.addParameter(name='xmin', value=xmin, format='int')
    opObj15.addParameter(name='xmax', value=xmax, format='int') #max value =23
    opObj15.addParameter(name='ymin', value=ymin, format='int')
    opObj15.addParameter(name='zmin', value=dbmin, format='int')
    opObj15.addParameter(name='zmax', value=dbmax, format='int')
    opObj15.addParameter(name='showprofile', value='0', format='int')
    opObj15.addParameter(name='save', value=outPath+'/plots', format='str')
    #opObj15.addParameter(name='colormap', value='jet', format='str')
    opObj15.addParameter(name='show', value = showRTI, format='int')



    ##.......................................................................................
    ##.......................................................................................


    controllerObj.start()

 
if __name__ == '__main__':
    import time
    start_time = time.time()
    main()
    print("--- %s seconds ---" % (time.time() - start_time))

