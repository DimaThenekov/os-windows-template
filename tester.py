import os
import subprocess
import time
from pathlib import Path
import re

def compile_sources(output_filename, input_array_cpp_files):
    build_dir = Path("build")
    build_dir.mkdir(exist_ok=True)
    result = subprocess.run(["g++", "-o", str(build_dir / output_filename)] + input_array_cpp_files, capture_output=True, text=True)
    if result.returncode != 0:
        print("Ошибка компиляции:")
        print(result.stderr)
        return False
    else:
        return True

def run_and_validate(output_filename, input_data, validate_output):
    executable_path = Path("build") / output_filename
    if not executable_path.exists():
        print(f"Исполняемый файл {output_filename} не найден.")
        return
    
    for i, input_value in enumerate(input_data):
        input_lines = input_value.splitlines()
        process = subprocess.Popen([str(executable_path)], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        for line in input_lines:
            process.stdin.write(line + '\n')
            process.stdin.flush()
            time.sleep(0.5)
        process.stdin.close()
        stdout, stderr = process.communicate()
        if process.returncode != 0:
            print(f"Ошибка при выполнении теста {i} с входными данными: {input_value}")
            print(stderr)
            continue
        validate_output(i+1, input_value, stdout)

def main():
    output_filename = "main.exe"
    output_benchmark = "benchmark.exe"
    cpp_source = [str(file) for file in Path("./source").rglob("*.cpp")]
    cpp_benchmark = [str(file) for file in Path("./benchmark").rglob("*.cpp")]
    input_data = ["cd build\n./benchmark.exe\nexit\n"]
    output_data = ["__PATH__$ cd__PATH__\\build$ Sleep 5.5s...\nDonen./benchmark.exeExecution time: __NUM__ seconds\n__PATH__\\build$ exit"]
    def validate_output(test_number, input_value, output_value):
        cout = output_value.replace(os.path.dirname(os.path.abspath(__file__)), "__PATH__")
        cout = re.sub(r"\d+.\d+ seconds", "__NUM__ seconds", cout)
        if cout == output_data[test_number-1]:
            print(f"Тест {test_number} пройден")
        else:
            print(f"Тест {test_number} не пройден:\nвходные данные: {input_value}\nрезультат: {cout}\nожидалось: {output_data[test_number-1]}")
            exit(1)

    if compile_sources(output_filename, cpp_source):
        if compile_sources(output_benchmark, cpp_benchmark):
            run_and_validate(output_filename, input_data, validate_output)

if __name__ == "__main__":
    main()
