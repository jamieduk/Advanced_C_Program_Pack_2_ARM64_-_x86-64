README.md
Overview
Created By (c)J~Net 2024

This repository contains a collection of unique, lightweight, and standalone utilities developed for ARM64 and x86-64 architectures. 
These utilities are designed to enhance productivity and provide system management tools, including audio processing, clipboard management, encrypted notepads, hash checking, image compression, and more.

Programs List
1. Audio Cutter and Converter
Purpose: Trim and convert audio files with ease.
Architectures: ARM64, x86-64
Paths:
ARM64: /Audio-Cutter-and-Converter/ARM64/
x86-64: /Audio-Cutter-and-Converter/x86-64/
Files:
audio-cutter-and-converter.c: Source code
audio-cutter-and-converter: Executable
Notes.txt: Documentation
2. Clipboard Manager
Purpose: Advanced clipboard management with multiple versions.
Architectures: ARM64, x86-64
Versions:
V1: Basic clipboard management
V2: Enhanced features
Paths:
ARM64: /Clipboard-Manager-with/ARM64/
x86-64: /Clipboard-Manager-with/x86-64/
Files:
clipboard-manager.c: Source code
clipboard-manager: Executable
Notes.txt: Documentation
3. Encrypted Notepad
Purpose: Secure text storage with encryption and versioning.
Architectures: ARM64, x86-64
Versions:
V1: Basic encryption
V2: Enhanced encryption and additional features
Paths:
ARM64: /encrypted-notepad/ARM64/
x86-64: /encrypted-notepad/x86-64/
Files:
encrypted-notepad.c: Source code
encrypted-notepad: Executable
notes.enc: Encrypted notes file
Notes.txt: Documentation
4. File Hash Checker
Purpose: Generate and verify file hashes (e.g., MD5, SHA256).
Architectures: ARM64, x86-64
Paths:
ARM64: /file-hash-checker/ARM64/
x86-64: /file-hash-checker/x86-64/
Files:
file-hash-checker.c: Source code
file-hash-checker: Executable
Notes.txt: Documentation
5. Image Compressor
Purpose: Compress images while retaining quality.
Architectures: ARM64, x86-64
Paths:
ARM64: /image-compressor/ARM64/
x86-64: /image-compressor/x86-64/
Files:
image-compressor.c: Source code
image-compressor: Executable
Notes.txt: Documentation
6. Screenshot Editor
Purpose: Capture, edit, and save screenshots.
Architectures: ARM64, x86-64
Versions:
V1: Basic features
V2: Advanced editing tools
Paths:
ARM64: /screenshot-editor.c/ARM64/
x86-64: /screenshot-editor.c/x86-64/
Files:
screenshot-editor.c: Source code
screenshot-editor: Executable
Notes.txt: Documentation
7. Simple Backup Tool
Purpose: Create and manage backups efficiently.
Architectures: ARM64, x86-64
Paths:
ARM64: /Simple-Backup-Tool/ARM64/
x86-64: /Simple-Backup-Tool/x86-64/
Files:
simple-backup-tool.c: Source code
simple-backup-tool: Executable
Notes.txt: Documentation
8. Sysmon GUI
Purpose: Monitor system resources via a graphical interface.
Architectures: ARM64, x86-64
Versions:
V1: Basic monitoring
V2: Advanced resource tracking
Paths:
ARM64: /sysmon-gui/ARM64/
x86-64: /sysmon-gui/x86-64/
Files:
sysmon-gui.c: Source code
sysmon-gui: Executable
Notes.txt: Documentation
9. USB Device Manager
Purpose: Manage and configure USB devices.
Architectures: ARM64, x86-64
Paths:
ARM64: /usb-device-manager/ARM64/
x86-64: /usb-device-manager/x86-64/
Files:
usb-device-manager.c: Source code
usb-device-manager: Executable
Notes.txt: Documentation
Screenshot: Screenshot from 2024-11-25 13-05-52.png
Building and Running the Programs
Prerequisites
C Compiler: GCC or Clang
Dependencies: Check Notes.txt in each directory for additional requirements.
Build Instructions
Navigate to the program's directory.
Compile using:
bash
Copy code
gcc -o <executable-name> <source-file>.c
Example:
bash
Copy code
gcc -o audio-cutter-and-converter audio-cutter-and-converter.c
Run Instructions
Execute the compiled binary:

bash
Copy code
./<executable-name>
Notes
Detailed usage instructions are provided in Notes.txt for each program.
ARM64 and x86-64 versions are optimized for their respective architectures.
Licensing and Acknowledgments
All programs in this repository are open-source. See individual Notes.txt files for credits and licensing details. Contributions and feedback are welcome!
