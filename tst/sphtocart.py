import math

# Conversion borrowed from here: https://stackoverflow.com/a/1185413/13413114
# after several attempts converting spherical coords, which
# appear to measure the polar angle from the poles (which makes sense)
# rather than from the equator.




lat1 = -0.145
lon1 = -1.025
rad1 = 97.722


lat2 = -0.166
lon2 = -1.296
rad2 = 97.722

x1 = rad1 * math.cos(lat1) * math.cos(lon1)
y1 = rad1 * math.cos(lat1) * math.sin(lon1)
z1 = rad1 * math.sin(lat1)

x2 = rad2 * math.cos(lat2) * math.cos(lon2)
y2 = rad2 * math.cos(lat2) * math.sin(lon2)
z2 = rad2 * math.sin(lat2)

dist = math.sqrt(
    (x2-x1)**2 + (y2-y1)**2 + (z2-z1)**2
)


print("1", x1, y1, z1)
print("2", x2, y2, z2)
print("D",dist)