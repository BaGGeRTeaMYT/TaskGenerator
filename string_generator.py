import exrex
import sys

def generate_string(pattern: str) -> str:
    return exrex.getone(pattern)

if (len(sys.argv) < 3):
    raise RuntimeError("Not enough arguments given")

input_name = sys.argv[1]
output_name = sys.argv[2]

with open(input_name, "r") as input_file:
    pattern = input_file.read().strip()

# print("Python got pattern ", pattern)
result = generate_string(pattern)
with open(output_name, "w") as output_file:
    output_file.write(result)