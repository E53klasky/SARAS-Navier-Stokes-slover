import numpy as np
import h5py
import matplotlib.pyplot as plt
import sys

def hdf5_reader(filepath, dataset):
    """Read a dataset from an HDF5 file."""
    with h5py.File(filepath, 'r') as f:
        data = f[dataset][:]
    return data

def calculate_difference(high_res_data, low_res_data, skip_factor):
    """
    Calculate the absolute difference between high-resolution and low-resolution data.
    Use the skip factor to align the low-resolution data with the high-resolution grid.
    """
    diff = np.zeros_like(low_res_data)
    for i in range(low_res_data.shape[0]):
        for j in range(low_res_data.shape[1]):
            high_res_value = high_res_data[i * skip_factor, j * skip_factor]
            low_res_value = low_res_data[i, j]
            diff[i, j] = np.abs(high_res_value - low_res_value)
    return diff

def plot_difference(diff, output_path):
    """Plot the 2D difference array and save it as an image."""
    plt.figure(figsize=(8, 6))
    plt.imshow(diff, cmap='gnuplot2', origin='lower')
    plt.colorbar(label='Difference')
    plt.title("Difference Between Resolutions")
    plt.xlabel("X-axis")
    plt.ylabel("Y-axis")
    plt.savefig(output_path)
    plt.show()

def calculate_and_plot_difference(filepath1, filepath2, dataset, output_path):
    """Calculate and plot the difference between high-res and low-res datasets."""
    try:

        high_res_data = hdf5_reader(filepath1, dataset)
        low_res_data = hdf5_reader(filepath2, dataset)

        if (high_res_data.shape[0] - 1) % (low_res_data.shape[0] - 1) != 0 or \
           (high_res_data.shape[1] - 1) % (low_res_data.shape[1] - 1) != 0:
            print("Error: High-resolution and low-resolution shapes are not compatible.")
            sys.exit(1)
        
  
        skip_factor_x = (high_res_data.shape[0] - 1) // (low_res_data.shape[0] - 1)
        skip_factor_y = (high_res_data.shape[1] - 1) // (low_res_data.shape[1] - 1)
        if skip_factor_x != skip_factor_y:
            print("Error: Non-uniform skip factors detected. Ensure grid compatibility.")
            sys.exit(1)
        

        skip_factor = skip_factor_x

        diff = calculate_difference(high_res_data, low_res_data, skip_factor)

 
        plot_difference(diff, output_path)
    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)

def main():
    """Main function to handle user input and process filepaths."""
    if len(sys.argv) != 5:
        print("Usage: python script.py <high_res_filepath> <low_res_filepath> <dataset_name> <output_file>")
        sys.exit(1)

    high_res_filepath = sys.argv[1]
    low_res_filepath = sys.argv[2]
    dataset_name = sys.argv[3]
    output_file = sys.argv[4]

    # Calculate and plot the difference
    calculate_and_plot_difference(high_res_filepath, low_res_filepath, dataset_name, output_file)

if __name__ == "__main__":
    main()
