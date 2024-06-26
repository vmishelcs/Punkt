from tests.compiler.punkt_compiler_test_helper import PunktCompilerTestHelper


class TestWhileStatement(object):
    """ While statement test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompilerTestHelper()

    def test_bad_while_stmt_1(self):
        try:
            self._compile_helper.compile_with_failure()
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_bad_while_stmt_2(self):
        try:
            self._compile_helper.compile_with_failure()
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_bad_while_stmt_3(self):
        try:
            self._compile_helper.compile_with_failure()
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_while_stmt_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_while_stmt_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)
