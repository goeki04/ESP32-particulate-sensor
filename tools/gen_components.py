import os
import re
import sys

def generate_component_tuple(search_root, target_filename, rel_output_file):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.abspath(os.path.join(script_dir, ".."))
    
    abs_search_root = os.path.join(project_root, search_root)
    abs_output_file = os.path.join(project_root, rel_output_file)

    print(f"-- ECS Generator: Searching in {abs_search_root}")

    if not os.path.exists(abs_search_root):
        print(f"!! ERROR: Search path {abs_search_root} does not exist!")
        return

    components = []
    pattern = re.compile(r'struct\s+(\w+)[^{]*\{')

    for root, dirs, files in os.walk(abs_search_root):
        for file in files:
            if file == target_filename:
                file_path = os.path.join(root, file)
                print(f"-- ECS Generator: Processing {file_path}")
                with open(file_path, 'r') as f:
                    content = f.read()
                    found = pattern.findall(content)
                    components.extend(found)

    components = list(set(components))
    print(f"-- ECS Generator: Found {len(components)} components: {components}")

    os.makedirs(os.path.dirname(abs_output_file), exist_ok=True)

    with open(abs_output_file, 'w') as f:
        f.write("#pragma once\n\n")
        f.write("#include <tuple>\n\n")
        f.write("namespace Andromeda::ECS::Component {\n")

        for c in sorted(components):
            f.write(f"    struct {c};\n")
            
        f.write("\n    using ComponentDirectory = std::tuple<\n")
        f.write(",\n".join([f"        {c}" for c in sorted(components)]))
        f.write("\n    >;\n")
        f.write("}\n")

if __name__ == "__main__":
    generate_component_tuple(
        "modules", 
        "a_components.hpp", 
        "modules/definitions/generated_components.hpp"
    )