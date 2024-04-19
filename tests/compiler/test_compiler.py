import inspect
import os
import subprocess

from pathlib import Path

class TestCompiler(object):
    punkt_project_dir = Path
    compiler_path = Path
    input_directory = Path
    ir_output_directory = Path
    exe_output_directory = Path
    actual_output_directory = Path
    expected_output_directory = Path


    def setup_class(self):
        self.punkt_project_dir = Path(__file__).parent.parent.parent.resolve()

        self.compiler_path = Path(os.path.join(self.punkt_project_dir, "build", "applications", "punkt_compiler"))
        assert self.compiler_path.exists(), "executable file could not be found; make sure punkt_compiler has been built."

        self.input_directory = Path(os.path.join(self.punkt_project_dir, "tests", "test_input_files", "good"))
        assert self.input_directory.exists(), "input directory could not be found."

        self.ir_output_directory = Path(os.path.join(self.punkt_project_dir, "tests", "test_output_files", "ir_output"))
        self.ir_output_directory.mkdir(parents=True, exist_ok=True)
        # Remove any old IR output
        for file in os.scandir(self.ir_output_directory):
            os.unlink(file)

        self.exe_output_directory = Path(os.path.join(self.punkt_project_dir, "tests", "test_output_files", "exe_output"))
        self.exe_output_directory.mkdir(parents=True, exist_ok=True)
        # Remove any old executable output
        for file in os.scandir(self.exe_output_directory):
            os.unlink(file)

        self.actual_output_directory = Path(os.path.join(self.punkt_project_dir, "tests", "test_output_files", "actual", "compiler"))
        self.actual_output_directory.mkdir(parents=True, exist_ok=True)
        # Remove any old text output
        for file in os.scandir(self.actual_output_directory):
            os.unlink(file)

        self.expected_output_directory = Path(os.path.join(self.punkt_project_dir, "tests", "test_output_files", "expected", "compiler"))
        assert self.expected_output_directory.exists(), "expected output directory could not be found."

    
    def get_input_file_path(self) -> Path:
        return Path(os.path.join(self.input_directory, inspect.stack()[2][3] + ".punkt"))
    
    def get_ir_output_file_path(self) -> Path:
        return Path(os.path.join(self.ir_output_directory, inspect.stack()[2][3] + ".ll"))
    
    def get_exe_output_file_path(self) -> Path:
        return Path(os.path.join(self.exe_output_directory, inspect.stack()[2][3]))
    
    def get_actual_output_file_path(self) -> Path:
        return Path(os.path.join(self.actual_output_directory, inspect.stack()[2][3] + ".txt"))

    def get_expected_output_file_path(self) -> Path:
        return Path(os.path.join(self.expected_output_directory, inspect.stack()[2][3] + ".txt"))


    def run_compiler_good(self) -> Path:
        input_file_path = self.get_input_file_path()
        assert input_file_path.exists(), "input file " + str(input_file_path) + " could not be found."

        ir_output_path = self.get_ir_output_file_path()
        subprocess.run([self.compiler_path, input_file_path, "-o", ir_output_path])
        assert ir_output_path.exists(), "IR output was not generated."

        exe_output_path = self.get_exe_output_file_path()
        subprocess.run(["clang-15", ir_output_path, "-o", exe_output_path])
        assert exe_output_path.exists(), "executable file was not generated."

        actual_output_path = self.get_actual_output_file_path()
        with open(actual_output_path, "w") as outfile:
            subprocess.run(exe_output_path, stdout=outfile)
        assert actual_output_path.exists(), "text output file was not generated."

        return actual_output_path


    def output_matches(self, actual_output_path : Path):
        expected_output_path = self.get_expected_output_file_path()
        assert expected_output_path.exists(), "expected output file " + str(expected_output_path) + " could not be found."

        with open(actual_output_path, 'r') as actual_f:
            read_ac = actual_f.read()
        
        with open(expected_output_path, 'r') as expected_f:
            read_ex = expected_f.read()

        col_num = 1
        line_num = 1
        for char_ac, char_ex in zip(read_ac, read_ex):
            assert char_ac == char_ex, "actual output character does not match at line " + str(line_num) + ", column " + str(col_num)
            col_num += 1
            if char_ac == '\n':
                line_num += 1
                col_num = 1


    
    def test_good_empty_main_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_empty_main_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_empty_main_3(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_empty_main_4(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_empty_main_5(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_code_block_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_code_block_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_code_block_3(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_code_block_4(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_code_block_5(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_code_block_6(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_code_block_7(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_code_block_8(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_decl_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_decl_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_decl_3(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_decl_4(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_decl_5(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_decl_6(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_decl_7(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_decl_8(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_decl_9(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_int_ops_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_int_ops_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_int_ops_3(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_int_ops_4(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_int_ops_5(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_int_ops_6(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_int_ops_7(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_str_decl_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_str_decl_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_str_decl_3(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_str_decl_4(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_str_decl_5(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_str_decl_6(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_expression_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_expression_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_identifiers_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_identifiers_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_identifiers_3(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_comment_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_comment_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_comment_3(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_comment_4(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_comment_5(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_comment_6(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_comment_7(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_comment_8(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_comment_9(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_bool_decl_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_bool_decl_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_bool_decl_3(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_bool_decl_4(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_bool_decl_5(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_bool_decl_6(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_char_decl_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_char_decl_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_char_decl_3(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_char_decl_4(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_char_decl_5(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_bool_expr_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_bool_expr_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_bool_expr_3(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_bool_expr_4(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_bool_expr_5(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_bool_expr_6(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_if_stmt_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_if_stmt_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_if_stmt_3(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_if_stmt_4(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_if_stmt_5(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_if_stmt_6(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_if_stmt_7(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_if_stmt_8(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_var_decl_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_var_decl_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_var_decl_3(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_var_decl_4(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_var_decl_5(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_var_decl_6(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_var_decl_7(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_var_decl_8(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_for_stmt_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_for_stmt_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path) 

    def test_good_for_stmt_3(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path) 

    def test_good_for_stmt_4(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_for_stmt_5(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path) 
    
    def test_good_for_stmt_6(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path) 
    
    def test_good_for_stmt_7(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_for_stmt_8(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_for_stmt_9(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_for_stmt_10(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_for_stmt_11(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_for_stmt_12(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_for_stmt_13(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_for_stmt_14(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_for_stmt_15(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_func_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_func_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path) 

    def test_good_func_3(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path) 

    def test_good_func_4(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_func_5(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path) 
    
    def test_good_func_6(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path) 
    
    def test_good_func_7(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_func_8(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_func_9(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_func_10(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
    
    def test_good_func_11(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_func_12(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_func_13(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_func_14(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_func_15(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_lambda_1(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_lambda_2(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_lambda_3(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_lambda_4(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_lambda_5(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)

    def test_good_lambda_6(self):
        actual_output_path = self.run_compiler_good()
        self.output_matches(actual_output_path)
