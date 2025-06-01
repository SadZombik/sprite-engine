import os
import subprocess
from collections import defaultdict

exe_path = os.path.join(os.path.dirname(__file__),
    '..\\',  
    'build\\',
    'Debug\\',
    'generate-sprite-sheet.exe'
)

res_folder = os.path.join(os.path.dirname(__file__), '..', 'res')

def get_all_file_paths(root_folder):
    file_paths = []
    for dirpath, dirnames, filenames in os.walk(root_folder):
        for filename in filenames:
            file_paths.append(os.path.join(dirpath, filename))
    return file_paths

if __name__ == "__main__":
    all_files = get_all_file_paths(res_folder)

    # Group PNG files by their parent folder
    folders = defaultdict(list)
    for path in all_files:
        if path.lower().endswith('.png') and not path.endswith('sheet.png') and not path.endswith('count.txt'):
            folder = os.path.dirname(path)
            folders[folder].append(path)

    # Call generate-sprite-sheet.exe for each folder with PNGs
    for folder, pngs in folders.items():
        # Only process if there are multiple PNGs (optional)
        if len(pngs) > 0:
            sprite_path = os.path.join(folder, 'sheet.png')
            count_path = os.path.join(folder, 'count.txt')
            subprocess.run([exe_path, *pngs, '-o', sprite_path, count_path], check=True)
