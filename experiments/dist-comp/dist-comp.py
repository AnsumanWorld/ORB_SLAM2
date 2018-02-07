import os
import sys
import math
import logging

logging.basicConfig(level=logging.DEBUG)

earth_radius = 6371.01 * 1000.0 #meter

def get_gps_from_oxts(folder):
    ret_list = []
    for file in os.listdir(folder):
        if file.endswith(".txt"):
            with open(os.path.join(folder, file), "r") as oxt_file:
                oxt_info = oxt_file.readline()
                oxt_values = oxt_info.split(" ")
                lati = oxt_values[0]
                longi = oxt_values[1]
                alti = oxt_values[2]
                tpl = (lati, longi, alti)
                logging.debug(f"adding gps data {tpl}")
                ret_list.append(tpl)

    return ret_list

def get_xyz_from_gt(gt_file):
    ret_list=[]
    with open(gt_file, "r") as f:
        poses = f.readlines()
        for pose in poses:
            values = pose.split(" ")
            x = values[3]
            y = values[7]
            z = values[11].strip("\n")
            tpl = (x, y, z)
            logging.debug(f"adding ground truth data {tpl}")
            ret_list.append(tpl)

    return ret_list

def euclidean_distance(x1, y1, z1, x2, y2, z2):
    dx = float(x1) - float(x2)
    dy = float(y1) - float(y2)
    dz = float(z1) - float(z2)
    return math.sqrt(dx**2 + dy**2 + dz**2)

def geo_haversine_distance(lati1, longi1, lati2, longi2):
    lat1 = math.radians(float(lati1))
    lon1 = math.radians(float(longi1))
    lat2 = math.radians(float(lati2))
    lon2 = math.radians(float(longi2))

    dlon = lon2 - lon1
    dlat = lat2 - lat1
    a = math.sin(dlat / 2)**2 + math.cos(lat1) * math.cos(lat2) * math.sin(dlon / 2)**2
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))

    distance = earth_radius * c
    return distance

def geo_euclidean_distance(lati1, longi1, lati2, longi2):
    slat = math.radians(float(lati1))
    slon = math.radians(float(longi1))
    elat = math.radians(float(lati2))
    elon = math.radians(float(longi2))

    start_x = earth_radius * math.cos(slat) * math.cos(slon)
    start_y = earth_radius * math.cos(slat) * math.sin(slon)
    start_z = earth_radius * math.sin(slat)

    end_x = earth_radius * math.cos(elat) * math.cos(elon)
    end_y = earth_radius * math.cos(elat) * math.sin(elon)
    end_z = earth_radius * math.sin(elat)

    return euclidean_distance(start_x, start_y, start_z,
                                end_x, end_y, end_z)

def calculate_all_distances(gps_values, gt_values):
    if len(gps_values) != len(gt_values):
        raise ValueError("invalid lengths")

    ret_list = []
    for idx, (gps, gt) in enumerate(zip(gps_values, gt_values)):
        if(idx == 0):
            logging.debug(f"index is {idx} continuing")
            continue

        prev_idx = idx - 1
        dist_geo_euc = geo_euclidean_distance(
            gps_values[prev_idx][0], gps_values[prev_idx][1],
            gps_values[idx][0], gps_values[idx][1]
        )

        dist_geo_hav = geo_haversine_distance(
            gps_values[prev_idx][0], gps_values[prev_idx][1],
            gps_values[idx][0], gps_values[idx][1]
        )

        dist_xyz = euclidean_distance(
            gt_values[prev_idx][0], gt_values[prev_idx][1], gt_values[prev_idx][2],
            gt_values[idx][0], gt_values[idx][1], gt_values[idx][2],
        )

        e1 = dist_geo_euc - dist_geo_hav
        e2 = dist_geo_hav - dist_xyz
        e3 = dist_geo_euc - dist_xyz
        tpl = (dist_geo_euc, dist_geo_hav, dist_xyz, e1, e2, e3)
        ret_list.append(tpl)

    return ret_list

def log_distances(gps_values, gt_values):
    distances = calculate_all_distances(gps_values, gt_values)
    with open("dist-comp.txt", "w") as f:
        for dist_geo_euc,  dist_geo_hav, dist_xyz, e1, e2, e3 in distances:
            distances = f"{dist_geo_euc}, {dist_geo_hav}, {dist_xyz}, {e1}, {e2}, {e3} "
            logging.debug(f"dist_geo = {dist_geo_euc}, dist_geo_hav = {dist_geo_hav}, dist_xyz = {dist_xyz}")
            f.write(distances + "\n")

def main(argv):
    logging.debug(f"argv={argv}")
    assert len(argv) >= 3, "usage: dist-comp.py gt.txt oxts-folder"

    gnd_truth_file = argv[1]
    oxts_folder = argv[2]
    logging.debug(f"gnd_truth_file = {gnd_truth_file} oxts_folder = {oxts_folder}")

    gps_values = get_gps_from_oxts(oxts_folder)
    xyz_values = get_xyz_from_gt(gnd_truth_file)

    log_distances(gps_values, xyz_values)

if __name__ == "__main__":
    main(sys.argv)
