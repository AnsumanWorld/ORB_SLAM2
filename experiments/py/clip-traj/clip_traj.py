import sys
import logging

logging.basicConfig(level=logging.DEBUG)

def clip(gt_filepath, seq_filepath, gt_output):

    with open(gt_filepath, "r") as  gt_file:
        gt_lines = gt_file.readlines()

    with open(seq_filepath, "r") as seq_file:
        seq_lines = seq_file.readlines()

    len_gt = len(gt_lines)
    len_seq = len(seq_lines)
    assert (len_gt > len_seq), "Sequence file is larger than ground truth!"

    diff = len_gt - len_seq

    logging.info("Cipping {0} line(s) from {1}".format(diff, gt_filepath))

    with open(gt_output, "w") as out_file:
        out_file.writelines(gt_lines[diff:])

def main(argv):
    logging.info("argv={0}".format(argv))
    assert len(argv) == 4, "usage: clip_traj.py gt_filepath seq_filepath gt_output"

    gt_filepath = argv[1]
    seq_filepath = argv[2]
    gt_output = argv[3]

    clip(gt_filepath, seq_filepath, gt_output)

if __name__ == "__main__":
    main(sys.argv)
