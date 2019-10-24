#!/usr/bin/python

import numpy as np
import copy

import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

"""
Data file format: data
Anything, [Sub Bar 1], [Sub Bar 2], ... [Sub Bar N]
X Label 1, [Bar 1, Label 1], [Bar 2, Label 1] ... [Bar N, Label 1]
X Label 2, [Bar 1, Label 2], [Bar 2, Label 2] ... [Bar N, Label 2]
...
X Label M, [Bar 1, Label M], [Bar 2, Label M] ... [Bar N, Label M]
"""

# define files
DATAFILE = "data"
PLOT_FILE = "graph-stalls.png"
PDF_FILE = "graph-stalls.pdf"

# plot title and axis labels
TITLE = "Stalls by Type"
XLABEL="Execution Model"
YLABEL="Number of Stall Cycles"
BAR_WIDTH = 0.5

# palette definitions for bars
PATTERN_PALETTE = [ "/" , "\\" , "|", "x" , ".", "o", "O", "*" ]
#COLOR_PALETTE=['b','r','c','y','p','o']
COLOR_PALETTE=['aqua', 'mediumspringgreen', 'orangered', 'lightblue', 'yellow', 'azure']

# set font size
FONT_SIZE = 12
font = {'family' : 'serif', 'weight' : 'bold', 'size' : FONT_SIZE}
plt.rc('font', **font)

# set figure size
fig_size = plt.rcParams["figure.figsize"]
fig_size[0] = 8
fig_size[1] = 4

infile = open(DATAFILE, "r")

# scan the file to determine number of lines
DATA_LINES = 0
LINE_FIELDS = 0
for line in infile:
    DATA_LINES += 1
    LINE_FIELDS = len(line.split(",")) - 1
infile.seek(0)

# first line is always the header
DATASET = np.zeros((DATA_LINES-1, LINE_FIELDS))
PLOT_HEADER = []

# parse the file
line_count = 0
for line in infile:
    fields = line.split(",")

    # first line is always the header
    if (line_count == 0):
        DATA_LABELS = line.split(",")[1:]
        pass
    else:
        # strip x-axis label 
        PLOT_HEADER.append(fields[0])

        # get the data values
        for i in range(1, len(fields)):
            DATASET[line_count-1, i-1] = float(fields[i])

    line_count += 1

# normalize the dataset values
#DATASET = DATASET / 1000000

assert(len(PLOT_HEADER) == DATA_LINES-1)

# set the width of the stacked bars
BAR_WIDTH = 0.5

# calculate the base offsets of the bars
XBASE=range(0, len(PLOT_HEADER))
for i in range(0, len(XBASE)):
    XBASE[i] += (1 - BAR_WIDTH)/2
YBASE=[0] * len(PLOT_HEADER)

fig, ax = plt.subplots()

# actually plot the bars
for i in range(0, len(DATA_LABELS)):
    data_vector = DATASET[:,i]
    header = DATA_LABELS[i].strip()

    plt.bar(XBASE, data_vector, bottom = YBASE, label=header, width=BAR_WIDTH, color=COLOR_PALETTE[i % len(COLOR_PALETTE)], hatch = PATTERN_PALETTE[i%len(PATTERN_PALETTE)])

    for j in range(0, len(PLOT_HEADER)):
        YBASE[j] += data_vector[j]

# write the total value at the top of each bar
for i in range(0, len(PLOT_HEADER)):
    ax.text((XBASE[i] + XBASE[i]+BAR_WIDTH)/2, YBASE[i], '%0.2f'%float(YBASE[i]),
            ha='center', va='bottom', rotation='horizontal', size='small')

# Add labels and legend
xtick_positions=range(0, len(PLOT_HEADER))
for i in range(0, len(PLOT_HEADER)):
    xtick_positions[i] += 0.5
plt.xticks(xtick_positions, PLOT_HEADER)
#plt.xlabel(XLABEL)
plt.ylabel(YLABEL)
plt.legend(loc=2, prop={'size':8})
#plt.title(TITLE)

# export to png
plt.savefig(PLOT_FILE)
infile.close()

# export to pdf
pp = PdfPages(PDF_FILE)
pp.savefig(fig)
pp.close()

