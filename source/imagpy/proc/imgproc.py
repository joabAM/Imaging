'''
Created on May 25, 2010

@author: murco
'''
import numpy
from scipy import ndimage

def _removeInterf(data):
    '''
    '''
    return(data)

def _removeDC(data):
    '''
    '''
    return(data)
    
    
def _shift(array, n):
    """
        Shift the array A in place. Positive n -> shift right, negative n -> shift left
        
        Inputs:
            array    :    array_like. 4D numpy array
            n        :    number of points to shift on the last dimension
    """
    if n==0:
        return array
    
    A = numpy.copy(array)
    if n > 0:
        temp = numpy.copy(A[:,:,:,-n:])
        A[:,:,:,n:] = A[:,:,:,:-n]
        A[:,:,:,:n] = temp
    elif n < 0:
        n = abs(n)
        temp = numpy.copy(A[:,:,:,:n])
        A[:,:,:,:-n] = A[:,:,:,n:]
        A[:,:,:,-n:] = temp
    
    return A 

def _percentile(data4D, percentile, fillvalue):
    
    tmp = numpy.percentile(data4D, percentile, axis=(0,1))
    data4D =  data4D - tmp
            
    #finding negative values
    data4D = numpy.where(data4D < fillvalue, fillvalue, data4D)
    
    return data4D
           
def gen_color_indexes(nFFTPoints, nbins=None):
    """ Used to select FFT Points
        
        Input:
            nFFTPoints is the number of total fft points
            nbins is the number of points centered around zero used to average the data
        
        Output
            Indexes used in color composition
            
            ind_r: red color indexes
            ind_g: green color indexes
            ind_b: blue color indexes
            
            weights: weight for each index. It should be used when data is averaged
        
        Example:
        
            1. If nFFTPoints is 16 and nbins is 4
            
            Input:
            
                nFFTPoints:        0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
                nbins     :                           0  1  2  3
            
            
                Indexes to use                       6  7  8  9
                -----------------------------------------------
            Output:
                ind_r:                                  7
                ind_g:                                     8
                ind_b:                                        9
                
                weights_r:                              1
                weights_g:                                 1
                weights_b:                                    1

                so:
                
                weights = 1
                
            2. If nFFTPoints is 16 and nbins is 8.
            
            Input:
            
                nFFTPoints:        0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
                nbins      :                    0  1  2  3  4  5  6  7
            
                Indexes to use                 4  5  6  7  8  9 10 11
            Output
            
                ind_r:                            5' 6  7'      
                ind_g:                                  7' 8  9'
                ind_b:                                        9'10 11'
                
                weights_r:                        1  1  1
                weights_g:                              1  1  1
                weights_b:                                    1  1  1
                
                so:
                
                weights = (1  1  1)
                    
            3. If nFFTPoints is 16 and nbins is 16
            
            Input:
            
                nFFTPoints:        0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
                nbins      :        0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
            
                Indexes to use     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
                
            Output:
                ind_r:                1  2  3  4  5    
                ind_g:                               6  7  8  9 10
                ind_b:                                             11 12 13 14 15
                
                weights_r:            1  1  1  1  1
                weights_g:                           1  1  1  1  1
                weights_b:                                          1  1  1  1  1
                
                so:
                
                weights = (1  1  1  1  1)
                    
    """
    
    if nbins==None:
        nPoints = nFFTPoints
    else:
        nPoints = nbins
    
    if nPoints > nFFTPoints:
            raise "The number of points to use (%d) should be less than number of FFT points (%d)" %(nPoints, nFFTPoints)
        
    size = int(nPoints/3)
    
    if size<0:
        raise "selected number of beam points should be greater or equal than 3" %(nPoints)
    
    if ((size % 2) != 0):
        ind_r = numpy.arange(size) + int(nFFTPoints/2) - int(size/2) - size
        ind_g = numpy.arange(size) + int(nFFTPoints/2) - int(size/2)
        ind_b = numpy.arange(size) + int(nFFTPoints/2) - int(size/2) + size
        weights = numpy.ones(size, dtype=numpy.float)
    else:                                                 
        ind_r = numpy.arange(size+1) + int(nFFTPoints/2) - int(size/2) - size
        ind_g = numpy.arange(size+1) + int(nFFTPoints/2) - int(size/2)
        ind_b = numpy.arange(size+1) + int(nFFTPoints/2) - int(size/2) + size
        weights = numpy.ones(size+1, dtype=numpy.float)
#         weights[0] = weights[-1] = 0.5
    
    print "FFT points: Red =", ind_r, "Green =", ind_g, "Blue =", ind_b
