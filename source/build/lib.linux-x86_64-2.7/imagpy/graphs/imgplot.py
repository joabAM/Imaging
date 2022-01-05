'''
Created on Jun 9, 2010

@author: roj-idl71
'''
import os
import datetime
import numpy
from shutil import copyfile
import driver_plplot as dplot


ID_XCUT = 1
ID_YCUT = 2
ID_HCUT = 3
ID_AVG  = 4
ID_SPEC_RTI = 5

NX_SUBPLOT = 3
NY_SUBPLOT = 3
NTOTAL_SUBPLOTS = NX_SUBPLOT*NY_SUBPLOT

WSCREEN_SPEC = 400
HSCREEN_SPEC = 620

translateMonth = {"January":"Enero", "February":"Febrero", "March":"Marzo",
                "April":"Abril","May":"Mayo", "June":"Junio", "July":"Julio",
                "August":"Agosto", "September":"Setiembre", "October":"Octubre",
                "November":"Noviembre", "December":"Diciembre"}

def linearData2RGB(linearData, mindB, maxdB):
    """
    Inputs:
        linear_data  :    numpy array (float). Linear data values should not be in dB.
    """
    data_dB = 10.0*numpy.log10(linearData)

    rgbImage = (numpy.clip(data_dB, mindB, maxdB) - mindB)/(maxdB-mindB)*255
    rgbImage = rgbImage.astype(int)

    return rgbImage

def _getFilename(fpath, fprefix, ddatetime, ext, fsufix="_%n"):

    fname = fprefix + ddatetime.strftime("%Y%m%d_%H%M%S") + fsufix + ext
    filename = os.path.join(fpath, fname)

    return filename

def _plotBasicXYZCut(x1D, y1D, z1D, rgb_data, xlabel, ylabel, title, text="", axis=0,
                     nx_subplot=None, ny_subplot=None):
    """
    Inputs:

        x1D            :    1D numpy array. X range with nx elements
        y1D            :    1D numpy array. Y range with ny elements
        z1D            :    1D numpy array. Altitude range with nz elements
        rgb_data     :    4D numpy array (int). rgb_data values should be in [0 - 255]
                          Array dimension is (nx, ny, nz, 3).
                          Dim 3 is used as RGB value

        xlabel
        ylabel
        show         :    True or False
        save         :    True or False
        fpath        :    figure path
        fprefix :    file name prefix
        ddatetime    :    date and time of the data in datetime.datetime format
        ext          :    filename extension


    """
    if axis == 0:
        cut_vector = x1D
        x_ = y1D
        y_ = z1D
    if axis == 1:
        cut_vector = y1D
        x_ = x1D
        y_ = z1D
    if axis == 2:
        cut_vector = z1D
        x_ = x1D
        y_ = y1D

    if nx_subplot == None:
        nx_subplot = NX_SUBPLOT

    if ny_subplot == None:
        ny_subplot = NY_SUBPLOT

    NTOTAL_SUBPLOTS = nx_subplot * ny_subplot

    if len(x_) < 2 or len(y_) < 2:
        raise ValueError, "X, Y, Z (%d, %d, %d) length should be greater than 1" %(len(x1D), len(y1D), len(z1D))
        return

    nplot = 0
    iplot = 0

    for index in range(len(cut_vector)):

        if iplot == 0:
            dplot.openpage()

        dplot.subplot(nx_subplot, ny_subplot, iplot+1)

        dplot.pcolor_basic(x_, y_, rgb_data.take(index, axis=axis))
        title_ = "%s = %5.4f" %(title, cut_vector[index])
        dplot.label(xlabel, ylabel, title_)

        nplot += 1
        iplot = nplot % NTOTAL_SUBPLOTS

        if iplot == 0:
            dplot.ftitle(text)
            dplot.closepage()

    if iplot != 0:
        dplot.ftitle(text)
        dplot.closepage()

def _plotXYCut(x3D, y3D, z3D, rgb_data, xlabel, ylabel, title, text="", axis=0,
               nx_subplot=None, ny_subplot=None):

    ncuts = None
    nx, ny, nz = z3D.shape

    if axis == 0:
        cut_vector = x3D[:,int(ny/2),:]
        x_ = y3D
        y_ = z3D
    if axis == 1:
        cut_vector = y3D[int(nx/2),:,:]
        x_ = x3D
        y_ = z3D
