import inspect
import os
import subprocess
import filecmp

from pathlib import Path


class PunktCompilerTestHelper(object):
    """ A class to mimic Punkt compilation and executable launching. """

    def __init__(self):
        # Set project directory member.
        self._punkt_project_dir = Path(__file__).parent.parent.parent.resolve()

        # Locate the compiler executable.
        self._compiler_path = Path(
            os.path.join(
                self._punkt_project_dir, "build", "applications", "punkt_compiler"
            )
        )
        assert self._compiler_path.exists(
        ), "executable file could not be found; make sure punkt_compiler has been built."

        # Locate input directory.
        self._input_directory = Path(
            os.path.join(self._punkt_project_dir, "tests",
                         "test_input_files", "good")
        )
        assert self._input_directory.exists(), "input directory could not be found."

        # Locate or create the IR output directory.
        self._ir_output_directory = Path(
            os.path.join(
                self._punkt_project_dir, "tests", "test_output_files", "ir_output"
            )
        )
        # Remove any old executable output.
        # for file in os.scandir(self._ir_output_directory):
        #     os.unlink(file)
        self._ir_output_directory.mkdir(parents=True, exist_ok=True)

        # Locate or create the executable output directory.
        self._exe_output_directory = Path(
            os.path.join(
                self._punkt_project_dir, "tests", "test_output_files", "exe_output"
            )
        )
        self._exe_output_directory.mkdir(parents=True, exist_ok=True)
        # Remove any old executable output.
        # for file in os.scandir(self._exe_output_directory):
        #     os.unlink(file)

        # Locate or create the program output directory.
        self._actual_output_directory = Path(
            os.path.join(
                self._punkt_project_dir,
                "tests",
                "test_output_files",
                "actual",
            )
        )
        self._actual_output_directory.mkdir(parents=True, exist_ok=True)
        # Remove any old program output.
        # for file in os.scandir(self._actual_output_directory):
        #     os.unlink(file)

        # Locate the expected output directory.
        self._expected_output_directory = Path(
            os.path.join(
                self._punkt_project_dir,
                "tests",
                "test_output_files",
                "expected"
            )
        )
        assert (
            self._expected_output_directory.exists()
        ), "expected output directory could not be found."

    def _get_caller_name(self) -> str:
        return inspect.stack()[2][3]

    def _get_input_path(self) -> Path:
        """ Infer Punkt source file input path based on the name of the calling
            function/method. """

        return Path(
            os.path.join(self._input_directory,
                         inspect.stack()[2][3] + ".punkt")
        )

    def _get_ir_output_path(self) -> Path:
        """ Infer LLVM IR output file path based on the name of the calling
            function/method. """

        return Path(
            os.path.join(self._ir_output_directory,
                         inspect.stack()[2][3] + ".ll")
        )

    def _get_exe_output_path(self) -> Path:
        """ Infer executable output file path based on the name of the calling
            function/method. """

        return Path(os.path.join(self._exe_output_directory, inspect.stack()[2][3]))

    def _get_program_actual_output_path(self) -> Path:
        """ Infer actual program output file path based on the name of the
            calling function/method. """

        return Path(
            os.path.join(self._actual_output_directory,
                         inspect.stack()[2][3] + ".txt")
        )

    def _get_program_expected_output_path(self) -> Path:
        """ Infer expected program output file path based on the name of the
            calling function/method. """

        return Path(
            os.path.join(self._expected_output_directory,
                         inspect.stack()[2][3] + ".txt")
        )

    def compile_with_success(self) -> Path:
        """ Compile a Punkt source file whose name is specified by the name of
            the calling function/method.

            The compilation sequence used by this method is as follows.
            1. Generate LLVM IR output.
            2. Compile the generated LLVM IR output using clang-18 to an
            executable.
            3. Run the executable and capture its output to a text file.

            If any of the steps above fail, a `RuntimeError` is raised.

            This method returns a `Path` object to a text file that contains the
            executable output. """

        input_file_path = self._get_input_path()
        if not input_file_path.exists():
            raise RuntimeError(
                "input file " + str(input_file_path) + " could not be found."
            )

        ir_output_path = self._get_ir_output_path()
        # Remove old IR output.
        if ir_output_path.exists():
            os.unlink(ir_output_path)
        subprocess.run([self._compiler_path, input_file_path,
                        "-o", ir_output_path], text=True)
        if not ir_output_path.exists():
            raise RuntimeError(
                "IR output to " + str(ir_output_path) + " was not generated."
            )

        exe_output_path = self._get_exe_output_path()
        # Remove old executable output.
        if exe_output_path.exists():
            os.unlink(exe_output_path)
        subprocess.run(["clang-18", ir_output_path, "-o",
                        exe_output_path], text=True)
        if not exe_output_path.exists():
            raise RuntimeError(
                "Program executable file " +
                str(exe_output_path) + " was not generated."
            )

        actual_output_path = self._get_program_actual_output_path()
        # Remove old program output.
        if actual_output_path.exists():
            os.unlink(actual_output_path)
        with open(actual_output_path, "w") as outfile:
            subprocess.run(exe_output_path, stdout=outfile)
        if not actual_output_path.exists():
            raise RuntimeError(
                "Program output file " +
                str(actual_output_path) + " was not generated."
            )

        return actual_output_path

    def verify_actual_output(self, actual_output_path):
        """ Verifies that the provided program output matches with the
            expected program output. Raises `RuntimeError` if not. """

        expected_output_path = self._get_program_expected_output_path()
        if not expected_output_path.exists():
            raise RuntimeError(
                "Expected program output file " +
                str(expected_output_path) + " was not found."
            )

        # Compare actual output vs expected output.
        cmp = filecmp.cmp(actual_output_path, expected_output_path)
        if not cmp:
            raise RuntimeError(
                "Actual program output does not match expected program output for " +
                self._get_caller_name() + "."
            )
