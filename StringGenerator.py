import exrex
import sys

def generate_string(pattern: str, n: int) -> str:
    ans = ""
    for i in range(n):
        ans += exrex.getone(pattern) + "\n"
    return ans

if (len(sys.argv) < 3):
    raise RuntimeError("Not enough arguments given")

input_name = sys.argv[1]
output_name = sys.argv[2]

n = 1
if (len(sys.argv) == 4):
    n = int(sys.argv[3])

with open(input_name, "r", encoding='utf-8') as input_file:
    pattern = input_file.read().strip()

# print("Python got pattern ", pattern)
result = generate_string(pattern, n)
with open(output_name, "w", encoding='utf-8') as output_file:
    output_file.write(result)