#     if axis == 2:
#         cut_vector = z3D[0,:,:]
#         ncuts = len(z3D[0,0,:])
#         x_ = x3D
#         y_ = y3D

    if nx_subplot == None:
        nx_subplot = NX_SUBPLOT

    if ny_subplot == None:
        ny_subplot = NY_SUBPLOT

    NTOTAL_SUBPLOTS = nx_subplot * ny_subplot

    nx, ny = x_.take(0, axis=axis).shape

    if nx < 2 or ny < 2:
        raise ValueError, "Every dimension of X (%s), Y(%s) and Z (%s) should be greater than 1" %(str(x3D.shape), str(y3D.shape), str(z3D.shape))
        return

    ncuts = len(cut_vector[:,0])

    if ncuts == None:
        raise ValueError, "axis should be 0 or 1"

    nplot = 0
    iplot = 0

    for icut in range(ncuts):

        if iplot == 0:
            dplot.openpage()

        dplot.subplot(nx_subplot, ny_subplot, iplot+1)

        dplot.pcolor(x_.take(icut, axis=axis),
                     y_.take(icut, axis=axis),
                     rgb_data.take(icut, axis=axis),
                     xmin = numpy.min(x_),
                     xmax = numpy.max(x_),
                     ymin = numpy.min(y_),
                     ymax = numpy.max(y_),
                     contour=False)

        title_ = "%s = [%4.2f, %4.2f] Km" %(title, cut_vector[icut,0], cut_vector[icut,-1])
        dplot.label(xlabel, ylabel, title_)

        nplot += 1
        iplot = nplot % NTOTAL_SUBPLOTS

        if iplot == 0:
            dplot.ftitle(text)
            dplot.closepage()

    if iplot != 0:
        dplot.ftitle(text)
        dplot.closepage()


def _plotZCut(x3D, y3D, z1D, rgb_data4D, xlabel, ylabel, title, text="",
              nx_subplot=None, ny_subplot=None):

    if nx_subplot == None:
        nx_subplot = NX_SUBPLOT

    if ny_subplot == None:
        ny_subplot = NY_SUBPLOT

    NTOTAL_SUBPLOTS = nx_subplot * ny_subplot

    cut_vector = z1D
    x_ = x3D
    y_ = y3D

    nplot = 0
    iplot = 0

    for iz in range(len(cut_vector)):

        if iplot == 0:
            dplot.openpage()

        dplot.subplot(nx_subplot, ny_subplot, iplot+1)

        dplot.pcolor(x_[:,:,iz],
                     y_[:,:,iz],
                     rgb_data4D[:,:,iz,:],
                     xmin = numpy.min(x_),
                     xmax = numpy.max(x_),
                     ymin = numpy.min(y_),
                     ymax = numpy.max(y_),
                     contour=True)

        title_ = "%s = %4.2f Km" %(title, cut_vector[iz])
        dplot.label(xlabel, ylabel, title_)

        nplot += 1
        iplot = nplot % NTOTAL_SUBPLOTS

        if iplot == 0:
            dplot.ftitle(text)
            dplot.closepage()

    if iplot != 0:
        dplot.ftitle(text)
        dplot.closepage()

def plotDcosxCut(x1D, y1D, z1D, rgb_data, ddatetime, xlabel="Dcosy", ylabel="Range (Km)", title="Dcosx", show=True,
                 save=False, fpath="./", fprefix="dcosxcut", ext=".png",
                 nx_subplot=None, ny_subplot=None, display_size=None):
    """
    Inputs:

        x1D            :    1D numpy array. X range with nx elements
        y1D            :    1D numpy array. Y range with ny elements
        z1D            :    1D numpy array. Altitude range with nz elements
        rgb_data     :    4D numpy array. Array dimension is (nx, ny, nz, 3).
                          Dim 3 is used as RGB value

        xlabel
        ylabel
        show         :    True or False
        save         :    True or False
        fpath        :    figure path
        fprefix :    file name prefix
        ddatetime    :    date and time of the data in datetime.datetime format
        ext          :    filename extension


    """
    ftitle = ddatetime.strftime("%B %d, %Y - %X")
    if show:
        dplot.figure(ID_XCUT, display_size=display_size)
        _plotBasicXYZCut(x1D, y1D, z1D, rgb_data, xlabel=xlabel, ylabel=ylabel, title=title, axis=0, text = ftitle,
                         nx_subplot=nx_subplot, ny_subplot=ny_subplot)
