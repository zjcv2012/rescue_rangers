import ConfigParser
import sys

from location import *
from global_nav import *
from local_nav import *
from utils import *

def usage():
    print "Usage: python wpgen_main.py <loc_of_interest_file> <out_waypoints>"

def main():
    if len(sys.argv) != 3:
        usage()
        exit()

    loc_of_interest = sys.argv[1]
    out_waypoints = sys.argv[2]
     
    data = filter(lambda x: not x.startswith("#") , open(loc_of_interest, "r").readlines())
    locations = [Location([math.radians(float(loc)) for loc in location.rstrip().split(",")]) for location in data] 

    cfg = ConfigParser.ConfigParser()
    cfg.readfp(open("config.cfg"))
    local_wp = LocalWP(cfg)
    global_wp = GlobalWP(cfg, local_wp)

    waypoints = global_wp.getWaypoints(locations)
    out = open(out_waypoints, "w")
    for waypoint in waypoints:
        out.write(str(waypoint) + "\n")
    out.close()

    cartesian = Location.getApproxCartesianWaypoints(waypoints, locations[0])    
    plot([x[0] for x in cartesian], [x[1] for x in cartesian], [x[2] for x in cartesian])

if __name__ == "__main__":
    main()

# References
# http://www.movable-type.co.uk/scripts/latlong.html
# https://en.wikipedia.org/wiki/Decimal_degrees
# http://stackoverflow.com/questions/1185408/converting-from-longitude-latitude-to-cartesian-coordinates
# http://www.latlong.net/place/seattle-wa-usa-2655.html
# http://www.gps-coordinates.net/gps-coordinates-converter
# http://www.darrinward.com/lat-long/?id=2365962
# http://www.gpsvisualizer.com/map_input?form=data
# http://www.hamstermap.com/quickmap.php
