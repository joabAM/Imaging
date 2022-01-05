import os
import plplot
import numpy

PNG_DRIVER = "pngcairo"
PDF_DRIVER = "pdfcairo"

SATURATION = 0.95
LIGHTNESS = 0.5

def _isOpen(id):

    cur_id = plplot.plgstrm()
    plplot.plsstrm(id)

    (xp0, yp0, xleng0, yleng0, xoff0, yoff0) = plplot.plgpage()

    if xleng0 == 0 and yleng0 == 0:
        return False

    return True

def _fill_contour(array, value=255, step=1):

    array[0,::step,:] = value
    array[-1,::step,:] = value
    array[::step,0,:] = value
    array[::step,-1,:] = value

    return array

def figure(id=None, device='xcairo', display_size = None, save=False, fname=None, multifiles=False):

    if id==None:
        id = plplot.plmkstrm()
    else:
        plplot.plsstrm(id)

#     plplot.plscolbg(255,255,255)

    if _isOpen(id):
        clear()
        return id

    if display_size == None:
        if save:
            display_size = "1200x1200"
        else:
            display_size = "750x750"

    plplot.plsetopt("geometry", display_size)

    if save:
        FILE_DRIVER = PNG_DRIVER

        if os.path.splitext(fname)[1].lower() == ".pdf":
            FILE_DRIVER = PDF_DRIVER

        plplot.plsdev(FILE_DRIVER)
        plplot.plsfnam(fname)

        if multifiles:
            plplot.plsfam(1, 0, 10000)

    else:
        plplot.plsdev(device)

    plplot.plscolbg(0,0,0)
    plplot.plscol0(15, 255, 255, 255)       #Font color

    plplot.plinit()

    plplot.plspause(0)
    plplot.pltimefmt("%H:%M")

    return id

def close(figure=None):

    if figure == "all":
#         plplot.plspause(1)
        plplot.plend()
    else:
        plplot.plend1()

def clear():

    plplot.plssub(1, 1)
    plplot.pladv(1)
    plplot.plclear()

def flush():

    plplot.plflush()

def openpage():

    plplot.plbop()

def closepage():

    plplot.pleop()

def subplot(nx, ny, nsubplot=0):

    plplot.plssub(nx,ny)
    plplot.pladv(nsubplot)

def axis(xmin, xmax, ymin, ymax, szchar=2.3,
         xpage_min=None, xpage_max=None, ypage_min=None, ypage_max=None,
         xlabel_ticks=True, ylabel_ticks=True,
         xtime = False, ytime = False,
         xopt="bcts", yopt = "bctsv"):

    #"b": Draws bottom (X) and left (Y) edge of frame and
    #"c": Draws top (X) and right (Y) edge of frame
    #"t": Ticks
    #"s": Subticks

#     xopt = "bcts"
#     yopt = "bctsv"

    #Numeric labels on major ticks
    if xlabel_ticks:
        xopt += "n"

    if xtime:
        xopt += "d"

    if ylabel_ticks:
        yopt += "n"

    if ytime:
        yopt += "d"

    old_szchar = plplot.plgchr()
#     plplot.plscol0(15, 255, 255, 255)
    plplot.plcol0(15)
    plplot.plschr(1, szchar)

    if xpage_min != None and xpage_max != None and ypage_min != None and ypage_max != None:
        plplot.plvpor(xpage_min, xpage_max, ypage_min, ypage_max)
        plplot.plwind(xmin, xmax, ymin, ymax)
#         set_background(xmin, xmax, ymin, ymax)

        plplot.plcol0(15)
        plplot.plbox(xopt, 0.0, 0, yopt, 0.0, 0)
    else:
        plplot.plcol0(15)
        plplot.plenv0(xmin, xmax, ymin, ymax, 0, 0)
#         set_background(xmin, xmax, ymin, ymax)

    plplot.plschr(old_szchar[0], old_szchar[1])

def label(xlabel, ylabel, title="", szchar=2.3):

    plplot.plcol0(15)
    plplot.plschr(1, szchar)
    plplot.pllab(xlabel, ylabel, title)
#     plplot.plflush()

def ptitle(text, szchar=3.0, pos=1):

    plplot.plcol0(15)
    plplot.plschr(1, szchar)
    plplot.plmtex("b", pos, 0.5, 0.5, text)
#     plplot.plflush()

def ftitle(text, szchar=3.0, pos=-0.75):

    plplot.plcol0(15)
    plplot.plssub(1, 1)
    plplot.pladv(1)
    plplot.plvpor(0.0, 1.0, 0.0, 1.0)
    plplot.plschr(1, szchar)
    plplot.plmtex("t", pos, 0.5, 0.5, text)
#     plplot.plflush()

def set_background(xmin, xmax, ymin, ymax, r=0, g=0, b=0):

    plplot.plcol0(13)
    plplot.plfill( (xmin, xmax, xmax, xmin),
                   (ymin, ymin, ymax, ymax)
                 )