#         dplot.close()    #Don't close. Reusing opened graphic

    if save:
        filename = _getFilename(fpath, fprefix, ddatetime, ext)

        dplot.figure(save=save, fname=filename, multifiles=True, display_size=display_size)
        _plotBasicXYZCut(x1D, y1D, z1D, rgb_data, xlabel=xlabel, ylabel=ylabel, title=title, axis=0, text = ftitle,
                         nx_subplot=nx_subplot, ny_subplot=ny_subplot)
        dplot.close()

#     dplot.close("all")

def plotDcosyCut(x1D, y1D, z1D, rgb_data, ddatetime, xlabel="Dcosx", ylabel="Range (Km)", title="Dcosy", show=True,
                 save=False, fpath="./", fprefix="dcosycut", ext=".png",
                 nx_subplot=None, ny_subplot=None, display_size=None):
    """
    Inputs:

        x1D            :    1D numpy array. X range with nx elements
        y1D            :    1D numpy array. Y range with ny elements
        z1D            :    1D numpy array. Altitude range with nz elements
        rgb_data     :    4D numpy array. Array dimension is (nx, ny, nz, 3).
                          Dim 3 is used as RGB value

        xlabel
        ylabel
        show         :    True or False
        save         :    True or False
        fpath        :    figure path
        fprefix :    file name prefix
        ddatetime    :    date and time of the data in datetime.datetime format
        ext          :    filename extension


    """
    ftitle = ddatetime.strftime("%B %d, %Y - %X")
    if show:
        dplot.figure(ID_YCUT, display_size=display_size)
        _plotBasicXYZCut(x1D, y1D, z1D, rgb_data, xlabel=xlabel, ylabel=ylabel, title=title, axis=1, text = ftitle,
                         nx_subplot=nx_subplot, ny_subplot=ny_subplot)
#         dplot.close()    #Don't close. Reusing opened graphic

    if save:
        filename = _getFilename(fpath, fprefix, ddatetime, ext)

        dplot.figure(save=save, fname=filename, multifiles=True, display_size=display_size)
        _plotBasicXYZCut(x1D, y1D, z1D, rgb_data, xlabel=xlabel, ylabel=ylabel, title=title, axis=1, text = ftitle,
                         nx_subplot=nx_subplot, ny_subplot=ny_subplot)
        dplot.close()

#     dplot.close("all")

def plotRangeCut(x1D, y1D, z1D, rgb_data, xlabel="Dcosx", ylabel="Dcosy", title="Range", show=True,
                 save=False, fpath="./", fprefix="rcut", ddatetime=None, ext=".png",
                 nx_subplot=None, ny_subplot=None, display_size=None):
    """
    Inputs:

        x1D            :    1D numpy array. X range with nx elements
        y1D            :    1D numpy array. Y range with ny elements
        z1D            :    1D numpy array. Altitude range with nz elements
        rgb_data     :    4D numpy array (int). rgb_data values should be in [0 - 255]
                          Array dimension is (nx, ny, nz, 3).
                          Dim 3 is used as RGB value

        xlabel
        ylabel
        show         :    True or False
        save         :    True or False
        fpath        :    figure path
        fprefix :    file name prefix
        ddatetime    :    date and time of the data in datetime.datetime format
        ext          :    filename extension


    """
    ftitle = ddatetime.strftime("%B %d, %Y - %X")

    if show:
        dplot.figure(ID_HCUT, display_size=display_size)
        _plotBasicXYZCut(x1D, y1D, z1D, rgb_data, xlabel=xlabel, ylabel=ylabel, title=title, axis=2, text = ftitle,
                         nx_subplot=nx_subplot, ny_subplot=ny_subplot)
