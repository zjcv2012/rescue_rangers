import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib.pyplot as plt
import math

R = 6370000
ANGLE_INCR = math.radians(30)
L_RAD = 20

class LatLong(object):
    def __init__(self, latlong):
        self.lat = float(latlong[0])
        self.long = float(latlong[1])

    def __str__(self):
        return str(self.lat) + "," + str(self.long) + " "
        
    def getLat(self):
        return self.lat    

    def getLong(self):
        return self.long

    @staticmethod
    def getBearing(latLong1, latLong2):
        diff = math.radians(latLong2.getLong())-math.radians(latLong1.getLong())
        x = math.cos(latLong2.getLat())*math.sin(diff)
        y = math.cos(latLong1.getLat())*math.sin(latLong2.getLat())-math.sin(latLong1.getLat())*math.cos(latLong2.getLat())*math.cos(diff)
        return math.atan2(x,y) if math.atan2(x,y) >= 0 else 2*math.pi + math.atan2(x,y)

    @staticmethod
    def getLocation(latLong, bearing, distance):
        lat1, lon1 = map(math.radians, [latLong.getLat(), latLong.getLong()])
        lat2 = math.asin(math.sin(lat1)*math.cos(float(distance)/R) + math.cos(lat1)*math.sin(float(distance)/R)*math.cos(bearing))
        lon2 = lon1 + math.atan2(math.sin(bearing)*math.sin(float(distance)/R)*math.cos(lat1), math.cos(float(distance)/R)-math.sin(lat1)*math.sin(lat2))
        lon2 = math.fmod((lon2+3*math.pi),(2*math.pi)) - math.pi;  
        return LatLong([math.degrees(lat2), math.degrees(lon2)])

    @staticmethod
    def getDistance(latLong1, latLong2):
        lat1, lon1 = map(math.radians, [latLong1.getLat(), latLong1.getLong()])
        lat2, lon2 = map(math.radians, [latLong2.getLat(), latLong2.getLong()])
        lon = lon2 - lon1 
        lat = lat2 - lat1 
        a = math.sin(lat/2)**2 + math.cos(lat1) * math.cos(lat2) * math.sin(lon/2)**2
        c = 2 * math.asin(math.sqrt(a)) 
        return c * R

    @staticmethod
    def getApproxCartesianDist(latLong1, latLong2):
        bearing = LatLong.getBearing(latLong1, latLong2)
        dist = LatLong.getDistance(latLong1, latLong2)
#        print math.degrees(bearing), dist
        dx = dist*math.cos(ANGLE_INCR)
        dy = dist*math.sin(ANGLE_INCR)
        return (dx, dy)
    
    @staticmethod    
    def getWaypoints1(latLong1, latLong2, step):
        waypoints = [latLong1]
        dist = LatLong.getDistance(latLong1, latLong2)
        bearing = LatLong.getBearing(latLong1, latLong2)
        currDist, currLatLong = 0, latLong1
        while currDist < dist:
            currLatLong = LatLong.getLocation(currLatLong, bearing, step)
            waypoints.append(currLatLong)            
            if currDist + 2*step < dist:
                local_waypoints = LatLong.getLocalNavigationWaypoints(currLatLong, bearing, L_RAD)
                [waypoints.append(l_waypoint) for l_waypoint in local_waypoints]
            currDist += step
        return waypoints

    @staticmethod
    def getWaypoints(g_waypoints):
        waypoints = [g_waypoints[0]]
        count = 1
        for count in range(len(g_waypoints)-1):
            prevWaypoint = g_waypoints[count-1]
            currWaypoint = g_waypoints[count]
            bearing = LatLong.getBearing(prevWaypoint, currWaypoint)
            local_waypoints = LatLong.getLocalNavigationWaypoints(currWaypoint, bearing, L_RAD)             
            [waypoints.append(l_waypoint) for l_waypoint in local_waypoints]
            count += 1
        waypoints.append(g_waypoints[-1])    
        return waypoints
    
    @staticmethod
    def getApproxCartesianWaypoints(waypoints, initLatLong):
        cartesianLocs = [[0, 0]]
        dx, dy = LatLong.getApproxCartesianDist(initLatLong, waypoints[0])
        cartesianLocs.append([dx, dy])
        for i in range(1, len(waypoints)):
            dx, dy = LatLong.getApproxCartesianDist(waypoints[i-1], waypoints[i])
            [x, y] = cartesianLocs[-1]
            cartesianLocs.append([x+dx, y+dy])
        return cartesianLocs

    @staticmethod
    def getLocalNavigationWaypoints(center, bearing, rad):
        local_waypoints = [LatLong.getLocation(center, bearing, rad)]
        inc = 0
        while inc <= 2*math.pi:
            nxt_loc = LatLong.getLocation(center, bearing+inc, rad)
            local_waypoints.append(nxt_loc)
            inc += ANGLE_INCR
        return local_waypoints
        
def plot(x, y, z):
    mpl.rcParams['legend.fontsize'] = 10
    fig = plt.figure()
    ax = fig.gca(projection='3d')
    ax.plot(x, y, z, label='drone path', linestyle='--', marker='o')
    ax.legend()
    x_start = x == 0
    y_start = y == 0
    plt.scatter(x[x_start], y[y_start], color='green')     
    plt.show()

def main():
    data = open("locations.txt", "r").readlines()
    locations = [LatLong(location.rstrip().split(",")) for location in data]
    waypoints = LatLong.getWaypoints(locations)

    count = 0 
    for waypoint in waypoints:
        print str(waypoint)
    
    #plot([x[0] for x in cartesian], [x[1] for x in cartesian], [0 for x in cartesian])
    
if __name__ == "__main__":
    main()
    
# References
# http://www.movable-type.co.uk/scripts/latlong.html
# http://www.gps-coordinates.net/gps-coordinates-converter
# https://en.wikipedia.org/wiki/Decimal_degrees
# http://stackoverflow.com/questions/1185408/converting-from-longitude-latitude-to-cartesian-coordinates
# http://www.latlong.net/place/seattle-wa-usa-2655.html
# http://www.gps-coordinates.net/gps-coordinates-converter
# http://www.darrinward.com/lat-long/?id=2365962
# http://www.gpsvisualizer.com/map_input?form=data
