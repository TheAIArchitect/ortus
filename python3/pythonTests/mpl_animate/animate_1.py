import numpy as np
from matplotlib.patches import Circle
from matplotlib.collections import PatchCollection
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib import animation

# from:
# http://stackoverflow.com/questions/23915859/how-to-efficiently-alter-the-color-in-an-animation-of-patches-circles-in-matplot

fig, ax = plt.subplots()

patches = []
# create circles with random sizes and locations
N = 10 # number of circles
x = np.random.rand(N)
y = np.random.rand(N)
radii  = 0.1*np.random.rand(N)
for x1,y1,r in zip(x, y, radii):
    circle = Circle((x1,y1), r)
    patches.append(circle)

# add these circles to a collection
p = PatchCollection(patches, cmap=cm.prism, alpha=0.4)
ax.add_collection(p)

def animate(i):
    colors = 100*np.random.rand(len(patches)) # random index to color map
    p.set_array(np.array(colors)) # set new color colors
    return p,

ani = animation.FuncAnimation(fig, animate, frames=50, interval=50)

plt.show()