#         dplot.close()    #Don't close. Reusing opened graphic

    if save:
        filename = _getFilename(fpath, fprefix, ddatetime, ext)

        dplot.figure(save=save, fname=filename, multifiles=True, display_size=display_size)
        _plotBasicXYZCut(x1D, y1D, z1D, rgb_data, xlabel=xlabel, ylabel=ylabel, title=title, axis=2, text = ftitle,
                         nx_subplot=nx_subplot, ny_subplot=ny_subplot)
        dplot.close()

#     dplot.close("all")

def _plotAvg(x1D, y1D, z1D, rgb_xdata, rgb_ydata, rgb_zdata,
             rgb_xdata_filtered, rgb_ydata_filtered, rgb_zdata_filtered,
             xlabel, ylabel, zlabel, ftitle,
             ix_min, ix_max, iy_min, iy_max, iz_min, iz_max):

    dplot.subplot(3,2,1)
    dplot.pcolor_basic(y1D, z1D, rgb_xdata)
    dplot.label(ylabel, zlabel, title="Average in Dcosx around (%4.3f, %4.3f)" %(x1D[0], x1D[-1]) )

    dplot.subplot(3,2,2)
    dplot.pcolor_basic(x1D, z1D, rgb_ydata)
    dplot.label(xlabel, zlabel, title="Average in Dcosy around (%4.3f, %4.3f)" %(y1D[0], y1D[-1]) )

    dplot.subplot(3,2,3)
    dplot.pcolor_basic(x1D, y1D, rgb_zdata)
    dplot.label(xlabel, ylabel, title="Average in Range around (%4.2f, %4.2f)" %(z1D[0], z1D[-1]) )

    dplot.subplot(3,2,4)
    dplot.pcolor_basic(y1D, z1D, rgb_xdata_filtered)
    dplot.label(ylabel, zlabel, title="Average in Dcosx around (%4.3f, %4.3f)" %(x1D[ix_min], x1D[ix_max]) )

    dplot.subplot(3,2,5)
    dplot.pcolor_basic(x1D, z1D, rgb_ydata_filtered)
    dplot.label(xlabel, zlabel, title="Average in Dcosy around (%4.3f, %4.3f)" %(y1D[iy_min], y1D[iy_max]) )

    dplot.subplot(3,2,6)
    dplot.pcolor_basic(x1D, y1D, rgb_zdata_filtered)
    dplot.label(xlabel, ylabel, title="Average in Range around (%4.2f, %4.2f)" %(z1D[iz_min], z1D[iz_max]) )

    dplot.ftitle(ftitle)

def plotAverage(x1D, y1D, z1D, linear_data, mindB, maxdB, ddatetime, show=True,
                xmin=None, xmax=None, ymin=None, ymax=None, zmin=None, zmax=None,
                save=False, fpath="./", ext=".png", fprefix="avg",
                display_size="900x600"):
    """
    Inputs:

        x1D            :    1D numpy array. X range with nx elements
        y1D            :    1D numpy array. Y range with ny elements
        z1D            :    1D numpy array. Altitude range with nz elements
        linear_data  :    4D numpy array (float). Linear data values should not be in dB.
                          Array dimension is (nx, ny, nz, 3).

        xlabel
        ylabel
        show         :    True or False
        save         :    True or False
        fpath        :    figure path
        fprefix      :    file name prefix
        ddatetime    :    date and time of the data in datetime.datetime format
        ext          :    filename extension


    """
    import imagpy.utils.misc as misc

    xlabel = "Dcosx"
    ylabel = "Dcosy"
    zlabel = "Range"
    ftitle = ddatetime.strftime("%B %d, %Y - %X")

    if xmin == None: xmin = min(x1D)
    if xmax == None: xmax = max(x1D)
    if ymin == None: ymin = min(y1D)
    if ymax == None: ymax = max(y1D)
    if zmin == None: zmin = min(z1D)
    if zmax == None: zmax = max(z1D)

    ix_min, ix_max = misc.findMinMaxIndex(x1D, xmin, xmax)
    iy_min, iy_max = misc.findMinMaxIndex(y1D, ymin, ymax)
    iz_min, iz_max = misc.findMinMaxIndex(z1D, zmin, zmax)

    avg_data = numpy.mean(linear_data[:,:,:,:], axis=0)
    rgb_xdata = linearData2RGB(avg_data, mindB=mindB, maxdB=maxdB)

    avg_data = numpy.mean(linear_data[:,:,:,:], axis=1)
    rgb_ydata = linearData2RGB(avg_data, mindB=mindB, maxdB=maxdB)

    avg_data = numpy.mean(linear_data[:,:,:,:], axis=2)
    rgb_zdata = linearData2RGB(avg_data, mindB=mindB, maxdB=maxdB)

    avg_data = numpy.mean(linear_data[ix_min:ix_max+1,:,:,:], axis=0)
    rgb_xdata_filtered = linearData2RGB(avg_data, mindB=mindB, maxdB=maxdB)

    avg_data = numpy.mean(linear_data[:,iy_min:iy_max+1,:,:], axis=1)
    rgb_ydata_filtered = linearData2RGB(avg_data, mindB=mindB, maxdB=maxdB)

    avg_data = numpy.mean(linear_data[:,:,iz_min:iz_max+1,:], axis=2)
    rgb_zdata_filtered = linearData2RGB(avg_data, mindB=mindB, maxdB=maxdB)

    if show:
        dplot.figure(ID_AVG, display_size=display_size)

        _plotAvg(x1D, y1D, z1D, rgb_xdata, rgb_ydata, rgb_zdata,
             rgb_xdata_filtered, rgb_ydata_filtered, rgb_zdata_filtered,
             xlabel, ylabel, zlabel, ftitle,
             ix_min, ix_max, iy_min, iy_max, iz_min, iz_max)
