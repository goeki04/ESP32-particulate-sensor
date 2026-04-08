import os
import re
import sys

def generate_component_tuple(search_root, target_filename, rel_output_file):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.abspath(os.path.join(script_dir, ".."))
    
    abs_search_root = os.path.join(project_root, search_root)
    abs_output_file = os.path.join(project_root, rel_output_file)

    print(f"-- ComponentNames: Searching in {abs_search_root}")

    if not os.path.exists(abs_search_root):
        print(f"!! ERROR: Search path {abs_search_root} does not exist!")
        return

    components = []

    for root, dirs, files in os.walk(abs_search_root):
        for file in files:
            if file == target_filename:
                file_path = os.path.join(root, file)
                print(f"-- ComponentNames generation: Processing {file_path}")
                with open(file_path, 'r') as f:
                    content = f.read()
                    
                    tuple_match = re.search(r'using ComponentDirectory = std::tuple<(.*?)>;', content, re.DOTALL)
                    if tuple_match:
                        raw_list = tuple_match.group(1).split(',')
                        components = [c.strip() for c in raw_list if c.strip()]
    if not components:
        return

    os.makedirs(os.path.dirname(abs_output_file), exist_ok=True)
    with open(abs_output_file, 'w') as f:
            f.write("#pragma once\n")
            f.write("#include <string_view>\n")
            f.write('#include "a_components.hpp"\n\n')
            
            f.write("namespace Andromeda::ECS::Component {\n\n")
            
            f.write("    template<typename T>\n")
            f.write("    constexpr std::string_view get_component_name() { return \"Unknown\"; }\n\n")
            
            for comp in components:
                f.write(f'    template<> constexpr std::string_view get_component_name<{comp}>() {{ return "{comp}"; }}\n')
                
            f.write("\n}\n")

if __name__ == "__main__":
    generate_component_tuple(
        "modules",
        "generated_components.hpp",
        "modules/definitions/generated_component_names.hpp"
    )