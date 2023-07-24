from schainpy.controller import Project

prj = Project()

read_unit = prj.addReadUnit(
    datatype='Spectra',
    path='/home/japaza/Data/pdata/d2010002',
    startDate='2010/01/02',
    endDate='2010/01/03',
    startTime='18:00:00',
    endTime='07:59:59',
    online=0,
    walk=0
    )

proc_unit = prj.addProcUnit(
    datatype='Spectra',
    inputId=read_unit.getId()
    )

#op = proc_unit.addOperation(name='selectChannels')
#op.addParameter(name='channelList', value='0,1')

op = proc_unit.addOperation(name='setAttribute')
op.addParameter(name='flagDecodeData', value=False)
    
op = proc_unit.addOperation(name='IncohInt')
op.addParameter(name='n', value='10')



op = proc_unit.addOperation(name='selectHeights')
op.addParameter(name='minHei', value='80')
op.addParameter(name='maxHei', value='600')

#op = proc_unit.addOperation(name='removeDC')

#op = proc_unit.addOperation(name='SpectraPlot')
#op.addParameter(name='wintitle', value='Spectra', format='str')

op = proc_unit.addOperation(name='RTIPlot')
op.addParameter(name='wintitle', value='RTI', format='str')
op.addParameter(name='zmin', value=20, format='int')
op.addParameter(name='zmax', value=35, format='int')

prj.start()