#         dplot.close()    #Don't close. Reusing opened graphic

    if save:
        filename = _getFilename(fpath, fprefix, ddatetime, ext)
        dplot.figure(display_size=display_size, save=save, fname=filename)

        _plotAvg(x1D, y1D, z1D, rgb_xdata, rgb_ydata, rgb_zdata,
             rgb_xdata_filtered, rgb_ydata_filtered, rgb_zdata_filtered,
             xlabel, ylabel, zlabel, ftitle,
             ix_min, ix_max, iy_min, iy_max, iz_min, iz_max)

        dplot.close()

def close_all():
    dplot.close("all")

def plotXCut(x3D, y3D, z3D, rgb_data, ddatetime, xlabel="Y (Km)", ylabel="Altitude (Km)", title="X", show=True,
             save=False, fpath="./", fprefix="xcut", ext=".png",
             nx_subplot=None, ny_subplot=None, display_size=None):
    """
    Inputs:

        x3D            :    3D numpy array. X range with nx elements
        y3D            :    3D numpy array. Y range with ny elements
        z3D            :    3D numpy array. Altitude range with nz elements
        rgb_data     :    4D numpy array (int). rgb_data values should be in [0 - 255]
                          Array dimension is (nx, ny, nz, 3).
                          Dim 3 is used as RGB value

        xlabel
        ylabel
        show         :    True or False
        save         :    True or False
        fpath        :    figure path
        fprefix :    file name prefix
        ddatetime    :    date and time of the data in datetime.datetime format
        ext          :    filename extension


    """
    ftitle = ddatetime.strftime("%B %d, %Y - %X")


    if show:
        dplot.figure(ID_XCUT, display_size=display_size)
        _plotXYCut(x3D, y3D, z3D, rgb_data, xlabel=xlabel, ylabel=ylabel, title=title, axis=0, text = ftitle,
                         nx_subplot=nx_subplot, ny_subplot=ny_subplot)
#         dplot.close()    #Don't close. Reusing opened graphic

    if save:
        filename = _getFilename(fpath, fprefix, ddatetime, ext)

        dplot.figure(save=save, fname=filename, multifiles=True, display_size=display_size)
        _plotXYCut(x3D, y3D, z3D, rgb_data, xlabel=xlabel, ylabel=ylabel, title=title, axis=0, text = ftitle,
                         nx_subplot=nx_subplot, ny_subplot=ny_subplot)
        dplot.close()

#     dplot.close("all")

