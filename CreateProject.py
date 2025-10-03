import traceback
import shutil
import os
from ctypes import windll
import sys
import subprocess
import re
from pathlib import Path
import threading
import time
try:
    from tkinter import Tk, filedialog, StringVar, messagebox, ttk, Toplevel, BooleanVar, IntVar
    
    # ---------------------- Helper functions ----------------------
    
    def error_fallback(remove_directory):
        try:
            shutil.rmtree(remove_directory)
            return True
        except Exception as e:
            messagebox.showerror("Error", "Failed to clean when got error during generation")
            return False
    
    def copy_project_structure(source, destination):
        try:
            shutil.copytree(source, destination, dirs_exist_ok=True)
            return True
        except Exception as e:
            messagebox.showerror("Error", f"Failed to copy project structure: '{e}'")
            error_fallback(destination)
            return False
            
    
    def generate_build_scripts(script_base, project_name, compiler, use_submodule, engine_root):
        build_script_content = ""
        try:
            with open(script_base, 'r') as file:
                build_script_content = file.read()
        except Exception as e:
            messagebox.showerror("Error", f"Failed to generate scripts\n{e}")
            return
        
        try:
            os.remove(script_base)
        except Exception as e:
            messagebox.showerror("Error", f"Failed to generate scripts\n{e}")
            return
        
        
        generator = ""
        if compiler == "GCC":
            generator = "MinGW Makefiles"
        elif compiler == "MSVC":
            generator = "Visual Studio 17 2022"
        
        engine_install_location = os.path.join(engine_root, "Build", compiler, "Install")
        submodule_option = "ON" if use_submodule else "OFF"
        
        targets = ["","-Server"]
        configurations = ["Debug", "Development", "Shipping"]
           
        script_name_base = os.path.basename(script_base)
        script_type = "Submodule" if use_submodule else "Installed" 
        script_name_type = script_name_base.replace("Type", script_type)
        script_name_type = script_name_type.replace("Compiler", compiler)
        
        for conf in configurations:
            script_name_conf = script_name_type.replace("Configuration", conf)
            for target in targets:
                script_name_conf_target = script_name_conf.replace("-Target", target)
                script_location = os.path.dirname(script_base)
                script_full_filename = os.path.join(script_location, script_name_conf_target)
                
                replacements = {
                    "#Compiler#" : compiler,
                    "#Generator#" : generator,
                    "#Template_Blank#" : project_name,
                    "#Configuration#" : conf,
                    "#Target#" : target,
                    "#Engine_Install_Location#" : engine_install_location,
                    "#Use_Submodule#" : submodule_option
                }
                new_content = build_script_content
                for key, value in replacements.items():
                    new_content = new_content.replace(key, value)
                
                pattern = r"\[\[(.*?)\]\]"
                replacement = r"" if use_submodule else r"\1"
                new_content = re.sub(pattern, replacement, new_content)
                
                with open(script_full_filename, 'w') as file:
                    file.write(new_content)
                    
                
        
    
    def rename_source_files(source_folder, placeholder, project_name):
        for dirpath, dirnames, filenames in os.walk(source_folder):
            for original_name in filenames:
                if placeholder in original_name:
                    new_name = original_name.replace(placeholder, project_name)
                    old_file_path = os.path.join(dirpath, original_name)
                    new_file_path = os.path.join(dirpath, new_name)
                    try:
                        os.rename(old_file_path, new_file_path)
                    except Exception as e:
                        messagebox.showerror("Error", f"Failed to rename '{old_file_path}' to '{new_file_path}'. Project generation aborted")
                        error_fallback(project_root)
                        return False
        return True
                    
    
    def substitute_placeholders(replacements_directories, replacements):
        for rep_directory in replacements_directories:
            contents = os.listdir(rep_directory)
            for name in contents:
                full_path = os.path.join(rep_directory, name)
                if os.path.isfile(full_path):
                    with open(full_path, 'r', encoding="utf-8") as file:
                        file_content = file.read()
                    for key, value in replacements.items():
                        file_content = file_content.replace(key, value)
                    with open(full_path, 'w', encoding="utf-8") as file:
                        file.write(file_content)
    
    def find_install_folders(root_folder):
        if not os.path.isdir(root_folder):
            return {}
        root_folder_abs = os.path.abspath(root_folder)
        install_paths = {}
        
        for dirpath, dirnames, filenames in os.walk(root_folder_abs):
            current_dir_name = os.path.basename(dirpath)
            if current_dir_name == "Install":
                install_folder_abs_path = dirpath
                try:
                    relative_path = os.path.relpath(install_folder_abs_path, root_folder_abs)
                except ValueError:
                    continue
                
                if relative_path == ".":
                    continue
                
                path_components = relative_path.split(os.path.sep)
                first_folder_name = path_components[0]
                install_paths[first_folder_name] = install_folder_abs_path
        return install_paths
    
    def check_for_lib_files(start_path):
        if not os.path.isdir(start_path):
            return False
        target_extensions = {'.a', '.lib'}
        
        found_configurations = []
        
        for dirpath, dirnames, filenames in os.walk(start_path):
            for name in filenames:
                root, ext = os.path.splitext(name)
                if ext.lower() in target_extensions:
                    if "Debug" in name:
                        found_configurations.append("Debug")
                    elif "Development" in name:
                       found_configurations.append("Development")
                    else:
                        found_configurations.append("Shipping")
        return found_configurations
    
    def run_script(command):
        if not os.path.exists(command[0]):
            messagebox.showerror("Error", f"Script not found at '{command[0]}'")
            return False
        try:
            result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            shell=True,
            check=True,
            input='\n',
            timeout=300
            )
            if result.stderr:
                messagebox.showwarning("Warning", f"Script warning '{command[0]}'\n{result.stderr}")
                return False
                
            return True
        except subprocess.CalledProcessError as e:
            messagebox.showerror("Error", f"Batch file failed with {e.returncode}\nCommand output: {e.stdout}\nError details: {e.stderr}")
        except FileNotFoundError:
            messagebox.showerror("Error", "Command or file not found.")
        except Exception as e:
            messagebox.showerror("Error", f"An unexpected error occurred during execution: {e}")
        return False
            
    
    def run_install_script(bat_path, compiler, configuration):
        if not os.path.exists(bat_path):
            messagebox.showerror("Error", f"Install script not found at '{bat_path}'")
            return False
        command = [bat_path, compiler, configuration]
        
        try:
            result = subprocess.run(
                command,
                capture_output=True,
                text=True,
                shell=True,
                check=True,
                input='\n',
                timeout=300
            )
            if result.stderr:
                messagebox.showwarning("Warning", f"Failed to install Engine\n{result.stderr}")
                return False
                
            messagebox.showinfo("Success", f"Termynth Engine successfully installed")
            return True
        except subprocess.CalledProcessError as e:
            messagebox.showerror("Error", f"Batch file failed with {e.returncode}\nCommand output: {e.stdout}\nError details: {e.stderr}")
        except FileNotFoundError:
            messagebox.showerror("Error", "Command or file not found.")
        except Exception as e:
            messagebox.showerror("Error", f"An unexpected error occurred during execution: {e}")
        return False
    
    def check_installation(root, compiler_option):
        title = "Engine is not installed"
        message = f"Engine built with {compiler_option} is not installed\nDo you want to install now?"
        
        installations = find_install_folders(os.path.join(root, "Build"))
        installed = False
        found_conf = []
        for compiler, inst_path in installations.items():
            found_conf += check_for_lib_files(inst_path)
            if compiler != compiler_option or not found_conf:
                found_conf = []
        
        #messagebox.showinfo("Error", f"Engine installed")
        
        if not found_conf:
            response = messagebox.askyesno(title, message)
            if response:
                success = run_install_script(os.path.join(root, "Install.bat"), compiler_option, "Shipping")
                if success:
                    installed = True
                    found_conf.append("Shipping")
        return found_conf
    
    def set_progress(value):
        main_frame.after(0, lambda: progress_var.set(value))
        time.sleep(0.01)
    
    def generate_project(on_finish_callback, on_cancel_callback, progress_var, use_submodule, compiler_option):
        project_name = project_name_var.get().strip() # Remove whitespaces
        destination = dest_folder_var.get().strip()
        
        if not use_submodule:
            if not project_name or not destination:
                messagebox.showerror("Error", "Please provide project name and destination folder.")
                on_cancel_callback()
                return
                
            if not project_name.isalnum():
                messagebox.showerror("Error", "Project name should contain only letters and digits.")
                on_cancel_callback()
                return;
            if project_name[0].isdigit():
                messagebox.showerror("Error", "Project name should start with a letter.")
                on_cancel_callback()
                return;
        set_progress(10)
        engine_root = os.path.dirname(os.path.abspath(sys.argv[0]))
        
        installed_confs = []
        if not use_submodule:
            installed_confs = check_installation(engine_root, compiler_option)
            if not installed_confs:
                messagebox.showerror("Error", f"Engine is not installed\nExiting...")
                on_cancel_callback()
                return
        set_progress(60)
        project_root = destination
        if not use_submodule:
            project_root = os.path.join(destination, project_name)
            if os.path.exists(project_root):
                messagebox.showerror("Error", f"Project with name '{project_name}' at '{destination}' already exists")
                on_cancel_callback()
                return
            os.makedirs(project_root, exist_ok = True)
        else:
            normalized_path = os.path.normpath(project_root)
            project_name = os.path.basename(normalized_path)
        
        set_progress(65)
        
        engine_relative_to_project = ""
        if use_submodule:
            try:
                engine_relative_to_project = os.path.relpath(engine_root, project_root)
            except ValueError as e:
                messagebox.showerror("Error", "Placing project on different drive is not allowed with building from source")
            engine_r = Path(os.path.abspath(engine_root))
            project_r = Path(os.path.abspath(project_root))
            if not engine_r.is_relative_to(project_r):
                messagebox.showerror("Error", "Engine should be a subdirectory of a project with building from source")
                on_cancel_callback()
                return 
        set_progress(70)        
        template_folder = os.path.join(engine_root, "Templates/Template_Blank")

        success = copy_project_structure(template_folder, project_root)
        if not success:
            on_cancel_callback()
            return
        
        set_progress(75)  
        base_script_filepath = os.path.join(project_root, "Scripts/Type-Build-Compiler-Configuration-Target.bat")
        generate_build_scripts(base_script_filepath, project_name, compiler_option, use_submodule, engine_root)
        set_progress(80)  
     
        source_folder = os.path.join(project_root, "Source")
        success = rename_source_files(source_folder, "Template_Blank", project_name)
        if not success:
            on_cancel_callback()
            return
        set_progress(85)  
       
        
        
        replacements = {
            "#Template_Blank#" : project_name
        }
        
        if use_submodule:
            replacements["#Engine_Path#"] = engine_relative_to_project.replace(os.sep, '/')
        else:
            replacements["#Engine_Path#"] = "Placeholder"
        
        replacements_directories = [
            os.path.join(project_root, "Source/Private"), 
            os.path.join(project_root, "Source/Public"), 
            project_root
        ]       
        substitute_placeholders(replacements_directories, replacements)
        set_progress(90)  
        sn_type = "Submodule" if use_submodule else "Installed"
        script_conf = ""
        
        if use_submodule:
            script_conf = "Shipping"
        else:
            if "Shipping" in installed_confs:
                script_conf = "Shipping"
            elif "Development" in installed_confs:
                script_conf = "Development"
            elif "Debug" in installed_confs:
                script_conf = "Debug"
        
        build_script_name = "-".join([sn_type, "Build", compiler_option, script_conf])
        build_script_name += ".bat"
        
        run_script([os.path.join(project_root, "Scripts", build_script_name)])
        set_progress(100)  
        messagebox.showinfo("Success", f"Project {project_name} generated!")
        on_finish_callback()
        
                        
    
    
    # ---------------------- GUI ----------------------
    
    class ResizableWindow:
        def __init__(self, root, swidth, sheight):
            self.root = root
            
            screen_w = root.winfo_screenwidth()
            screen_h = root.winfo_screenheight()
            
            sposx = (screen_w // 2) - (swidth // 2)
            sposy = (screen_h // 2) - (sheight // 2)
            
            self.root.geometry(f"{swidth}x{sheight}+{sposx}+{sposy}")
            
            self._offset_x = 0
            self._offset_y = 0
            self.resizing = False
            self.resize_direction = ""
            self.border_size = 5
            
            root.bind('<Button-1>', self.start_drag)
            root.bind('<B1-Motion>', self.drag_window)
            root.bind('<ButtonRelease-1>', self.stop_action)
            
            root.bind('<Motion>', self.check_border)
            root.bind('<Button-1>', self.start_resize,add='+')
            root.bind('<B1-Motion>', self.resize_window, add='+')
            
              
            
        def start_drag(self, event):
            self._offset_x = event.x
            self._offset_y = event.y
        
        def drag_window(self, event):
            x = self.root.winfo_x() + event.x - self._offset_x
            y = self.root.winfo_y() + event.y - self._offset_y
            self.root.geometry(f"+{x}+{y}")
        
        def stop_action(self, event):
            self._offset_x = 0
            self._offset_y = 0
            self.resizing = False
            self.resize_direction = ""
            
        def check_border(self, event):
            # Get window geometry
            win_x = self.root.winfo_x()
            win_y = self.root.winfo_y()
            win_w = self.root.winfo_width()
            win_h = self.root.winfo_height()
            bs = self.border_size
    
            # Get mouse position in screen coordinates
            mx, my = event.x_root, event.y_root
    
            direction = []
            cursor_direction = ""
    
            # Check vertical borders
            if win_y <= my <= win_y + bs:
                direction.append("top")
            elif win_y + win_h - bs <= my <= win_y + win_h:
                direction.append("bottom")
            
            # Check horizontal borders
            if win_x <= mx <= win_x + bs:
                direction.append("left")
            elif win_x + win_w - bs <= mx <= win_x + win_w:
                direction.append("right")
    
            if direction:
                selected = "_".join(direction)
                suffix = "_corner" if len(direction) > 1 else "_side"
                cursor_direction = f"{selected}{suffix}"
                self.root.config(cursor=cursor_direction)
            else:
                self.root.config(cursor="")
            
            self.resize_direction = cursor_direction

        def start_resize(self, event):
            """Starts the resize process if a border is clicked."""
            if self.resize_direction:
                self.resizing = True
                # Store the current window dimensions and mouse offset for resizing
                self._offset_x_root = event.x_root
                self._offset_y_root = event.y_root
                
                self._start_w = self.root.winfo_width()
                self._start_h = self.root.winfo_height()
                self._start_x = self.root.winfo_x()
                self._start_y = self.root.winfo_y()
    
        def resize_window(self, event):
            """Calculates and applies the new window size and position."""
            if not self.resizing:
                return
    
            w, h = self._start_w, self._start_h
            x, y = self._start_x, self._start_y
            
            # Change in mouse position since click
            dx = event.x_root - self._offset_x_root
            dy = event.y_root - self._offset_y_root
            
            if 'left' in self.resize_direction:
                # West edge resize: increase width and move window left
                w -= dx
                x += dx
            elif 'right' in self.resize_direction:
                # East edge resize: only increase width
                w += dx
                
            if 'top' in self.resize_direction:
                # North edge resize: increase height and move window up
                h -= dy
                y += dy
            elif 'bottom' in self.resize_direction:
                # South edge resize: only increase height
                h += dy
    
            # Apply new geometry, ensuring min size (e.g., 50x50)
            if w > 50 and h > 50:
                self.root.geometry(f"{w}x{h}+{x}+{y}")
    
    GWL_EXSTYLE=-20
    WS_EX_APPWINDOW=0x00040000
    WS_EX_TOOLWINDOW=0x00000080
    
    def browse_folder():
        folder_selected = filedialog.askdirectory()
        if  folder_selected:
            dest_folder_var.set(folder_selected)
            
    def close_window(window):
        window.destroy()
    
    def set_appwindow(root):
        hwnd = windll.user32.GetParent(root.winfo_id())
        style = windll.user32.GetWindowLongW(hwnd, GWL_EXSTYLE)
        style = style & ~WS_EX_TOOLWINDOW
        style = style | WS_EX_APPWINDOW
        res = windll.user32.SetWindowLongW(hwnd, GWL_EXSTYLE, style)
        root.wm_withdraw()
        root.after(10, lambda: root.wm_deiconify())
     
    def generation_finished():
        main_frame.after(0, task_done_ui)
    
    def generation_cancelled():
        main_frame.after(0, task_cancelled_ui)
    
    def task_done_ui():
        create_btn.config(state="normal")
        cancel_btn.config(state="normal")
        app_root.destroy()
    
    def task_cancelled_ui():
        create_btn.config(state="normal")
        cancel_btn.config(state="normal")
        messagebox.showinfo("Fail", f"Project generation cancelled")
    
    def start_generation():
        create_btn.config(state="disabled")
        cancel_btn.config(state="disabled")
        progress_var.set(5)
        
        thread = threading.Thread(
            target = generate_project, args=(generation_finished,generation_cancelled, progress_var, selection_var.get(), compiler_selection.get()), daemon=True
        )
        thread.start()
    app_root = Tk()
    #app_root.title("Termynth Project Generator")
    app_root.wm_title("Termynth Project Generator")

    style=ttk.Style()
    style.theme_use('clam')

    ResizableWindow(app_root, 600, 182)
        
    bg_frame = ttk.Frame(app_root)
    bg_frame.pack(padx=0, pady=0, fill='both',expand=True)
    main_frame = ttk.Frame(bg_frame)
    main_frame.pack(padx=20, pady=(5, 0), fill='both', expand=True) 
     
    ttk.Label(main_frame, text="Termynth Project Generator", font=('Arial', 10, 'bold')).pack(
        padx=20,
        pady=5
    )
        
    LABEL_WIDTH = 16
        
    style = ttk.Style()
    style.configure("Custom.TEntry", font=('Arial', 10), borderwidth=5)
        
    row1 = ttk.Frame(main_frame)
    row1.pack(fill='x', padx=0, pady=5)     
    ttk.Label(row1, text="Project Name:", font=('Arial', 10), width=LABEL_WIDTH, anchor='w').pack(side='left', padx=0)  
    project_name_var = StringVar()
    ttk.Entry(row1, textvariable=project_name_var, style="Custom.TEntry").pack(
        side='left',
        fill='x',
        expand=True,
        padx=10
    )
    
    row2 = ttk.Frame(main_frame)
    row2.pack(fill='x', padx=0, pady=5)
     
    ttk.Label(row2, text="Destination Folder:", font=('Arial', 10), width=LABEL_WIDTH, anchor='w').pack(side='left', padx=0)
    dest_folder_var = StringVar()
    ttk.Entry(row2, textvariable=dest_folder_var, style="Custom.TEntry").pack(
        side='left',
        fill='x',
        expand=True,
        padx=10
    )
    
    browse_frame = ttk.Frame(row2, width = 30, height=21)
    browse_frame.pack_propagate(False)
    browse_frame.pack(padx=(0, 10), pady=10, side='right')
    
    style = ttk.Style()
    style.configure("Folder.TButton", font=('Arial', 11), padding=(0, 0, 0, 0), anchor='n', relief='solid')
    
    ttk.Button(browse_frame, text="📁", command=browse_folder, style="Folder.TButton").pack(fill='both', expand=True)
    
    progress_var = IntVar(value=0)
    
    progress_frame = ttk.Frame(bg_frame, height=10)
    progress_frame.pack(fill='x', side='bottom', pady=0, padx=0)
    progress_frame.pack_propagate(False)
    
    style.configure("Custom.Horizontal.TProgressbar", thickness=10,  background='#78ff7f') 
    
    progress = ttk.Progressbar(progress_frame,orient='horizontal', mode='determinate', variable=progress_var, maximum=100, length=500, style="Custom.Horizontal.TProgressbar")
    progress.pack(fill='both', padx=0, pady=0)
    
    
    row3 = ttk.Frame(main_frame, height = 24)
    row3.pack_propagate(False)
    row3.pack(fill='x', padx=(0, 10), pady=(5, 10), side='bottom')
    
    style = ttk.Style()
    style.configure("My.TButton", font=('Arial', 10, 'bold'),padding=(0, 0, 0, 0), width=15, relief='solid')
    style.configure("MyCreate.TButton", font=('Arial', 10, 'bold'),padding=(0, 0, 0, 0), width=15, relief='solid')
    
    style.configure("My.TRadiobutton", font=('Arial', 10), padding=(0, 0, 0, 0))
    
    row4 = ttk.Frame(main_frame, height = 15)
    row4.pack_propagate(False)
    row4.pack(fill='x', padx=(0,10), pady=0)
    
    compiler_selection = StringVar(value="GCC")
    
    radio_gcc = ttk.Radiobutton(row4, text="GCC", variable=compiler_selection, value="GCC", style="My.TRadiobutton")
    radio_gcc.pack(side='left', padx=(0,5))
    radio_msvc = ttk.Radiobutton(row4, text="MSVC", variable=compiler_selection, value="MSVC", style="My.TRadiobutton")
    radio_msvc.pack(side='left', padx=(5, 0))
    
    
    selection_var = BooleanVar(value=False)
    radio_a = ttk.Radiobutton(row3, text="Engine Installed", variable=selection_var, value=False, style="My.TRadiobutton")
    radio_a.pack(side='left', padx=(0, 5))
    radio_b = ttk.Radiobutton(row3, text="Engine Source", variable=selection_var, value=True, style="My.TRadiobutton")
    radio_b.pack(side='left', padx=(5, 0))
    
    
    create_btn = ttk.Button(row3, text="Create", command=start_generation, style="MyCreate.TButton")
    create_btn.pack(side='right', padx=(5, 0))
    cancel_btn = ttk.Button(row3, text="Cancel", command=lambda: close_window(app_root),  style="My.TButton")
    cancel_btn.pack(side='right', padx=(0, 5))
    
    
    
    app_root.overrideredirect(True) 
    app_root.after(10, lambda: set_appwindow(app_root))
    app_root.mainloop()
    
except Exception as e:
    print("An error occured:")
    traceback.print_exc()
    input("Press Enter to exit...")