
import datetime
import numpy

def intList(list):
    
    outList = [int(i) for i in list]
    
    return outList

def floatList(list):
    
    outList = [float(i) for i in list]
    
    return outList

def toseconds(datetime_, epoch=datetime.datetime(1970,1,1)):
    
    delta = datetime_ - epoch
    
    return delta.total_seconds()

def findMinMaxIndex(vector, minValue=None, maxValue=None):
    
    minIndex = 0
    maxIndex = len(vector)-1
    
    if minValue != None:
        indxs = numpy.argwhere(vector>=minValue)
        if indxs.shape[0] != 0:
            minIndex = indxs[0][0]
        
    if maxValue != None:
        indxs = numpy.argwhere(vector<=maxValue)
        if indxs.shape[0] != 0:
            maxIndex = indxs[-1][0]
        
    return minIndex, maxIndex