import tkinter as tk
from tkinter import ttk, messagebox
import json
import os

def create_control_for_value(parent, value):
    """Create the appropriate control for a given value."""
    if isinstance(value, bool):
        var = tk.BooleanVar(value=value)
        control = tk.Checkbutton(parent, variable=var)
        return control, var  # Return both control and variable reference
    elif isinstance(value, (int, float)):
        var = tk.DoubleVar(value=value)  # Use DoubleVar to support both int and float
        control = tk.Spinbox(parent, from_=0, to=10000, textvariable=var)  # Adjust range as needed
        return control, var
    elif isinstance(value, str):
        var = tk.StringVar(value=value)
        control = tk.Entry(parent, textvariable=var)
        return control, var
    else:
        raise ValueError("Unsupported value type")

class ModManagerApp:
    def __init__(self):
        self.data = self.load_json("shroudtopia.json")
        self.root = tk.Tk()
        self.root.title("Shroudtopia GUI")
        self.create_widgets()

    def load_json(self, filename):
        if os.path.exists(filename):
            with open(filename, 'r') as file:
                return json.load(file)
        else:
            return {
                "active": True,
                "bootDelay": 3000,
                "enableLogging": True,
                "logLevel": "VERBOSE",
                "mods": {},
                "updateDelay": 500
            }

    def save_json(self, filename):
        with open(filename, 'w') as file:
            json.dump(self.data, file, indent=4)

    def create_widgets(self):
        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill='both', expand=True, padx=10, pady=10)  # Add padding to the notebook

        self.mod_tab = ttk.Frame(self.notebook)
        self.root_config_tab = ttk.Frame(self.notebook)

        self.notebook.add(self.mod_tab, text="Mods")
        self.notebook.add(self.root_config_tab, text="Root Configuration")

        # Populate the mod tab using pack
        self.mod_list = tk.Listbox(self.mod_tab)
        self.mod_list.pack(fill='both', expand=True, padx=5, pady=5)  # Use pack instead of grid
        self.mod_list.bind('<Double-1>', self.open_mod_editor)

        # Add button to create new mod with padding, using pack
        add_mod_button = tk.Button(self.mod_tab, text="Add Mod", command=self.add_mod)
        add_mod_button.pack(pady=5)  # Use pack instead of grid

        self.populate_mods()

        # Populate the root configuration tab
        self.populate_root_config()

        # Configure the grid weights for resizing in the root config tab
        self.root_config_tab.grid_rowconfigure(0, weight=1)  # Allow the config area to expand

        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)



    def populate_mods(self):
        self.mod_list.delete(0, tk.END)
        for mod in self.data.get("mods", {}).keys():
            self.mod_list.insert(tk.END, mod)

    def add_mod(self):
        mod_name = f"NewMod{len(self.data['mods']) + 1}"
        self.data['mods'][mod_name] = {"active": False}
        self.save_json("shroudtopia.json")
        self.populate_mods()

    def open_mod_editor(self, event):
        selection = self.mod_list.curselection()
        if selection:
            mod_name = self.mod_list.get(selection[0])
            mod_data = self.data["mods"][mod_name]
            ModEditor(self, mod_name, mod_data)

    def populate_root_config(self):
        # Clear the previous widgets
        for widget in self.root_config_tab.winfo_children():
            widget.destroy()

        # Configure the grid
        self.root_config_tab.grid_columnconfigure(1, weight=3)

        # Iterate over the root configuration items
        for index, (key, value) in enumerate(self.data.items()):
            if key != "mods":  # Skip mods for now
                label = tk.Label(self.root_config_tab, text=key)
                label.grid(row=index, column=0, sticky='e', padx=5, pady=5)

                # Create the appropriate control based on the value type
                entry, var = create_control_for_value(self.root_config_tab, value)

                # Set the value in the control
                if isinstance(var, tk.BooleanVar):
                    var.set(value)  # Populate BooleanVar with the value from JSON
                elif isinstance(var, tk.StringVar):
                    var.set(str(value))  # Populate StringVar with the value from JSON
                elif isinstance(var, tk.DoubleVar):
                    var.set(float(value))  # Populate DoubleVar for numbers

                entry.grid(row=index, column=1, sticky='ew', padx=5, pady=5)

                # Bind the control to update the data when the value changes
                if isinstance(var, (tk.StringVar, tk.BooleanVar, tk.DoubleVar)):
                    var.trace_add("write", lambda *args, k=key, v=var: self.on_root_config_change(k, v))


    def on_root_config_change(self, key, var):
        """Update root config value based on the control change."""
        new_value = var.get()  # Get the updated value from the tkinter variable

        # Typecast the value if necessary
        if isinstance(self.data[key], bool):
            new_value = bool(new_value)
        elif isinstance(self.data[key], (int, float)):
            new_value = float(new_value) if '.' in str(new_value) else int(new_value)

        # Update the JSON data with the new value
        self.data[key] = new_value

        # Save the updated JSON data to file
        self.save_json("shroudtopia.json")

    def on_closing(self):
        self.save_json("shroudtopia.json")
        self.root.destroy()

