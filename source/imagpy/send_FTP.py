from ftplib import FTP
import os
dpath = "/home/soporte/Documentos/Imaging/OUTPUTmp4/MOVIES/"
user = "wmaster"
password = "mst2013vhf"
year = 2018
#domain name or server ip:



ftp = FTP('jro-db.igp.gob.pe')
ftp.login(user=user, passwd = password)
ftp.cwd("database/juliadata/julia_imaging/"+str(year))
movie_list = os.listdir(dpath)
movie_list.sort()
fail_list = []
for i in range(len(movie_list)):
    if movie_list[i].endswith(".mp4"):

        #movieYYYYddd_008.avi
        filename = dpath+movie_list[i]
        try:
            print('sending... ',movie_list[i])
            file = open(filename,'rb')
            ftp.storbinary('STOR '+movie_list[i], file)
            file.close()
        except:
            fail_list.append(movie_list[i])
ftp.quit()
print("Fail movies: ", fail_list)
