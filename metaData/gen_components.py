import os
import re
import sys

def generate_component_tuple(search_root, target_filename, rel_output_file, output_undo_rel):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.abspath(os.path.join(script_dir, ".."))
    
    abs_search_root = os.path.join(project_root, search_root)
    abs_output_file = os.path.join(project_root, rel_output_file)
    abs_output_undo = os.path.join(project_root, output_undo_rel)

    print(f"-- ECS Generator: Searching in {abs_search_root}")

    if not os.path.exists(abs_search_root):
        print(f"!! ERROR: Search path {abs_search_root} does not exist!")
        return

    all_components = set()
    undo_components = set()
    
    pattern = re.compile(r'struct\s+(\[\[Andromeda::Undo\]\]\s+)?(\w+)[^{]*\{')

    for root, dirs, files in os.walk(abs_search_root):
        for file in files:
            if file == target_filename:
                file_path = os.path.join(root, file)
                print(f"-- ECS Generator: Processing {file_path}")
                with open(file_path, 'r') as f:
                    content = f.read()
                    for match in pattern.finditer(content):
                        has_undo_attr = match.group(1) is not None
                        name = match.group(2)
                        
                        all_components.add(name)
                        if has_undo_attr:
                            undo_components.add(name)

    print(f"-- ECS Generator: Found {len(all_components)} components.")

    # File 1: Component Tuple
    os.makedirs(os.path.dirname(abs_output_file), exist_ok=True)
    with open(abs_output_file, 'w') as f:
        f.write("#pragma once\n\n#include <tuple>\n\n")
        f.write("namespace Andromeda::ECS::Component {\n")
        for c in sorted(all_components):
            f.write(f"    struct {c};\n")
            
        f.write("\n    using ComponentDirectory = std::tuple<\n")
        f.write(",\n".join([f"        {c}" for c in sorted(all_components)]))
        f.write("\n    >;\n}\n")
        
    # File 2: Undo Commands
    os.makedirs(os.path.dirname(abs_output_undo), exist_ok=True) 
    with open(abs_output_undo, 'w') as f:
        f.write("#pragma once\n#include \"a_components.hpp\"\n#include <cstdint>\n#include \"a_primitives.hpp\"\n\n")
        f.write("namespace Andromeda::Editor::Undo {\n\n")
        f.write("enum class CommandType : uint16_t {\n")
        sorted_undo = sorted(undo_components)
        for i, c in enumerate(sorted_undo):
            f.write(f"    Update{c} = {i},\n")
        f.write(f"    COUNT = {len(sorted_undo)}\n")
        f.write("};\n")
        
        f.write("struct CommandHeader {\n    CommandType type;\n    u32 dataSize;\n};\n")
        for c in sorted_undo:
            f.write(f"\nstruct Undo{c}Data {{\n    ECS::Entity entityID;\n")
            f.write(f"    ECS::Component::{c} oldState;\n")
            f.write(f"    ECS::Component::{c} newState;\n}};\n")
        f.write("\n}\n")

    # File 3
    dispatcher_inc = abs_output_undo.replace("generated_undo_commands.hpp", "generated_dispatcher.inc")
    with open(dispatcher_inc, 'w') as f:
        f.write("// Generated jump table\n\n")
        for c in sorted_undo:
            f.write(f"static void undo_func_{c}(uint8_t* payload, ECS::ComponentRegistry& reg) {{\n")
            f.write(f"    auto* d = reinterpret_cast<Undo{c}Data*>(payload);\n")
            f.write(f"    reg.getPool<ECS::Component::{c}>().get(d->entityID) = d->oldState;\n")
            f.write(f"}}\n\n")
        f.write(f"static void(*g_undoDispatchTable[(size_t)CommandType::COUNT])(uint8_t*, ECS::ComponentRegistry&) = {{\n")
        for c in sorted_undo:
            f.write(f"    undo_func_{c},\n")
        f.write("};\n")

    print(f"Generator: Success! Total: {len(all_components)}, Undo-ready: {len(undo_components)}")

if __name__ == "__main__":
    generate_component_tuple(
        "modules", 
        "a_components.hpp", 
        "modules/definitions/generated_components.hpp",
        "modules/definitions/generated_undo_commands.hpp"
    )