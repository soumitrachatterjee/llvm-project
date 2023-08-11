import sys
import re

def find_all_class_and_struct_extents(cpp_code):
    class_pattern = r'\b(class|struct|union)\s+(\w+)\b'
    class_matches = re.finditer(class_pattern, cpp_code)

    class_extents = []

    for match in class_matches:
        class_kind = match.group(1)
        class_name = match.group(2)
        start_line = cpp_code.count('\n', 0, match.start()) + 1

        # Find the end of the class/struct/union definition by searching for the next '}'
        end_pos = match.end()
        brace_count = 1
        while brace_count > 0 and end_pos < len(cpp_code):
            if cpp_code[end_pos] == '{':
                brace_count += 1
            elif cpp_code[end_pos] == '}':
                brace_count -= 1
            end_pos += 1

        end_line = cpp_code.count('\n', 0, end_pos) + 1
        class_extents.append((class_kind, class_name, start_line, end_line))

    return class_extents

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py input.cpp")
        sys.exit(1)

    input_file = sys.argv[1]

    try:
        with open(input_file, 'r') as file:
            cpp_code = file.read()
    except FileNotFoundError:
        print(f"Error: File '{input_file}' not found.")
        sys.exit(1)

    class_extents = find_all_class_and_struct_extents(cpp_code)

    if not class_extents:
        print("No class, struct, or union definitions found in the code.")
    else:
        for class_kind, class_name, start_line, end_line in class_extents:
            print(f"The {class_kind} '{class_name}' : {start_line} - {end_line}.")