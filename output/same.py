import h5py
import adios2
import numpy as np
import argparse
FLOAT_THRESHOLD = 1e-7
DOUBLE_THRESHOLD = 1e-15


# error 10e-7 float and error 10e-15 double
def read_hdf5_timestep(file_path, variable):
    """Read data from HDF5 file."""
    with h5py.File(file_path, 'r') as f:
        if variable in f:
            return f[variable][...]
        raise ValueError(f"Variable '{variable}' not found in HDF5 file: {file_path}")

def read_adios_bp(file_path, variable):
    """Read data from ADIOS BP file."""
    fr = adios2.FileReader(file_path)
    data = fr.read(variable)
    return data

# def read_adios_bp(file_path, variable):
    

def compare_and_subtract(hdf5_data, adios_data):
    """Compare and subtract the datasets."""
    if hdf5_data.shape != adios_data.shape:
        print("WARNING: The shapes of the data do not match:")
        print(f"HDF5 shape: {hdf5_data.shape}")
        print(f"ADIOS BP shape: {adios_data.shape}")
        return None
    
    # Perform subtraction
    difference = hdf5_data - adios_data
    
    # Calculate statistics
    max_diff = np.max(np.abs(difference))
    mean_diff = np.mean(difference)
    std_diff = np.std(difference)

      # Evaluate precision
    if max_diff < FLOAT_THRESHOLD:
        print("The data match within float precision (threshold: 1e-7).")
        if max_diff < DOUBLE_THRESHOLD:
            print("The data match within double precision (threshold: 1e-15).")
    else:
        print("The data do not meet the float or double precision thresholds.")

    
    return {
        'difference': difference,
        'max_difference': max_diff,
        'mean_difference': mean_diff,
        'std_difference': std_diff
    }

def main():
    parser = argparse.ArgumentParser(description="Compare and subtract variables between HDF5 and ADIOS BP files.")
    parser.add_argument("--hdf5", required=True, help="Path to the HDF5 file.")
    parser.add_argument("--adios", required=True, help="Path to the ADIOS BP file.")
    parser.add_argument("--hdf5-var", default="IntArray", help="Variable name in HDF5 file (default: IntArray)")
    parser.add_argument("--adios-var", default="data", help="Variable name in ADIOS file (default: data)")
    parser.add_argument("--output", help="Output file to save difference (optional)")
    parser.add_argument("-v", "--verbose", action="store_true", help="Print verbose output")

    args = parser.parse_args()

    try:
        # Read the variables from both files
        print(f"Reading {args.hdf5_var} from {args.hdf5}")
        hdf5_data = read_hdf5_timestep(args.hdf5, args.hdf5_var)
        print(f"Reading {args.adios_var} from {args.adios}")
        adios_data = read_adios_bp(args.adios, args.adios_var)

        print("\nData shapes:")
        print(f"HDF5 data shape: {hdf5_data.shape}")
        print(f"ADIOS data shape: {adios_data.shape}")

        if args.verbose:
            print("\nHDF5 data:")
            print(hdf5_data)
            print("\nADIOS data:")
            print(adios_data)

        # Compare and subtract the data
        result = compare_and_subtract(hdf5_data, adios_data)
        
        if result is not None:
            print("\nComparison Results:")
            print(f"Maximum absolute difference: {result['max_difference']}")
            print(f"Mean difference: {result['mean_difference']}")
            print(f"Standard deviation of difference: {result['std_difference']}")
            
            if args.verbose:
                print("\nDifference array:")
                print(result['difference'])
            
            # Save difference to file if requested
            if args.output:
                with h5py.File(args.output, 'w') as f:
                    f.create_dataset('difference', data=result['difference'])
                    f.create_dataset('max_difference', data=result['max_difference'])
                    f.create_dataset('mean_difference', data=result['mean_difference'])
                    f.create_dataset('std_difference', data=result['std_difference'])
                print(f"\nDifference data saved to: {args.output}")

    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()