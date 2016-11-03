# Waypoint Generation

# Introduction

This module implements logic for generating intermediate  waypoints for the drone, given a set of initial lcoations of interest.

# Parameters

The waypoint generation is controlled by the following parameters as used in the config file

* global_wp::d_step Distance between two intermediate waypoints
* local_wp::ini_rad Initial radius for local pattern
* local_wp::ha_step Angle between intermediate waypoints in local circular pattern
* local_wp::va_step Incremental increase in radius for concentric local search
* local_wp::elev_h Max elevation for local search
* local_wp::elev_l Min elevation for local search
* local_wpelev_step Incremental elevation change between intermediate waypoints between locations of interest

# Sample Output

![alt tag](https://github.com/karamach/rescue_rangers/tree/master/mrsd_project/waypoint_gen/images/path.png)

# TODO
1. Employ shortest part

# References
- http://www.movable-type.co.uk/scripts/latlong.html
- https://en.wikipedia.org/wiki/Decimal_degrees
- http://stackoverflow.com/questions/1185408/converting-from-longitude-latitude-to-cartesian-coordinates
- http://www.latlong.net/place/seattle-wa-usa-2655.html
- http://www.gps-coordinates.net/gps-coordinates-converter
- http://www.darrinward.com/lat-long/?id=2365962
- http://www.gpsvisualizer.com/map_input?form=data
- http://www.hamstermap.com/quickmap.php
