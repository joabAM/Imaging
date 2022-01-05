'''
Created on Jul 12, 2010

@author: roj-idl71
'''
import os
import subprocess


"""
    Change history:

    Written by "Miguel Urco":mailto:murco@jro.igp.gob.pe  Set 9, 2010

    Esta clase es usada para generar movies a partir de archivos png.
"""

def mkMovie(imagepath, moviefilename = 'movie.avi', vcodec='mpeg4', vbitrate=400, type='png', fps=3, pattern='*'):
    """
    Genera un movie a partir de las imagenes almacenadas en el directorio especificado.

    It generates a movie from images stored on imagepath.

    """
    #Mantiene la calidad de la imagen.
    #cmd = mencoder -ovc lavc -lavcopts vcodec=mpeg4:vbitrate=1500 -vf scale=800 -mf type=png:fps=5 -nosound -o output.avi mf://*.png
    #cmd = mencoder -ovc lavc -lavcopts vcodec=mpeg4:vbitrate=1500 -vf scale=800 -mf type=png:fps=5 -nosound -of mpeg -o output.mpg mf://*.png
    cmd = 'mencoder -ovc lavc -lavcopts vcodec=%s:vbitrate=%d -of lavf -mf type=%s:fps=%d -nosound -o %s "mf://%s/%s.png"' %(vcodec,
                                                                                                                   vbitrate,
                                                                                                                   type,
                                                                                                                   fps,
                                                                                                                   moviefilename,
                                                                                                                   imagepath,
                                                                                                                   pattern)

    print cmd

    p = subprocess.Popen(cmd, shell=True)
    os.waitpid(p.pid, 0)

    new_name = moviefilename[0:-11]+'ccf-spc'+moviefilename[-11:]
    print new_name
    cmd2 = 'yes y | ffmpeg -i %s %s'%(moviefilename,new_name)
    print cmd2
    q = subprocess.Popen(cmd2, shell=True)
    os.waitpid(q.pid, 0)

    os.remove(moviefilename)

if __name__ == '__main__':
    imagepath = "/home/murco/Data/PROCDATA/IMAGING"
    # Script mkMovie.py.py
    # This script only calls the init function of the class mkMovie
    # All work is done by the init function
    obj = mkMovie(imagepath)
