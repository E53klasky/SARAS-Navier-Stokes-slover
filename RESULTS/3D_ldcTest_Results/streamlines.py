import h5py
import numpy as np
import matplotlib.pyplot as plt
import os
import sys

directory = "./128"
file_pattern = "Soln_{:04d}.0000.h5"  

def read_hdf5_velocity(file_path):
    with h5py.File(file_path, 'r') as f:
        Vx = f['/Vx'][:] 
        Vy = f['/Vy'][:] 
        Vz = f['/Vz'][:]  

    return Vx, Vy, Vz

# Function to plot streamlines for a given timestep
def plot_streamlines(timestep, var_1, var_2, slice_idx):
    file_path = os.path.join(directory, file_pattern.format(timestep))
    
    if not os.path.exists(file_path):
        print(f"Warning: File {file_path} not found! Skipping timestep {timestep}.")
        return
    
    Vx, Vy, Vz = read_hdf5_velocity(file_path)

    var_1 = var_1.lower()
    var_2 = var_2.lower()

    if var_1 == "vx":
        u = Vx[:, :, slice_idx]
    elif var_1 == "vy":
        u = Vy[:, :, slice_idx]
    else:
        u = Vz[:, :, slice_idx]

    if var_2 == "vx":
        v = Vx[:, :, slice_idx]
    elif var_2 == "vy":
        v = Vy[:, :, slice_idx]
    else:
        v = Vz[:, :, slice_idx]

    x, y = np.meshgrid(np.linspace(0, 1, Vx.shape[1]), np.linspace(0, 1, Vx.shape[0]))


    plt.figure(figsize=(8, 6))
    plt.streamplot(x, y, u, v, color=np.sqrt(u**2 + v**2), cmap='jet')
    plt.xlabel(var_1.upper())
    plt.ylabel(var_2.upper())
    plt.title(f"Streamlines at t={timestep}, Slice {slice_idx}")
    plt.colorbar(label="Velocity magnitude")
    plt.savefig(f"streamlines_{timestep:04d}.png", dpi=300)
    plt.close()

# Process multiple timesteps
timesteps = [0, 10, 20, 30, 40, 50]  # Adjust based on available files

# Command-line arguments
if len(sys.argv) < 4:
    print("Usage: python streamlines.py <U axis> <V axis> <slice_idx>")
    sys.exit(1)

var_1 = sys.argv[1]
var_2 = sys.argv[2]
slice_idx = int(sys.argv[3])

for t in timesteps:
    plot_streamlines(t, var_1, var_2, slice_idx)

print("Streamline images saved!")
