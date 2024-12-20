import os

import glob

import h5py

import numpy as np

import matplotlib.pyplot as plt

import sys

 

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

 

                # Determine the size of the variable

                size = P.shape

                print(f"Processing file {h5_file} with variable 'P' of size {size}.")

 

                # Generate an image for 'P'

                plt.imshow(P, cmap='viridis')

                plt.colorbar(label='P')

                plt.title(f"Variable 'P' from {os.path.basename(h5_file)}")

 

                # Create output filename for 'P'

                output_file_P = os.path.join(input_dir, os.path.basename(h5_file).replace('.h5', '_P.png'))

 

                # Save the image for 'P'

                plt.savefig(output_file_P, dpi=300, bbox_inches='tight')

                plt.close()

                print(f"Saved image to {output_file_P}.")

 

                # Read the 'VX' and 'VY' variables

                VX = f['Vx'][:]

                VY = f['Vz'][:]

 

                # Compute the magnitude of the velocity

                velocity_magnitude = np.sqrt(VX**2 + VY**2)

 

                # Generate an image for velocity magnitude

                plt.imshow(velocity_magnitude, cmap='plasma')

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