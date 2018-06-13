import os
import sys

def main(argv):
    assert len(argv) >=2, "usage: gt2oxts.py gt.txt"

    gnd_truth_file = argv[1]
    assert os.path.isfile(gnd_truth_file), ("{0} is not a valid file".format(gnd_truth_file))

    output_folder = "oxts-from-gt"
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)
    assert os.path.isdir(output_folder), "Could not create folder {0}".format(output_folder)

    dummy = ["0.0"] * 27
    count = 0
    with open(gnd_truth_file, "r") as ifile:
        poses = ifile.readlines()
        for pose in poses:
            values = pose.split(" ")
            
            x = values[3]
            y = values[7]
            z = values[11].strip("\n")
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
