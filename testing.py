import os
import glob
import h5py
import numpy as np
import matplotlib.pyplot as plt
import sys
from matplotlib import ticker, colors
from mpl_toolkits.mplot3d import axes3d
from matplotlib import gridspec
from scipy.interpolate import RegularGridInterpolator
#from scipy.integrate import simps, trapz

def plotStreamline(fName, input_dir , base_name="streamline"):
    """
    Plots streamlines from an HDF5 file and saves them with unique names.

    Args:
        fName (str): Path to the HDF5 file containing velocity data.
        base_name (str, optional): Base name for the output filenames. Defaults to "streamline".
    """

    # Read velocity data
    with h5py.File(fName, "r") as f:
        U = f["Vx"][:]
        # V = f["Vy"][:]  # Uncomment if you need vertical velocity
        W = f["Vz"][:]
        # T = f["T"][:]  # Uncomment if you need temperature

    [Nx, Nz] = U.shape

    # Generate coordinates
    x = np.linspace(0, 1, Nx)
    z = np.linspace(0, 1, Nz)
    X, Z = np.meshgrid(x, z)

    # Create plot
    plt.figure()
    stream_obj = plt.streamplot(X, Z, U, W, density=1)

    # Get unique filename with timestamp
    unique_filename = os.path.join(input_dir, os.path.basename(fName).replace('.h5', '_waterFlow.png'))
    # Save plot and close figure
    plt.savefig(unique_filename, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"Saved image to {unique_filename}.")



 

def main(input_dir):

    # Check if the input directory exists
    if not os.path.isdir(input_dir):
        print(f"Error: The directory {input_dir} does not exist.")
        return

 

    # Get all .h5 files in the input directory
    h5_files = glob.glob(os.path.join(input_dir, "*.h5"))

    if not h5_files:
        print(f"No .h5 files found in the directory {input_dir}.")

        return

 

    for h5_file in h5_files:

        try:
            # Open the HDF5 file
            with h5py.File(h5_file, 'r') as f:
                # Check if required variables exist
                if 'P' not in f or 'Vx' not in f or 'Vz' not in f:
                    print(f"One or more required variables ('P', 'Vx', 'Vz') not found in file {h5_file}. Skipping.")
                    continue

 

                # Read the 'P' variable
                P = f['P'][:]
<<<<<<< HEAD
                
=======

>>>>>>> origin/main
 
                plotStreamline(h5_file, input_dir)
                # Determine the size of the variable
                size = P.shape
                print(f"Processing file {h5_file} with variable 'P' of size {size}.")

 

                # Generate an image for 'P'
                plt.imshow(P, cmap='cool')
                plt.colorbar(label='P')

                plt.title(f"Variable 'P' from {os.path.basename(h5_file)}")

 

                # Create output filename for 'P'
                output_file_P = os.path.join(input_dir, os.path.basename(h5_file).replace('.h5', '_P.png'))

 
                # Save the image for 'P'
                plt.savefig(output_file_P, dpi=300, bbox_inches='tight')
                plt.close()

                print(f"Saved image to {output_file_P}.")

                # Read the 'Vx' and 'Vz' variables

                Vx = f['Vx'][:]

                Vz = f['Vz'][:]

 
                # Compute the magnitude of the velocity

                velocity_magnitude = np.sqrt(Vx**2 + Vz**2)

                # Generate an image for velocity magnitude

                plt.imshow(velocity_magnitude, cmap='gist_ncar')
                plt.colorbar(label='Velocity Magnitude')
                plt.title(f"Velocity Magnitude from {os.path.basename(h5_file)}")


                # Create output filename for velocity magnitude
                output_file_velocity = os.path.join(input_dir, os.path.basename(h5_file).replace('.h5', '_VelocityMag.png'))
                # Save the image for velocity magnitude

                plt.savefig(output_file_velocity, dpi=300, bbox_inches='tight')

                plt.close()
                print(f"Saved image to {output_file_velocity}.")

 

        except Exception as e:

            print(f"Error processing file {h5_file}: {e}")

 

if __name__ == "__main__":

    if len(sys.argv) != 2:

        print("Usage: python script.py %s" % sys.argv[0])

    else:

        input_dir = sys.argv[1]

        main(input_dir)
