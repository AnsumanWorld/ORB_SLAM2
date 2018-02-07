import os
import sys

def main(argv):
    assert len(argv) >=2, "usage: points2oxts.py points.txt"

    points_file = argv[1]
    assert os.path.isfile(points_file), ("{0} is not a valid file".format(points_file))

    output_folder = "oxts-from-points"
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)
    assert os.path.isdir(output_folder), "Could not create folder {0}".format(output_folder)

    dummy = ["0.0"] * 27
    count = 0
    with open(points_file, "r") as ifile:
        poses = ifile.readlines()
        for pose in poses:
            values = pose.split(" ")
            
            x = values[0]
            y = values[1]
            z = values[2].strip("\n")
            others = ' '.join(dummy)
            data = f"{x} {y} {z} {others}"

            fname = f"{count:010}"
            output_file_name = fname + ".txt"
            output_file_path = os.path.join(output_folder, output_file_name)
            with open(output_file_path ,"w") as ofile:
                ofile.write(data)
                count += 1

if __name__ == '__main__':
    main(sys.argv)
