import inspect
import os
import subprocess
import filecmp

from pathlib import Path

class TestParser(object):
    punkt_dir = Path
    executable_path = Path
    input_directory = Path
    actual_output_directory = Path
    expected_output_directory = Path


    def setup_class(self):
        self.punkt_dir = Path(__file__).parent.parent.parent.resolve()

        self.executable_path = Path(os.path.join(self.punkt_dir, "build", "applications", "ast_xml_printer"))
        assert self.executable_path.exists(), "executable file could not be found; make sure ast_xml_printer has been built."

        self.input_directory = Path(os.path.join(self.punkt_dir, "tests", "test_input_files", "good"))
        assert self.input_directory.exists(), "input directory could not be found."

        self.actual_output_directory = Path(os.path.join(self.punkt_dir, "tests", "test_output_files", "actual", "parser"))
        self.actual_output_directory.mkdir(parents=True, exist_ok=True)

        self.expected_output_directory = Path(os.path.join(self.punkt_dir, "tests", "test_output_files", "expected", "parser"))
        assert self.expected_output_directory.exists(), "expected output directory could not be found."

    
    def get_input_file_path(self) -> Path:
        return Path(os.path.join(self.input_directory, Path(inspect.stack()[2][3] + ".punkt")))
    

    def get_actual_output_file_path(self) -> Path:
        return Path(os.path.join(self.actual_output_directory, Path(inspect.stack()[2][3] + ".xml")))
    

    def get_expected_output_file_path(self) -> Path:
        return Path(os.path.join(self.expected_output_directory, Path(inspect.stack()[2][3] + ".xml")))


    def run_ast_printer(self) -> Path:
        input_file_path = self.get_input_file_path()
        assert input_file_path.exists(), "input file " + str(input_file_path) + " could not be found."

        output_file_path = self.get_actual_output_file_path()

        subprocess.run([self.executable_path, input_file_path, output_file_path])

        assert output_file_path.exists(), "no output file was generated."

        return output_file_path
    

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
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)
    
    def test_good_empty_main_2(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_empty_main_3(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_empty_main_4(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_empty_main_5(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_code_block_1(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_code_block_2(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_code_block_3(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_code_block_4(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_code_block_5(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_code_block_6(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_code_block_7(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_code_block_8(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_decl_1(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)
    
    def test_good_decl_2(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_decl_3(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_decl_4(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_decl_5(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_decl_6(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_decl_7(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_decl_8(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)
    
    def test_good_int_ops_1(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_int_ops_2(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_int_ops_3(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_int_ops_4(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_int_ops_5(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_int_ops_6(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_int_ops_7(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_str_decl_1(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)
    
    def test_good_str_decl_2(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)
    
    def test_good_str_decl_3(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)
    
    def test_good_str_decl_4(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)
    
    def test_good_str_decl_5(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_str_decl_6(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_expression_1(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_expression_2(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_identifiers_1(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_identifiers_2(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    def test_good_identifiers_3(self):
        actual_output_path = self.run_ast_printer()
        self.output_matches(actual_output_path)

    
