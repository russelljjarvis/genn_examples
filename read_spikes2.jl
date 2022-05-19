
using DelimitedFiles

#open("7d933154b27ff70fc2df9ed8bec39480-spikes-00001-0.gdf") do file
    # do stuff with the open file
#end
#using PyCall
using Plots
using NeuroAnalysis
using Pkg
ENV["PYTHON"] = "/home/user/miniconda3/bin/python"           # example for *nix
Pkg.build("PyCall")
using PyCall

@time spike_mat = readdlm("7d933154b27ff70fc2df9ed8bec39480-spikes-00001-0.gdf", '\t', Any, '\n')
@time spike_mat = spike_mat[:,1:2]
@time spike_mat = spike_mat[1:1000000]
@show(spike_mat[1])
@time plotspiketrain(reduce(vcat, spike_mat))|>display


py"""
import pyspike
from pyspike import SpikeTrain
import numpy as np
#from quantities import ms, s, Hz
#from elephant.spike_train_generation import homogeneous_poisson_process, homogeneous_gamma_process
#spike_train = SpikeTrain(np.array([0.1, 0.3, 0.45, 0.6, 0.9], [0.0, 1.0]))
#print(spike_train)
def wrangle_spikes_trains(spike_trains):
    for sp in spike_trains:
        #import pdb
        #pdb.set_trace()
        print(sp)
    return spike_trains
"""
#spike_trains = py"wrangle_spikes_trains"(spike_mat)

py"""
import pyspike
#from quantities import ms, s, Hz
#from elephant.spike_train_generation import homogeneous_poisson_process, homogeneous_gamma_process
spike_train = SpikeTrain(np.array([0.1, 0.3, 0.45, 0.6, 0.9], [0.0, 1.0]))

def get_spikes_trains(spike_trains):
    #spike_trains = spk.load_spike_trains_from_txt("PySpike_testdata.txt", 4000)

    plt.figure()
    isi_distance = spk.isi_distance_matrix(spike_trains)
    plt.imshow(isi_distance, interpolation='none')
    plt.title("ISI-distance")

    plt.figure()
    spike_distance = spk.spike_distance_matrix(spike_trains, interval=(0,1000))
    plt.imshow(spike_distance, interpolation='none')
    plt.title("SPIKE-distance")

    plt.figure()
    spike_sync = spk.spike_sync_matrix(spike_trains, interval=(2000,4000))
    plt.imshow(spike_sync, interpolation='none')
    plt.title("SPIKE-Sync")

    plt.show()

"""

#plotspiketrain(spike_mat)|>display

#spike_mat = spike_mat[:,1::]
#@show(size(spike_mat))
#@show(sizeof(spike_mat))
#for col in eachcol(spike_mat)
#       println(col)
#end

#m = Matrix{Int}(undef, 1012741, 2)
#for j in 1:2
#m = hcat([col for col in eachcol(spike_mat[:,1:2])])
#@show(m[1])
    #m[:, j] = column(spike_mat)
#end

#for col in eachcol(spike_mat[1:5,:])
#       println(col[:])
#end

#for col in eachrow(spike_mat[1:5,:])
#       println(col[:])
#end

#spike_mat[1,:]
#spike_mat[2,:]
#for col in
#=
totaltime, totallines = open("7d933154b27ff70fc2df9ed8bec39480-spikes-00001-0.gdf") do f
    #linecounter = 0
    #timetaken = @elapsed
    for l in eachline(f)

        #@show(l)
        #linecounter += 1
    end
    #(timetaken, linecounter)
end
=#
