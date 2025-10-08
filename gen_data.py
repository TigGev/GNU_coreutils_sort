import random
import string

def generate_random_strings(file_path, num_lines, line_length=10, duplicates=False):
    with open(file_path, 'w') as f:
        if duplicates:
            choices = [''.join(random.choices(string.ascii_letters + string.digits, k=line_length)) for _ in range(num_lines // 2)]
            choices *= 2  # Duplicate 50% of lines
            random.shuffle(choices)
            for line in choices[:num_lines]:
                f.write(line + '\n')
        else:
            for _ in range(num_lines):
                line = ''.join(random.choices(string.ascii_letters + string.digits, k=line_length))
                f.write(line + '\n')

def generate_random_numbers(file_path, num_lines):
    with open(file_path, 'w') as f:
        for _ in range(num_lines):
            num = random.uniform(-1e6, 1e6)
            f.write(f"{num}\n")

# Generate test files
generate_random_strings('small_strings.txt', 1000)
generate_random_strings('medium_strings.txt', 1000000)
generate_random_strings('large_strings.txt', 10000000)
generate_random_numbers('medium_numbers.txt', 1000000)
generate_random_strings('medium_duplicates.txt', 1000000, duplicates=True)

print("Test data generated.")