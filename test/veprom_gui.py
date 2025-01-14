import subprocess
import tkinter as tk
from tkinter import messagebox
from tkinter import filedialog

class EPROMViewer:
    def __init__(self, root):
        self.root = root
        self.root.title("Virtual EPROM Viewer")

        # GUI Components
        self.file_list_label = tk.Label(root, text="Stored Files")
        self.file_list_label.pack(pady=5)

        self.file_listbox = tk.Listbox(root, width=50, height=15)
        self.file_listbox.pack(pady=5)

        self.refresh_button = tk.Button(root, text="Refresh", command=self.refresh_file_list)
        self.refresh_button.pack(pady=10)

        # Initial file list population
        self.refresh_file_list()

    def refresh_file_list(self):
        try:
            # Execute the "veprom list" command
            list_result = subprocess.run(["./veprom", "list"], capture_output=True, text=True)
            if list_result.returncode != 0:
                messagebox.showerror("Error", f"Failed to list files: {list_result.stderr.strip()}")
                return

            # Display files in the listbox
            self.file_listbox.delete(0, tk.END)
            for line in list_result.stdout.splitlines():
                self.file_listbox.insert(tk.END, line)

        except Exception as e:
            messagebox.showerror("Error", f"An error occurred: {str(e)}")

if __name__ == "__main__":
    root = tk.Tk()
    app = EPROMViewer(root)
    root.mainloop()