def plotYCut(x3D, y3D, z3D, rgb_data, ddatetime, xlabel="X (Km)", ylabel="Altitude (Km)", title="Y", show=True,
             save=False, fpath="./", fprefix="ycut", ext=".png",
             nx_subplot=None, ny_subplot=None, display_size=None):
    """
    Inputs:

        x3D            :    3D numpy array. X range with nx elements
        y3D            :    3D numpy array. Y range with ny elements
        z3D            :    3D numpy array. Altitude range with nz elements
        rgb_data     :    4D numpy array (int). rgb_data values should be in [0 - 255]
                          Array dimension is (nx, ny, nz, 3).
                          Dim 3 is used as RGB value

        xlabel
        ylabel
        show         :    True or False
        save         :    True or False
        fpath        :    figure path
        fprefix :    file name prefix
        ddatetime    :    date and time of the data in datetime.datetime format
        ext          :    filename extension


    """
    ftitle = ddatetime.strftime("%B %d, %Y - %X")

    if show:
        dplot.figure(ID_YCUT, display_size=display_size)
        _plotXYCut(x3D, y3D, z3D, rgb_data, xlabel=xlabel, ylabel=ylabel, title=title, axis=1, text = ftitle,
                         nx_subplot=nx_subplot, ny_subplot=ny_subplot)
#         dplot.close()    #Don't close. Reusing opened graphic

    if save:
        filename = _getFilename(fpath, fprefix, ddatetime, ext)

        dplot.figure(save=save, fname=filename, multifiles=True, display_size=display_size)
        _plotXYCut(x3D, y3D, z3D, rgb_data, xlabel=xlabel, ylabel=ylabel, title=title, axis=1, text = ftitle,
                         nx_subplot=nx_subplot, ny_subplot=ny_subplot)
        dplot.close()

#     dplot.close("all")

def plotZCut(x3D, y3D, z1D, rgb_data, ddatetime, xlabel="X (Km)", ylabel="Y (Km)", title="H", show=True,
             save=False, fpath="./", fprefix="zcut", ext=".png",
             nx_subplot=None, ny_subplot=None, display_size=None):
    """
    Inputs:

        x3D            :    3D numpy array. X range with nx elements
        y3D            :    3D numpy array. Y range with ny elements
        z1D            :    1D numpy array. Altitude range with nz elements
        rgb_data     :    4D numpy array (int). rgb_data values should be in [0 - 255]
                          Array dimension is (nx, ny, nz, 3).
                          Dim 3 is used as RGB value

        xlabel
        ylabel
        show         :    True or False
        save         :    True or False
        fpath        :    figure path
        fprefix :    file name prefix
        ddatetime    :    date and time of the data in datetime.datetime format
        ext          :    filename extension


    """
    ftitle = ddatetime.strftime("%B %d, %Y - %X")

    if show:
        dplot.figure(ID_HCUT, display_size=display_size)
        _plotZCut(x3D, y3D, z1D, rgb_data, xlabel=xlabel, ylabel=ylabel, title=title, text = ftitle,
                         nx_subplot=nx_subplot, ny_subplot=ny_subplot)
#         dplot.close()    #Don't close. Reusing opened graphic

    if save:
        filename = _getFilename(fpath, fprefix, ddatetime, ext)

        dplot.figure(save=save, fname=filename, multifiles=True, display_size=display_size)
        _plotZCut(x3D, y3D, z1D, rgb_data, xlabel=xlabel, ylabel=ylabel, title=title, text = ftitle,
                         nx_subplot=nx_subplot, ny_subplot=ny_subplot)
        dplot.close()

#     dplot.close("all")

def _RGBfunction(np=20):

    color_shape = numpy.zeros(np)
    color_shape[        :np/2] = numpy.arange(0,1,2.0/np)
    color_shape[np/2:        ] = numpy.arange(1,0,-2.0/np)

    r = numpy.zeros(np*2+1)
    g = numpy.zeros(np*2+1)
    b = numpy.zeros(np*2+1)

    r[0     :np    ] = color_shape
    r[3*np/2:      ] = color_shape[:(np/2+1)]

    g[np/2  :3*np/2] = color_shape

    b[np    :2*np  ] = color_shape
    b[      :np/2  ] = color_shape[np/2:]

    colors = numpy.empty((3*np/2+1, 3))
    colors[:,0] = r[np/4:7*np/4+1]
    colors[:,1] = g[np/4:7*np/4+1]
    colors[:,2] = b[np/4:7*np/4+1]

    return colors

