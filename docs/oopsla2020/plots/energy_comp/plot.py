#!/usr/bin/python

import numpy as np
import copy
from numpy import *

import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

"""

Data Format in "data" file
Line 1 - x values
Line 1 - y values
Line 2 - x values
Line 2 - y values
...
Line N - x values
Line N - y values
...
"""

# define file names
DATAFILE="data" # the file to read data from
PLOT_FILE="energy_comp.png"
PDF_FILE="energy_comp.pdf"

# plot axis titles and labels
TITLE="Execution Model Comparison"
XLABEL='Graph Size (# Edges)'
YLABEL="Ops/Watt"
FONT_SIZE = 16

# set font and size
font = {'family' : 'serif', 'weight' : 'bold', 'size' : FONT_SIZE}
plt.rc('font', **font)

# define coloring, line type, and markers
PATTERN_PALETTE = [ "/" , "\\" , "|" , "-" , "+" , "x", "o", "O", ".", "*" ]
COLOR_PALETTE=['aqua', 'mediumspringgreen', 'orangered', 'blue', 'grey', 'magenta'] # use this color palette if you want different colors
LINE_PALETTE=['--', ':', '-.']
MARKER_PALETTE=['s','o','^','v', 's','o','^'] # change this marker palette if you need to

# set the figure size
fig_size = plt.rcParams["figure.figsize"]
fig_size[0] = 8
fig_size[1] = 4

fig, ax = plt.subplots()

LINES=0

# open the file
infile = open(DATAFILE, "r")

# scan the file for number of lines
TOTAL_LINES = 0
for line in infile:
    TOTAL_LINES += 1
infile.seek(0)

LINES=TOTAL_LINES/2

index = 0

# iterate over the file and get the data
MAX_Y = 0
MAX_X = 0
for i in range(0, LINES):

    # Extract the KD-Tree data and plot
    x_values_fields = infile.readline().split(",")
    y_values_fields = infile.readline().split(",")
    label = x_values_fields[0]
    x_values = map(float, x_values_fields[1:])
    y_values = map(float, y_values_fields[1:])

    # construct a list of the data
    tuples = [None]*len(x_values)
    for j in range(0, len(x_values)):
        tuples[j] = (y_values[j], x_values[j])

    # sort the data by the x value so that it plots properly
    tuples = sorted(tuples, key=lambda x: x[1])
    for j in range(0, len(tuples)):
        (y_values[j], x_values[j]) = tuples[j]

    # apply the plot
    plt.plot(x_values, y_values, LINE_PALETTE[i%len(LINE_PALETTE)], label=label, color=COLOR_PALETTE[i%len(COLOR_PALETTE)], marker=MARKER_PALETTE[i%len(MARKER_PALETTE)])

    if (max(y_values) > MAX_Y):
        MAX_Y = max(y_values)
    if (max(x_values) > MAX_X):
        MAX_X = max(x_values)

# set plot titles and axis
ax.set_title(TITLE)
ax.set_xlabel(XLABEL)
ax.set_ylabel(YLABEL)
ax.set_yscale('linear')
ax.set_xscale('linear')
ax.set_xlim([0, MAX_X+1])
ax.set_ylim([1, MAX_Y+2.0]) 

# add a legend and export to png
ax.legend(loc=1, prop={'size':10}, ncol=2)
fig.tight_layout()
fig.savefig(PLOT_FILE, dpi=300)

# export to pdf
pp = PdfPages(PDF_FILE)
pp.savefig(fig)
pp.close()