class ModEditor:
    def __init__(self, parent, mod_name, mod_data):
        self.parent = parent
        self.mod_name = mod_name
        self.mod_data = mod_data
        self.editor_window = tk.Toplevel(parent.root)
        self.editor_window.title(f"Editing Mod: {mod_name}")
        self.create_widgets()

    def create_widgets(self):
        self.key_entries = {}
        self.value_entries = {}
        self.pair_frames = []  # To keep track of the frames for key-value pairs

        for key, value in self.mod_data.items():
            self.add_key_value_pair(key, value)

        # Button to remove the mod
        remove_mod_button = tk.Button(self.editor_window, text="Remove Mod", command=self.remove_mod)
        remove_mod_button.grid(row=len(self.mod_data), column=0, pady=5, sticky='ew')

        # Add button to add new key-value pair
        add_pair_button = tk.Button(self.editor_window, text="Add Property", command=self.add_key_value_pair)
        add_pair_button.grid(row=len(self.mod_data), column=2, pady=5, sticky='ew')

    def add_key_value_pair(self, key=None, value=None):
        if key is None or value is None:
            key = f"NewKey{len(self.key_entries) + 1}"
            value = ""

        frame = tk.Frame(self.editor_window)
        row_index = len(self.key_entries)  # Determine row index for new key-value pair

        key_entry = tk.Entry(frame)
        key_entry.insert(0, key)
        key_entry.grid(row=0, column=0, padx=5)

        value_label = tk.Label(frame, text=":")
        value_label.grid(row=0, column=1, padx=5)

        value_control, value_var = create_control_for_value(frame, value)
        value_control.grid(row=0, column=2, padx=5)

        # Set the value in the control
        if isinstance(value_var, tk.BooleanVar):
            value_var.set(value)  # Populate BooleanVar with the value from JSON
        elif isinstance(value_var, tk.StringVar):
            value_var.set(str(value))  # Populate StringVar with the value from JSON
        elif isinstance(value_var, tk.DoubleVar):
            value_var.set(float(value))  # Populate DoubleVar for numbers

        # Button to remove this key-value pair
        remove_pair_button = tk.Button(frame, text="Remove", command=lambda: self.remove_key_value_pair(frame, key_entry, value_control))
        remove_pair_button.grid(row=0, column=3, padx=5)

        # Store references to entries
        self.key_entries[key_entry] = key_entry
        self.value_entries[key] = value_var if value_var else value_control  # Use value_var for traceable types
        self.pair_frames.append(frame)

        # Add frame to the window
        frame.grid(row=row_index, column=0, columnspan=3, sticky='ew', padx=5, pady=5)

        # Bind the entry change to update the mod_data
        key_entry.bind("<FocusOut>", self.update_mod_data)
        
        # Use trace_add only for tk.Variable-based controls (BooleanVar, StringVar, DoubleVar)
        if isinstance(value_var, (tk.BooleanVar, tk.StringVar, tk.DoubleVar)):
            value_var.trace_add("write", lambda *args: self.update_mod_data(None))

    def update_mod_data(self, event):
        """Update the mod_data with the current key-value pairs."""
        self.mod_data.clear()  # Clear existing mod_data

        # Loop through the stored key and value entries to update mod_data
        for key_entry, value_var in zip(self.key_entries.keys(), self.value_entries.values()):
            key = key_entry.get()  # Get key from the entry field
            if isinstance(value_var, tk.BooleanVar):
                value = value_var.get()  # Get boolean value from BooleanVar
            elif isinstance(value_var, tk.StringVar):
                value = value_var.get()  # Get string value from StringVar
            elif isinstance(value_var, tk.DoubleVar):
                value = value_var.get()  # Get numeric value from DoubleVar
            else:
                value = value_var.get()  # Get value from regular entry field

            # Only add non-empty keys
            if key:
                self.mod_data[key] = value

        # Save the updated mod_data to the parent
        self.parent.data["mods"][self.mod_name] = self.mod_data
        self.parent.save_json("shroudtopia.json")

    def remove_key_value_pair(self, frame, key_entry, value_entry):
        """Remove the key-value pair and update mod_data."""
        # Get the key value before destroying the widget
        key = key_entry.get()

        # Destroy the frame and widgets
        frame.destroy()

        # Remove entries from dictionaries
        if key in self.key_entries:
            del self.key_entries[key_entry]

        if key in self.value_entries:
            del self.value_entries[key]

        # Update mod_data after removal
        self.update_mod_data(None)


    def remove_mod(self):
        del self.parent.data["mods"][self.mod_name]
        self.parent.save_json("shroudtopia.json")
        self.editor_window.destroy()
        self.parent.populate_mods()  # Refresh the mod list

# Run the application
if __name__ == "__main__":
    app = ModManagerApp()
    app.root.mainloop()
