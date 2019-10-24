#!/usr/bin/python

import numpy as np
import copy
from numpy import *

import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

'''
Data file format:
Platform, [X Axis Group 1], [X Axis Group 2], ... [X-Axis Group M]
[Bar Type 1], [Group 1, Bar 1], [Group 2, Bar 1], ... , [Group M, Bar 1]
[Bar Type 2], [Group 1, Bar 2], [Group 2, Bar 2], ... , [Group M, Bar 2]
...
[Bar Type N], [Group 1, Bar N], [Group 2, Bar N], ... , [Group M, Bar N]
'''

# define files
DATAFILE="data"
PLOT_FILE="direction-speedup.png"
PDF_FILE="direction-speedup.pdf"

# define title labels and axis labels
#TITLE="Linear Search Energy Efficiency"
#XLABEL=""
YLABEL="Speedup over SparsePush"

# define pattern and color palettes
PATTERN_PALETTE = [ "/" , "\\" , "|" , "-" , "+" , "x", "o", "O", ".", "*" ]
#COLOR_PALETTE=['b','r','c','y','p','o']
COLOR_PALETTE=['aqua', 'mediumspringgreen', 'orangered', 'lightblue', 'yellow', 'azure']

# define font size
FONT_SIZE = 12
font = {'family' : 'serif', 'weight' : 'bold', 'size' : FONT_SIZE}
plt.rc('font', **font)

# set the figure size
fig_size = plt.rcParams["figure.figsize"]
fig_size[0] = 8
fig_size[1] = 4

infile = open(DATAFILE, "r")

# truncate the first field which is padding
X_TITLES = infile.readline().split(',')[1:]
for h in range(0, len(X_TITLES)):
    X_TITLES[h] = X_TITLES[h].strip()
NUM_BAR_GROUPS = len(X_TITLES)
NUM_BAR_TYPES = 0

BAR_LABELS = []
PLOT_DATA = []

# scan the file for number of x-groups
eof = False
while not eof:
    line = infile.readline()
    if (line == ""):
        eof = True
        break
    
    fields = line.split(',')
    data = map(float, fields[1:])
    assert(len(data) == NUM_BAR_GROUPS)
    BAR_LABELS.append(fields[0])
    PLOT_DATA.append(data)
    NUM_BAR_TYPES += 1

print "[Info] Detected: " + str(NUM_BAR_GROUPS) + " distinct bar groups (x-axis groups)"
print "[Info] Detected: " + str(NUM_BAR_TYPES) + " distinct bar types (bars per x-axis group)"

infile.close()

fig, ax = plt.subplots()

# compute the plot specific data
COL_BASE = 0.1
COL_MAX = 0.9
BAR_WIDTH = (COL_MAX - COL_BASE) / NUM_BAR_TYPES
Y_OFFSET = 1

# actually plot the data
bar_left = arange(0, NUM_BAR_GROUPS) 
for i in range(0, NUM_BAR_TYPES):
    offset = (COL_BASE + i * BAR_WIDTH)

    rects = ax.bar(left = bar_left + offset, height = PLOT_DATA[i], width = BAR_WIDTH, color = COLOR_PALETTE[i], label = BAR_LABELS[i], hatch=PATTERN_PALETTE[i])

    # truncate significant figures appropriate and scale position of text to be above the bar in log scale
    for p in rects:
        h = p.get_height()
        if h > 10:
            ax.text(p.get_x()+p.get_width()/2., h * log(3), '%d'%int(h),
                     ha='center', va='bottom', rotation = 'vertical', size = 'x-small')
        elif h > 1:
            ax.text(p.get_x()+p.get_width()/2., h * log(3), '%0.1f'%float(h),
                     ha='center', va='bottom', rotation = 'vertical', size = 'x-small')
        else:
            ax.text(p.get_x()+p.get_width()/2., h * log(3), '%0.2f'%float(h),
                     ha='center', va='bottom', rotation = 'vertical', size = 'x-small')
            
# apply the labeling
#ax.set_title(TITLE)
#ax.set_xlabel(XLABEL)
ax.set_ylabel(YLABEL)
ax.set_yscale('linear')
ax.set_xticks(bar_left + 0.5)
ax.set_xticklabels(X_TITLES, rotation=20)
ax.set_ylim([0, amax(np.matrix(PLOT_DATA)) + 1])

# add legend
ax.legend(loc=2, prop={'size':10})

# export to png
fig.tight_layout()
fig.savefig(PLOT_FILE, dpi=300)

# export to pdf
pp = PdfPages(PDF_FILE)
pp.savefig(fig)
pp.close()