def pcolor_basic(x1D, y1D, array3D, set_axis=True, opaque=12,
                 xmin=None, xmax=None, ymin=None, ymax=None,
                 xlabel_ticks=True, ylabel_ticks=True):
    """
    Inputs:
        x1D    :    array_like. 1-D numpy array
        y1D    :    array_like. 1-D numpy array
        array3D:    array_like. 3-D numpy array used to plot.
                  axis 2 is used as RGB value

        opaque:    Every RGB value (color) less than opaque is filtered

                   For opaque = #F0 then:
                        IF rgb_value < #F0F0F0 then this color is not plotted

    """

    if xmin == None: xmin = numpy.min(x1D)
    if xmax == None: xmax = numpy.max(x1D)
    if ymin == None: ymin = numpy.min(y1D)
    if ymax == None: ymax = numpy.max(y1D)

    nx, ny, ncolors = array3D.shape

    xw = (x1D[-1] - x1D[-2])*1.2
    yw = (y1D[-1] - y1D[-2])*1.2

    if set_axis:
        axis(xmin, xmax+xw, ymin, ymax+yw, xlabel_ticks=xlabel_ticks, ylabel_ticks=ylabel_ticks)

    #Deleting opaque colors
    ind_invalid = (array3D[:,:] < numpy.ones(3)*opaque).all(axis=2)
    xyrange = numpy.where(ind_invalid==False)

    for i, j in zip(xyrange[0], xyrange[1]):
        plplot.plscol0(11, array3D[i,j][0], array3D[i,j][1], array3D[i,j][2])
        plplot.plcol0(11)

        if i < nx-1: x1Df = x1D[i+1]
        else:        x1Df = x1D[i]+xw

        if j < ny-1: y1Df = y1D[j+1]
        else:        y1Df = y1D[j]+yw

        plplot.plfill( (x1D[i], x1Df  , x1Df, x1D[i]  ),
                       (y1D[j], y1D[j], y1Df, y1Df)
                     )

#     plplot.plflush()

def pcolor(x2D, y2D, array3D, set_axis=True, opaque=12, contour=False,
           xmin=None, xmax=None, ymin=None, ymax=None,
           xlabel_ticks=True, ylabel_ticks=True):
    """
    Inputs:
        x2D    :    array_like. 2-D numpy array (nx, ny)
        y2D    :    array_like. 2-D numpy array (nx, ny)
        array3D:    array_like. 3-D numpy array used to plot.  (nx, ny, rgb)
                  axis 2 is used as RGB value

        opaque:    Every RGB value (color) less than opaque is filtered

                   For opaque = #F0 then:
                        If rgb_value < (#F0, #F0, #F0) then this color is not plotted

    """

    if xmin == None: xmin = numpy.min(x2D)
    if xmax == None: xmax = numpy.max(x2D)
    if ymin == None: ymin = numpy.min(y2D)
    if ymax == None: ymax = numpy.max(y2D)

    nx, ny, ncolors = array3D.shape

    xw = (2*x2D[-1,:] - x2D[-2,:])
    x_ = numpy.append(x2D, xw.reshape(1,-1), axis=0)

    xw = (2*x_[:,-1] - x_[:,-2])
    x_ = numpy.append(x_, xw.reshape(-1,1), axis=1)

    yw = (2*y2D[:,-1] - y2D[:,-2])
    y_ = numpy.append(y2D, yw.reshape(-1,1), axis=1)
    yw = (2*y_[-1,:] - y_[-2,:])
    y_ = numpy.append(y_, yw.reshape(1,-1), axis=0)

    if set_axis:
        axis(xmin, xmax, ymin, ymax, xlabel_ticks=xlabel_ticks, ylabel_ticks=ylabel_ticks)

    if contour:
        array3D = _fill_contour(array3D)

    #Deleting opaque colors
    ind_invalid = (array3D[:,:] < numpy.ones(3)*opaque).all(axis=2)
    xyrange = numpy.where(ind_invalid==False)

    for i, j in zip(xyrange[0], xyrange[1]):
        plplot.plscol0(11, array3D[i,j][0], array3D[i,j][1], array3D[i,j][2])
        plplot.plcol0(11)
        plplot.plfill( (x_[i,j], x_[i+1,j], x_[i+1,j+1], x_[i,j+1]),
                       (y_[i,j], y_[i+1,j], y_[i+1,j+1], y_[i,j+1])
                     )

#     plplot.plflush()

def pline(x1D, y1D, icol=14, line=1):

    plplot.plcol0(icol)
    plplot.pllsty(line)
    plplot.plline(x1D, y1D)
    plplot.pllsty(1)

def parrow(x1D, y1D, icol=14, line=1, width=3):
    np = 1

    xg = numpy.array(x1D[1]).reshape((np,np))
    yg = numpy.array(y1D[1]).reshape((np,np))

    u = numpy.zeros((np,np))
    v = -1.0*numpy.ones((np,np))

    scale = (y1D[1]-y1D[0])/10.0

    plplot.plcol0(icol)
    plplot.pllsty(line)
    plplot.plwidth(width)
    plplot.plvect(u, v, scale, "pltr2", xg, yg)
    plplot.pllsty(1)
    plplot.plwidth(1)

