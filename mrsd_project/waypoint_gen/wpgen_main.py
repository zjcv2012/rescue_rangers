import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib.pyplot as plt
import math

mpl.rcParams['legend.fontsize'] = 10

R = 6370000

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
        diff = latLong2.getLong()-latLong1.getLong()
        x = math.cos(latLong2.getLat())*math.sin(diff)
        y = math.cos(latLong1.getLat())*math.sin(latLong2.getLat())-math.sin(latLong1.getLat())*math.cos(latLong2.getLat())*math.cos(diff)
        return math.atan2(x,y)

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
        lat1, lon1 = latLong1.getLat(), latLong1.getLong()
        lat2, lon2 = latLong2.getLat(), latLong2.getLong()
        dx = (lon2-lon1)*2*math.pi*R*math.cos((lat1+lat2)*math.pi/360)/360
        dy = (lat1-lat2)*2*math.pi*R/360
        return (dx, dy)
    
    @staticmethod    
    def getWaypoints(latLong1, latLong2, step):
        waypoints = []
        dist = LatLong.getDistance(latLong1, latLong2)
        bearing = LatLong.getBearing(latLong1, latLong2)
        currDist, currLatLong = 0, latLong1
        while currDist < dist:
            currLatLong = LatLong.getLocation(currLatLong, step, bearing)
            waypoints.append(currLatLong)
            currDist += step
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
        
def plot(x, y, z):
    fig = plt.figure()
    ax = fig.gca(projection='3d')
    ax.plot(x, y, z, label='parametric curve')
    ax.legend()
    plt.show()

if __name__ == "__main__":
    data = open("locations.txt", "r").readlines()
    locations = [LatLong(location.rstrip().split(",")) for location in data]

    start = locations[0]
    goal = LatLong.getLocation(start, math.radians(90), 100)
    dist = LatLong.getDistance(start, goal)
    print start, goal, dist

    waypoints = LatLong.getWaypoints(start, goal, 10)
    print [str(waypoint) for waypoint in waypoints]

    cartesian = LatLong.getApproxCartesianWaypoints(waypoints, start)
    print cartesian

    plot([x[0] for x in cartesian], [x[1] for x in cartesian], [0 for x in cartesian])

# References
# http://www.movable-type.co.uk/scripts/latlong.html
# http://www.movable-type.co.uk/scripts/latlong.html
# http://www.gps-coordinates.net/gps-coordinates-converter
# https://en.wikipedia.org/wiki/Decimal_degrees
# http://stackoverflow.com/questions/1185408/converting-from-longitude-latitude-to-cartesian-coordinates
# http://www.latlong.net/place/seattle-wa-usa-2655.html