#     print "Weights=", weights
    
    return ind_r, ind_g, ind_b, weights

def dBData2Linear(data_dB, remove_interf=False, remove_dc=False, fft_shift=False,
                  smooth = False, percentile = None,
                  min_value=1e-20):
    """
    Inputs:
        data_dB      :    4D numpy array (float). data values should be in dB.
                          Array dimension is (nx, ny, nz, 3).
    """
    
    nx, ny, nHeis, nFFTPoints = data_dB.shape
    
    #Lineal
    linear_data = numpy.power(10., data_dB/10.)
    linear_data = numpy.where(numpy.isfinite(linear_data), linear_data, min_value)
    linear_data = numpy.where(linear_data>min_value, linear_data, min_value)
    
    if remove_interf==True: linear_data = _removeInterf(linear_data)
    if remove_dc==True:     linear_data = _removeDC(linear_data)
    if fft_shift==True:     linear_data = _shift(linear_data, nFFTPoints/2)
    
    if percentile is not None:
       linear_data = _percentile(linear_data, percentile, fillvalue=min_value)
           
    if smooth:
        linear_data = ndimage.median_filter(linear_data, (3,3,3,1))
        
    return linear_data
        
def binFilter(linear_data, nbins=None, ind_colors=None):
    """
    Inputs:
        linear_data  :    4D array_like (float). Linear data values should not be in dB.
                          Array dimension is (nx, ny, nz, nffts).
        
        nbins        :
        
        ind_colors    :    list got from gen_color_indexes (ind_r, ind_g, ind_b, weights)
        
    Output:
    
        bin_data    :    4D array_like (float). Array dimension (nx, ny, nz, 3)
        
    """
                       
    data = linear_data
    nx, ny, nheis, nFFTPoints = data.shape
        
    bin_data = numpy.empty((nx,ny,nheis,3))
    
    if ind_colors is None:
        ind_r, ind_g, ind_b, weights = gen_color_indexes(nFFTPoints, nbins)
    else:
        ind_r, ind_g, ind_b, weights = ind_colors
    
    bin_data[:,:,:,0] = numpy.average(data[:,:,:,ind_r], axis=3, weights=weights)
    bin_data[:,:,:,1] = numpy.average(data[:,:,:,ind_g], axis=3, weights=weights)
    bin_data[:,:,:,2] = numpy.average(data[:,:,:,ind_b], axis=3, weights=weights)
    
    return bin_data

def genZaxis(zmin, zmax, zstep):
    
    nz = int(numpy.ceil((zmax - zmin)/zstep + 0.5))
    
    z = numpy.arange(nz)*zstep + zmin
    
    return z
    
def translateSphereArray2XYZ(range3D, array4D, z, fillvalue=1e-20):
    
    if array4D.shape[:-1] != range3D.shape:
        raise ValueError, "range3D should have the same first three dimensions than array4D"
    
    nz = len(z)
    ztep2 = (z[1] - z[0])/2.
    
#     nx, ny, nr, nc = array4D.shape
#     new_array4D = numpy.empty((nx, ny, nz, nc))

    dims = array4D.shape
    ndim = len(dims)
    
    nc = dims[-1]
    new_dims = list(dims[:-2])
    new_dims.append(nz)
    new_dims.append(nc)
    new_array4D = numpy.empty(new_dims)
    
    for iz in range(nz):
        zi = z[iz]
        ind_out_zi = numpy.abs(range3D - zi - ztep2) > ztep2
        array4D_masked = numpy.ma.masked_where(numpy.multiply.outer(ind_out_zi, numpy.ones(nc)), array4D)
        array_zi_masked = array4D_masked.mean(axis=ndim-2)
        array_zi = array_zi_masked.filled(fillvalue)
        
        if ndim == 4:
            new_array4D[:,:,iz,:] = array_zi
            continue
        
        if ndim == 3:
            new_array4D[:,iz,:] = array_zi
            continue
        
        if ndim == 2:
            new_array4D[iz,:] = array_zi
            continue
            
    return new_array4D

def mask_average(data, min_value=1e-10, fill_value=1e-20, axis=0):
    
    mask = numpy.ma.masked_less_equal(data, value=min_value)
    mask_avg = numpy.ma.average(mask, axis=axis)
    avg_data = mask_avg.filled(fill_value)
    
    return avg_data

def max_index(data):
    
    nx, ny = data.shape
    
    total = 0
    for i in range(nx):
        mid = numpy.sum(data[i] * numpy.arange(ny))/ numpy.sum(data[i])
        total += mid
        
    return int(total/nx)
    
    
                