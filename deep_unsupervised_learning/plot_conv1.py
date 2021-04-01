import matplotlib.pyplot as plt
import numpy as np

from glob import glob
from mpl_toolkits.mplot3d import Axes3D

WIDTH = 24
HEIGHT = 24
NUM_CHANNELS = 16

# Load spikes
conv_spikes = np.loadtxt("conv1_spikes_0.csv", skiprows=1, delimiter=",")

# Convert neuron IDs to row, col and channel
conv_spike_row = (conv_spikes[:,1] // NUM_CHANNELS) // WIDTH
conv_spike_col = (conv_spikes[:,1] // NUM_CHANNELS) % WIDTH
conv_spike_chan = conv_spikes[:,1] % NUM_CHANNELS

# Create 3D plot
fig = plt.figure()
axis = fig.add_subplot(projection="3d")


axis.scatter(conv_spike_col, conv_spikes[:,0], conv_spike_row, c=conv_spike_chan)
plt.show()
    