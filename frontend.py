import tkinter as tk
from tkinter import ttk
import os
import re

MARGIN=10
BAR_WIDTH=1024
BAR_HEIGHT=50

CANVAS_WIDTH=(2*MARGIN)+BAR_WIDTH
CANVAS_HEIGHT=(2*MARGIN)+BAR_HEIGHT
WINDOW_WIDTH=CANVAS_WIDTH
WINDOW_HEIGHT=400

COLORS = ['#64ac61', '#cbbd82', '#a97070', '#56556d', '#bdaaaa']

def parse_file_info(info_string):
    pattern = r"([0-9A-Fa-f]+)\s+(\d+)\s+(\S+)"
    match = re.match(pattern, info_string)
    if match:
        file_offset = int(match.group(1), 16)
        file_size = int(match.group(2))
        file_name = match.group(3)
        print(file_offset, file_size, file_name)
        return file_offset, file_size, file_name
    else:
        raise ValueError("Input string format is invalid.")

def parse_capacity(info_string):
    parts = info_string.strip().rsplit(" ", 1)
    if len(parts) < 2:
        raise ValueError("Input string format is invalid.")
    return int(parts[1])

def map_scale(value, from_min, from_max, to_min, to_max):
    return (to_max - to_min) / (from_max - from_min) * (value - from_min) + to_min 
    
# Create the main window
root = tk.Tk()
root.title("vEPROM Frontend")
root.configure(background='white')
root.geometry(f"{WINDOW_WIDTH}x{WINDOW_HEIGHT}")

canvas = tk.Canvas(root, width=CANVAS_WIDTH, height=CANVAS_HEIGHT)
canvas.pack()

label = tk.Label(root, text="Files:", bg='white')
label.pack(pady=5, padx=10, anchor="w")

separator = tk.Frame(root, height=1, bd=1, relief="solid")
separator.pack(fill="x", pady=(0, 5))

# Display canvas background
a = canvas.create_rectangle(0, 0, BAR_WIDTH, BAR_HEIGHT, fill='#888888', width=0)
canvas.move(a, MARGIN, MARGIN)

# Get file list from vEPROM emulator
i = 0
with os.popen("./build/veprom list", 'r') as veprom_output:

    # Get the total capacity of the vEPROM from the list output
    capacity = parse_capacity(veprom_output.readline().strip())
    
    # Loop through the files and show the area take by the file
    for line in veprom_output:
        file_offset, file_size, file_name = parse_file_info(line)

        # Map memory address to canvas
        bar_start = map_scale(file_offset, 0, capacity, 0, BAR_WIDTH)
        bar_size = map_scale(file_size, 0, capacity, 0, BAR_WIDTH)

        # Display file usage
        a = canvas.create_rectangle(0, 0, bar_size, BAR_HEIGHT, fill=COLORS[i%len(COLORS)], width=0)
        canvas.move(a, bar_start+MARGIN, MARGIN)

        # Display filename
        label = tk.Label(root, text=f"{file_name} ({file_size} bytes)", fg=COLORS[i%len(COLORS)], bg='white')
        label.pack(pady=5, padx=25, anchor="w")

        separator = tk.Frame(root, height=1, bd=1, relief="solid")
        separator.pack(fill="x", pady=(0, 5))

        # Increment loop counter to cycle through colors
        i = i + 1

# Run the application
root.mainloop()
