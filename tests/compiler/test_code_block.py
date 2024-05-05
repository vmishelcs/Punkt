from tests.compiler.punkt_compiler_test_helper import PunktCompilerTestHelper


class TestCodeBlock(object):
    """ Code block test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompilerTestHelper()

    def test_good_code_block_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_code_block_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_code_block_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_code_block_4(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_code_block_5(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_code_block_6(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_code_block_7(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_code_block_8(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)
