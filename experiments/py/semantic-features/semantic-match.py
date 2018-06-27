import numpy as np
import cv2
from matplotlib import pyplot as plt

def show_image(img, option="plot", name="image"):
    if(option == "cv"):
        cv2.imshow(name, img)
        cv2.waitKey(0)
    elif(option == "plot"):
        plt.title(name)
        plt.imshow(img, cmap='gray')
        plt.show()
    else:
        raise ValueError(option + "not supported")

def get_orb_detector():
    return cv2.ORB_create(edgeThreshold=5, 
        patchSize=31, 
        nlevels=8, 
        fastThreshold=20, 
        scaleFactor=1.2, 
        WTA_K=2,
        scoreType=cv2.ORB_HARRIS_SCORE, 
        firstLevel=0,
        nfeatures=5000)

def translate(keypoints, offset):
    keypoints_copy = keypoints
    for keypoint in keypoints_copy:
        xt = keypoint.pt[0] + offset[0]
        yt = keypoint.pt[1] + offset[1]
        keypoint.pt = (xt, yt)
    return keypoints_copy

def draw_matches(image1, keypoints1, descriptor1, 
    image2, keypoints2, descriptor2):
    bf = cv2.BFMatcher(cv2.NORM_HAMMING, crossCheck=True)
    matches = bf.match(descriptor1, descriptor2)
    matches = sorted(matches, key = lambda x:x.distance)
    match_img = cv2.drawMatches(image1, keypoints1, 
        image2, keypoints2, 
        matches[:20], None, flags=2)
    show_image(match_img)

def draw_keypoints(image, keypoints):
    kp_img = cv2.drawKeypoints(image, keypoints, None, color=(0,255,0))
    show_image(kp_img)

def mask_image(image, bounding_polygon):
    mask = np.zeros(image.shape, dtype=np.uint8)
    roi = np.array(bounding_polygon)
    cv2.fillPoly(mask, roi, (255, 255, 255))
    masked_image = cv2.bitwise_and(image, mask)
    return masked_image

def rectangle_crop(image, bounding_box):
    #bb = ((top left x, y), (bottom right x, y))
    sub_image = image[bounding_box[0][1]:bounding_box[1][1], 
    bounding_box[0][0]:bounding_box[1][0]]
    return sub_image

def get_bounding_box_for_ts(img_name):
    if(img_name == "i1"):
        return ((372, 266), (417, 330))
    elif(img_name == "i2"):
        return ((342, 260), (392, 330))
    elif(img_name == "i3"):
        return ((235, 235), (297, 332))
    else:
        raise ValueError(img_name + "not supported")

def get_bounding_polygon_for_ts(img_name):
    if(img_name == "i1"):
        return [[(372, 268), (415, 267), (419, 330), (372, 333)]]
    elif(img_name == "i2"):
        return [[(342, 261), (392, 260), (394, 332), (344, 332)]]
    elif(img_name == "i3"):
        return [[(236, 234), (293, 231), (299, 331), (237, 334)]]
    else:
        raise ValueError(img_name + "not supported")

def get_bounding_polygon_for_lane(img_name):
    if(img_name == "i1"):
        return [[(664, 355), (692, 352), (962, 487), (813, 491)]]
    elif(img_name == "i2"):
        return [[(670, 359), (703, 358), (966, 491), (829, 501)]]
    elif(img_name == "i3"):
        return [[(695, 379), (738, 377), (982, 498), (831, 501)]]
    else:
        raise ValueError(img_name + "not supported")

def lane_match():
    i1 = cv2.imread("i1.png", cv2.IMREAD_GRAYSCALE)
    i2 = cv2.imread("i2.png", cv2.IMREAD_GRAYSCALE)
    i3 = cv2.imread("i3.png", cv2.IMREAD_GRAYSCALE)

    bp1 = get_bounding_polygon_for_lane("i1")
    bp2 = get_bounding_polygon_for_lane("i2")
    bp3 = get_bounding_polygon_for_lane("i3")

    b1 = mask_image(i1, bp1)
    b2 = mask_image(i2, bp2)
    b3 = mask_image(i3, bp3)

    orb = get_orb_detector()

    k1, d1 = orb.detectAndCompute(b1, None)
    k2, d2 = orb.detectAndCompute(b2, None)
    k3, d3 = orb.detectAndCompute(b3, None)

    show_image(b1)
    show_image(b2)
    show_image(b3)

    draw_keypoints(b1, k1)
    draw_keypoints(b2, k2)
    draw_keypoints(b3, k3)

    draw_matches(i1, k1, d1, i2, k2, d2)
    draw_matches(i2, k2, d2, i3, k3, d3)
    draw_matches(i1, k1, d1, i3, k3, d3)

def ts_match_bounding_polygon():
    i1 = cv2.imread("i1.png", cv2.IMREAD_GRAYSCALE)
    i2 = cv2.imread("i2.png", cv2.IMREAD_GRAYSCALE)
    i3 = cv2.imread("i3.png", cv2.IMREAD_GRAYSCALE)

    bp1 = get_bounding_polygon_for_ts("i1")
    bp2 = get_bounding_polygon_for_ts("i2")
    bp3 = get_bounding_polygon_for_ts("i3")

    b1 = mask_image(i1, bp1)
    b2 = mask_image(i2, bp2)
    b3 = mask_image(i3, bp3)

    show_image(b1)
    show_image(b2)
    show_image(b3)

    orb = get_orb_detector()

    k1, d1 = orb.detectAndCompute(b1, None)
    k2, d2 = orb.detectAndCompute(b2, None)
    k3, d3 = orb.detectAndCompute(b3, None)

    draw_keypoints(b1, k1)
    draw_keypoints(b2, k2)
    draw_keypoints(b3, k3)
    
    draw_matches(i1, k1, d1, i2, k2, d2)
    draw_matches(i2, k2, d2, i3, k3, d3)
    draw_matches(i1, k1, d1, i3, k3, d3)

def ts_match_bounding_box():
    i1 = cv2.imread("i1.png", cv2.IMREAD_GRAYSCALE)
    i2 = cv2.imread("i2.png", cv2.IMREAD_GRAYSCALE)
    i3 = cv2.imread("i3.png", cv2.IMREAD_GRAYSCALE)

    bb1 = get_bounding_box_for_ts("i1")
    bb2 = get_bounding_box_for_ts("i2")
    bb3 = get_bounding_box_for_ts("i3")

    b1 = rectangle_crop(i1, bb1)
    b2 = rectangle_crop(i2, bb2)
    b3 = rectangle_crop(i3, bb3)

    show_image(b1)
    show_image(b2)
    show_image(b3)

    orb = get_orb_detector()

    k1, d1 = orb.detectAndCompute(b1, None)
    tk1 = translate(k1, bb1[0])

    k2, d2 = orb.detectAndCompute(b2, None)
    tk2 = translate(k2, bb2[0])

    k3, d3 = orb.detectAndCompute(b3, None)
    tk3 = translate(k3, bb3[0])

    draw_matches(i1, tk1, d1, i2, tk2, d2)
    draw_matches(i2, tk2, d2, i3, tk3, d3)
    draw_matches(i1, tk1, d1, i3, tk3, d3)

def main():
    ts_match_bounding_box()
    #ts_match_bounding_polygon()
    #lane_match()

if __name__ == "__main__":
    main()
