import json

def generate_cpp_header(json_file_path, output_file_path):
    """
    Parses a JSON reflection file and generates a C++ header file with struct definitions.

    Args:
        json_file_path: Path to the input JSON file.
        output_file_path: Path to the output C++ header file.
    """

    try:
        with open(json_file_path, 'r') as f:
            data = json.load(f)
    except FileNotFoundError:
        print(f"Error: JSON file not found at {json_file_path}")
        return
    except json.JSONDecodeError:
        print(f"Error: Invalid JSON format in {json_file_path}")
        return

    cpp_code = """#pragma once // Include guard to prevent multiple inclusions

#include <stdint.h> // Include standard integer types (uint32_t, etc.)

namespace keen { // Assuming a 'keen' namespace, adjust as needed

"""

    for entry in data:
        type_name = entry.get("type_name_2")  # Use type_name_2 for consistency
        qualified_name = entry.get("qualified_name")
        class_size = entry.get("class_size")
        fields = entry.get("struct_fields")
        primitive_type = entry.get("primitive_type")
        referenced_type_name = entry.get("referenced_type_name")


        if primitive_type == "Struct":
            cpp_code += f"struct {type_name} {{\n"
            if fields:
                for field in fields:
                    field_type = field.get("type_name")
                    field_name = field.get("name")
                    offset = field.get("data_offset")
                    cpp_code += f"    {field_type} {field_name}; // Offset: {offset}\n"
            cpp_code += f"}};\n\n"

        elif primitive_type == "Typedef":
           cpp_code += f"typedef {referenced_type_name} {type_name};\n\n"

        elif primitive_type == "Enum": # Example enum handling (if you have enums in your JSON)
            enum_fields = entry.get("enum_fields")
            if enum_fields:
                cpp_code += f"enum class {type_name} {{\n"
                for enum_field in enum_fields:
                    enum_name = enum_field.get("name")
                    enum_value = enum_field.get("value") # Assuming value is provided
                    cpp_code += f"    {enum_name} = {enum_value},\n"
                cpp_code += "};\n\n"

        # Handle other types (primitives, etc.) as needed.  You might not need to do anything for primitives.
        # If you need specific handling for BlobString or other types, add it here.


    cpp_code += "}\n"  # Close the keen namespace

    try:
        with open(output_file_path, 'w') as outfile:
            outfile.write(cpp_code)
        print(f"C++ header file generated successfully at {output_file_path}")
    except Exception as e:
        print(f"Error writing to output file: {e}")


# Example usage:
json_file = "reflection_data.json"  # Replace with your JSON file path
header_file = "game_structures.h"  # Replace with your desired header file path
generate_cpp_header(json_file, header_file)