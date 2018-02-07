import os
import sys
import fnmatch

def main(argv):
    assert len(argv) >=2, "usage: loggps.py oxts\data"

    directory = argv[1]
    assert os.path.isdir(directory), ("{0} is not a valid folder".format(directory))

    files = fnmatch.filter(os.listdir(directory), "*.txt")
    files.sort()

    with open("oxts-gps.txt", "w") as output_file:
        for file in files:
            path = os.path.join(directory, file)
            with open(path, "r") as sensor_file:
                data = sensor_file.readline()
                values = data.split(' ')
                output_file.write(values[0] + " " + values[1] + " " + values[2] + "\n")

if __name__ == '__main__':
    main(sys.argv)
