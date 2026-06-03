import os
total_lines = 0;
def count_lines_in_modules(directory):
    global total_lines
    extensions = ('.cpp', '.hpp', '.h', '.txt', '.glsl', '.vert', '.frag', '.py')
    
    if not os.path.exists(directory):
        print(f"Error: The folder '{directory}' couldn't be found.")
        return

    for root, dirs, files in os.walk(directory):
        if 'bindings' in root:
            continue
            
        for file in files:
            if file.endswith(extensions):
                path = os.path.join(root, file)
                try:
                    with open(path, 'r', encoding='utf-8', errors='ignore') as f:
                        lines = len(f.readlines())
                        total_lines += lines
                        print(f"{path}: {lines}")
                except Exception as e:
                    print(f"Error while reading from {path}: {e}")
                    
    print("-" * 30)


count_lines_in_modules('modules')
count_lines_in_modules("tools")
count_lines_in_modules("metaData")
count_lines_in_modules("conan/build/Release/generated")
print(f"Line count (excluding bindings): {total_lines}")