def _plotSpec(nplots, x2D, y2D, rgb_data3D,
              xmin=None, xmax=None, ymin=None, ymax=None,std_out=False,
              xlabel="Zonal Range (Km)", ylabel="Altitude (Km)", title="ESF", text="",
              szchar= 3.2, posT= 4.8, scChar=1.1):

    if xmin == None: xmin = numpy.min(x2D)
    if xmax == None: xmax = numpy.max(x2D)
    if ymin == None: ymin = numpy.min(y2D)
    if ymax == None: ymax = numpy.max(y2D)

    xpage_min = 0.20
    xpage_max = 0.90
    ypage_min = 0.10
    ypage_max = 0.95

    if nplots == 1:
        nw = 1
        if std_out:
            xpage_max = 0.98
            ypage_min = 0.15
            ypage_max = 0.93
    if nplots == 2:
        nw = 3

#     dplot.openpage()
    dplot.subplot(nw, 1, 1)
    dplot.axis(xmin, xmax, ymin, ymax, szchar=szchar/1.2,
               xpage_min=xpage_min, xpage_max=xpage_max, ypage_min=ypage_min,
               ypage_max=ypage_max)

    dplot.pcolor(x2D, y2D, rgb_data3D, contour=True, set_axis=False)
    dplot.label(xlabel, ylabel, title, szchar=szchar)
    dplot.ptitle(text, szchar=szchar*scChar, pos = posT)  # szchar=szchar*1.1,  pos = 4.8
#     dplot.closepage()



def _plotRTI(nplots, x1D, y1D, rgb_data3D,
             xmin=None, xmax=None, ymin=None, ymax=None,
             xlabel="Local Time", ylabel="Altitude (Km)", title="RTDI", text="",
             seconds=None, std_out = False,
             szchar = 3.2, posT = 4.8,scChar=1.1):

    if xmin == None: xmin = numpy.min(x1D)
    if xmax == None: xmax = numpy.max(x1D)
    if ymin == None: ymin = numpy.min(y1D)
    if ymax == None: ymax = numpy.max(y1D)

    if len(x1D) < 2:
        return

    if seconds == None:
        seconds = x1D[-1]
    ypage_min = 0.1
    ypage_max = 0.95

    if nplots == 1:
        xpage_min = 0.08
        xpage_max = 0.965   #0.965
        if std_out:
            xpage_min = 0.12
            xpage_max = 0.95
            ypage_min = 0.15
            ypage_max = 0.93
        ylabel_ticks = True

    if nplots == 2:
        xpage_min = 0.33
        xpage_max = 0.98
        ylabel_ticks = False
        ylabel = ""

#     dplot.openpage()
    dplot.subplot(1, 1, 1)
    dplot.axis(xmin, xmax, ymin, ymax, szchar=szchar/1.2,
               xpage_min=xpage_min, xpage_max=xpage_max, ypage_min=ypage_min, ypage_max=ypage_max,
               ylabel_ticks=ylabel_ticks, xtime=True)

    dplot.pcolor_basic(x1D, y1D, rgb_data3D, set_axis=False)
#     dplot.parrow([seconds, seconds], [ymin, ymax], line=1)
    dplot.pline([seconds, seconds], [ymin, ymax], icol=15, line=2)

    dplot.label(xlabel, ylabel, title, szchar=szchar)
    dplot.ptitle(text, szchar=szchar*scChar, pos = posT)
#     dplot.closepage()

