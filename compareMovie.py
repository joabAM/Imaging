import cv2 as cv
import argparse
import numpy as np
video1 =""
video2 =""

def main(args):
    cap = cv.VideoCapture(args.movie1)
    cap1 = cv.VideoCapture(args.movie2)
    outvideo = cv.VideoWriter("CompareMovie.mp4", cv.VideoWriter_fourcc(*'mp4v'), 10, (1300,680))
    while True:
       ret1, img1 = cap.read()
       ret2, img2 = cap1.read()
       if ( np.shape(img1) == () or np.shape(img2) == () ):
           break
       merge_img = cv.vconcat((img1, img2))
       new_img = cv.resize(merge_img,(1300,680))
       cv.imshow('movies',new_img)
       outvideo.write(new_img)
       k = cv.waitKey(0)
       if k == 27:
          break

    cap.release()
    cap1.release()
    outvideo.release()
    cv.destroyAllWindows()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("movie1", type=str, help="movie 1")
    parser.add_argument("movie2", type=str, help="movie 2")
    args = parser.parse_args()
    print(args)
    main(args)
