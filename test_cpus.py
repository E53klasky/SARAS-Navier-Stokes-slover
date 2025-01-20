import os
import multiprocessing

# Total number of CPU cores
total_cpu_cores = os.cpu_count()

# Number of usable CPU cores
usable_cpu_cores = len(os.sched_getaffinity(0)) if hasattr(os, "sched_getaffinity") else multiprocessing.cpu_count()

print(f"Total CPU cores: {total_cpu_cores}")
print(f"Usable CPU cores: {usable_cpu_cores}")