def plotSpecAndRTI(xspec2D, yspec2D, rgb_data3D,
                   ddatetime,
                   xmin=None, xmax=None, ymin=None, ymax=None,
                   trti1D=None, yrti1D=None, rti3D=None,
                   tmin=None, tmax=None,
                   plot_spec=False,
                   plot_rtdi=False, std_plots=False,
                   show=True, save=False,img_path= "./",
                   year = 0, doy = 0,
                   fpath = "./", fprefix="img", ext=".png",
                   plot_colormap=False,
                   cmap_xmin=None, cmap_xmax=None,
                   cmap_ymin=None, cmap_ymax=None):

    xlabel_spc = "Zonal Range (Km)"
    xlabel_rti = "Local Time"
    ylabel = "Altitude (Km)"
    sizeChar = 3.2
    posText = 4.8
    scale = 1.1
    global HSCREEN_SPEC

    if xmin == None: xmin = numpy.min(xspec2D)
    if xmax == None: xmax = numpy.max(xspec2D)
    if ymin == None: ymin = numpy.min(yspec2D)
    if ymax == None: ymax = numpy.max(yspec2D)

    ftitle = ddatetime.strftime("%B %d, %Y - %X")
    if std_plots:
        month = ftitle.split(" ")[0]
        monthSpanish = translateMonth[month]
        ftitle = ftitle.replace(month,monthSpanish)
        xlabel_spc = "Rango Zonal (Km)"
        xlabel_rti = "Hora Local"
        ylabel = "Altitud (Km)"
        sizeChar = 5
        posText = 5.5
        scale = 0.9

    utc_epoch = (ddatetime - datetime.datetime(1970,1,1)).total_seconds()
    nplots = plot_spec + plot_rtdi

    if nplots == 0:
        return

    if nplots == 1:
        if std_plots:
            nw = 1 + plot_rtdi*0.8
            #HSCREEN_SPEC = 720
        else:
            nw = 1 + plot_rtdi*1.2

    if nplots == 2:
        nw = 3

    display_size = "%dx%d" %(WSCREEN_SPEC*nw, HSCREEN_SPEC)
    if show:
        dplot.figure(ID_SPEC_RTI, display_size=display_size)
        dplot.openpage()

        if plot_spec:
            _plotSpec(nplots, x2D=xspec2D, y2D=yspec2D, rgb_data3D=rgb_data3D,
                      xmin=xmin, xmax=xmax, ymin=ymin, ymax=ymax,std_out=std_plots,
                      xlabel=xlabel_spc, ylabel=ylabel, title="ESF",text=ftitle,
                      szchar= sizeChar, posT= posText, scChar=scale)

        if plot_rtdi:
            _plotRTI(nplots, x1D=trti1D, y1D=yrti1D, rgb_data3D=rti3D,
                     xmin=tmin, xmax=tmax, ymin=ymin, ymax=ymax,
                     xlabel=xlabel_rti, ylabel=ylabel, title="RTDI",text=ftitle,
                     seconds = utc_epoch, std_out= std_plots,
                     szchar= sizeChar, posT =posText,scChar=scale)

            if plot_colormap:
                dplot.plot_colormap(nplots=nplots,
                                    xmin = cmap_xmin, xmax = cmap_xmax,
                                    ymin = cmap_ymin, ymax = cmap_ymax,
                                    std_plot=std_plots, szchar=2.2)

        dplot.closepage()
#         dplot.close()

    filename = None

    if save:
        filename = _getFilename(fpath, fprefix, ddatetime, ext, fsufix="")
        doyimg_name = "%s/%04d%03d%s"%(img_path,year,doy,ext)

        print "Saving %s" %filename

        dplot.figure(save=save, fname=filename, display_size=display_size)
        dplot.openpage()

        if plot_spec:
            _plotSpec(nplots, x2D=xspec2D, y2D=yspec2D, rgb_data3D=rgb_data3D,
                      xmin=xmin, xmax=xmax, ymin=ymin, ymax=ymax, std_out=std_plots,
                      xlabel=xlabel_spc, ylabel=ylabel, title="ESF",text=ftitle,
                      szchar= sizeChar, posT= posText, scChar=scale)

        if plot_rtdi:
            _plotRTI(nplots, x1D=trti1D, y1D=yrti1D, rgb_data3D=rti3D,
                     xmin=tmin, xmax=tmax, ymin=ymin, ymax=ymax,
                     xlabel=xlabel_rti, ylabel=ylabel, title="RTDI",text=ftitle,
                     seconds = utc_epoch, std_out= std_plots,
                     szchar= sizeChar, posT =posText,scChar=scale)

            if plot_colormap:
                dplot.plot_colormap(nplots=nplots,
                                    xmin = cmap_xmin, xmax = cmap_xmax,
                                    ymin = cmap_ymin, ymax = cmap_ymax,
                                    std_plot=std_plots, szchar=2.2)

        dplot.closepage()
        dplot.close()
        copyfile(filename, doyimg_name)

    return filename

if __name__ == '__main__':
    pass
