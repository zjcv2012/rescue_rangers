# import the necessary packages
import numpy as np
import argparse
import cv2

# d = -3.77r + 473.55 

def draw_circle_boundary(circles, image):
    # loop over the (x, y) coordinates and radius of the circles
    for (x, y, r) in circles:
        # draw the circle in the output image, then draw a rectangle
        # corresponding to the center of the circle
        cv2.circle(image, (x, y), r, (0, 255, 0), 4)
 
        # show the output image
        cv2.imshow("output", image)
        cv2.waitKey(0)

def get_dist(r):
    return -3.77 * r + 473.55
        
def detect(gray, image):
    # detect circles in the image
    circles = cv2.HoughCircles(gray, cv2.cv.CV_HOUGH_GRADIENT, 1.2, 10, param1=100,
                               param2=35, minRadius=20, maxRadius=0)
    
    # ensure at least some circles were found
    if circles is not None:
        print len(circles)

        # convert the (x, y) coordinates and radius of the circles to integers
        circles = np.round(circles[0, :]).astype("int")

        for (x, y, r) in circles:
            print x, y, r, get_dist(r)
        
        draw_circle_boundary(circles, image);
            
    else:
        print "No circles"


# construct the argument parser and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-i", "--image", required = True, help = "Path to the image")
args = vars(ap.parse_args())

# load the image, clone it for output, and then convert it to grayscale
image = cv2.imread(args["image"])
#imres = cv2.resize(image, (120, 120))

hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

#lower_red = np.array([0,100,100])
#upper_red = np.array([10,255,180])
lower_red = np.array([160,100,100])
upper_red = np.array([179,255,255])
    
mask = cv2.inRange(hsv, lower_red, upper_red)
res = cv2.bitwise_and(image, image, mask = mask)

gray = cv2.cvtColor(res, cv2.COLOR_BGR2GRAY)

# detect circles in the image
detect(gray, image)