def save(fname):
    # Get current stream and create a new one
    cur_strm = plplot.plgstrm()
    new_strm = plplot.plmkstrm()

    # Set new device type and file name - use a known existing driver
    plplot.plsdev("pngcairo")
    plplot.plsfnam(fname)

    # Copy old stream parameters to new stream, do the save,
    # then close new device.
    plplot.plcpstrm(cur_strm,0)
#     plplot.plreplot()
#     plplot.plend1()

    # Return to previous stream
#     plplot.plsstrm(cur_strm)

def create_colormap_scut(nx = 45, ny = 50, s=SATURATION):

    colormap = numpy.empty((nx, ny, 3), dtype=numpy.float)

    for ix in range(nx):
        h = ix*360.0/(nx-1) - 60

        for iy in range(ny):
            l = iy*1.0/(ny-1)
            colormap[ix,iy,:] = numpy.array(plplot.plhlsrgb(h, l, s))

    colormap = colormap*255

    return colormap.astype(int)

def create_colormap_lcut(nx = 45, ny = 50, l=LIGHTNESS):

    colormap = numpy.empty((nx, ny, 3), dtype=numpy.float)

    for ix in range(nx):
        h = ix*360.0/(nx-1) - 60

        for iy in range(ny):
            s = (ny-1-iy)*1.0/(ny-1)
            colormap[ix,iy,:] = numpy.array(plplot.plhlsrgb(h, l, s))

    colormap = colormap*255

    return colormap.astype(int)

def plot_colormap(nplots, xmin, xmax, ymin, ymax,
                 szchar = 2.2, std_plot=False):

    subplot(1, 1, 1)

    ypage_min = 0.155
    ypage_max = 0.265

    #SNR MAP
    if nplots == 1:
        xpage_min = 0.13
        xpage_max = 0.21
        if std_plot:
            ypage_min = 0.759
            ypage_max = 0.869
            xpage_min = 0.188
            xpage_max = 0.258
#0.08 0.045
    if nplots == 2:
        xpage_min = 0.37
        xpage_max = 0.43

    axis(xmin, xmax, ymin, ymax, szchar=szchar/1.1,
        xpage_min=xpage_min, xpage_max=xpage_max,
        ypage_min=ypage_min, ypage_max=ypage_max,
        xopt="bct", yopt="bctv")

    x1D = numpy.arange(xmin, xmax, 1.0*(xmax-xmin)/SNR_MAP.shape[0])
    y1D = numpy.arange(ymin, ymax, 1.0*(ymax-ymin)/SNR_MAP.shape[1])

    pcolor_basic(x1D, y1D, SNR_MAP, set_axis=False, opaque=0)
    label(xlabel="Doppler (m/s)", ylabel="SNR (dB)",
          title="SW = %3.2f m/s" %( (xmax-xmin)*(1-SATURATION) ),
          szchar=szchar)

    #SW MAP
    if nplots == 1:
        xpage_min = 0.255
        xpage_max = 0.335
        if std_plot:
            xpage_min = 0.315
            xpage_max = 0.385
    if nplots == 2:
        xpage_min = 0.47
        xpage_max = 0.53

    axis(xmin, xmax, 0, (xmax - xmin), szchar=szchar/1.1,
               xpage_min=xpage_min, xpage_max=xpage_max,
               ypage_min=ypage_min, ypage_max=ypage_max,
               xopt="bct", yopt="bctv")

    x1D = numpy.arange(xmin, xmax, 1.0*(xmax-xmin)/SW_MAP.shape[0])
    y1D = numpy.arange(0, (xmax - xmin), 1.0*(xmax-xmin)/SW_MAP.shape[1])

    pcolor_basic(x1D, y1D, SW_MAP, set_axis=False, opaque=0)
    label(xlabel="Doppler (m/s)", ylabel="SW (m/s)",
                title="SNR = %3.2f dB" %((ymax-ymin)*LIGHTNESS + ymin),
                szchar=szchar)

SNR_MAP = create_colormap_scut()
SW_MAP = create_colormap_lcut()

if __name__ == '__main__':
    import numpy
    nx = 256
    ny = 500

    x = numpy.arange(nx)
    y = numpy.arange(ny)
    array = numpy.random.rand(nx, ny, 3)*255
    array = array.astype(int)
    array = numpy.clip(array, 150, 255) - 150

    figure(1)
    subplot(2,2,1)
    plot2D_(x, y, array)
    subplot(2,2,2)
    plot2D_(x, y, array)
    subplot(2,2,3)
    plot2D_(x, y, array)
    subplot(2,2,4)
    plot2D_(x, y, array)
    save("./test1.png")
    close()

    figure(1)
    subplot(2,2,1)
    plot2D(x, y, array)
    subplot(2,2,2)
    plot2D(x, y, array)
    subplot(2,2,3)
    plot2D(x, y, array)
    subplot(2,2,4)
    plot2D(x, y, array)
    save("./test2.png")
    close()
