import numpy as np
import cv2
from matplotlib import pyplot as plt
import os
import sys
import glob as glob

def get_orb_detector():
    return cv2.ORB_create(edgeThreshold=19, 
        patchSize=31, 
        nlevels=8, 
        fastThreshold=20, 
        scaleFactor=1.2, 
        WTA_K=2,
        scoreType=cv2.ORB_HARRIS_SCORE, 
        firstLevel=0,
        nfeatures=3500)

def get_keypoints(image_file):
    orb = get_orb_detector()
    img = cv2.imread(image_file, cv2.IMREAD_GRAYSCALE)
    kp = orb.detect(img)
    return kp

def detect_from_image_folder(input_folder):
    number_of_files = len(os.listdir(input_folder))
    total_kp = 0
    files_processed = 0
    for image_index in range(0, 999999):
        if files_processed == number_of_files:
            break
        file_prefix = "{:06d}".format(image_index)
        path_pattern = os.path.join(input_folder, file_prefix + '.*')
        for image_file in glob.glob(path_pattern):
            files_processed += 1
            kp = get_keypoints(image_file)
            total_kp += len(kp)

    print("Frames =", files_processed, ", Frames Key-points =", total_kp)

def main():
    assert len(sys.argv) == 2, (
    "Not enough arguments. Provide input folder with images")
    assert os.path.isdir(sys.argv[1]), (
    "{0} is not a valid folder!".format(sys.argv[1]))
    input_folder = sys.argv[1]
    detect_from_image_folder(input_folder)

if __name__ == "__main__":
    main()
