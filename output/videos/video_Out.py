import cv2
import os
import numpy as np

# Get a list of all .png files in the current directory
image_files = [f for f in os.listdir() if f.endswith('.png')]

# Sort the images by name (optional, to ensure they're in order)
image_files.sort()

# Check if there are any .png files in the directory
if len(image_files) == 0:
    print("No .png files found in the directory.")
    exit()

# Read the first image to get the width and height
first_image = cv2.imread(image_files[0])
height, width, layers = first_image.shape

# Define the video writer object (output video file, codec, FPS, and frame size)
fourcc = cv2.VideoWriter_fourcc(*'mp4v')  # Codec for .mp4 video
fps = 30  # Frames per second
out = cv2.VideoWriter('output_video.mp4', fourcc, fps, (width, height))

# Loop through each image and write it to the video file
for image_file in image_files:
    image = cv2.imread(image_file)
    out.write(image)  # Write the image as a frame

# Release the video writer object and close
out.release()
cv2.destroyAllWindows()

print("Video has been created successfully!")