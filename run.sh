#!/bin/bash
#SBATCH --job-name=test_cpu
#SBATCH --output=log.out
#SBATCH --error=log.err
#SBATCH --mail-type=ALL
#SBATCH --mail-user=lixiao37mail@gmail.com
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --mem=4000mb
#SBATCH --time=24:00:00

echo "Date       = $(date)"
echo "Host       = $(hostname -s)"
echo "Directory  = $(pwd)"

module purge
module load pytorch/1.8.1

T1=$(date +%s)
python test_cpus.py

T2=$(date +%s)

ELAPSED=$((T2 - T1))
echo "Elapsed Time = $ELAPSED seconds"
