from tests.compiler.punkt_compiler_test_helper import PunktCompilerTestHelper


class TestIfStatement(object):
    """ If statement test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompilerTestHelper()

    def test_bad_if_stmt_1(self):
        try:
            self._compile_helper.compile_with_failure()
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_bad_if_stmt_2(self):
        try:
            self._compile_helper.compile_with_failure()
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_bad_if_stmt_3(self):
        try:
            self._compile_helper.compile_with_failure()
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_bad_if_stmt_4(self):
        try:
            self._compile_helper.compile_with_failure()
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_bad_if_stmt_5(self):
        try:
            self._compile_helper.compile_with_failure()
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_bad_if_stmt_6(self):
        try:
            self._compile_helper.compile_with_failure()
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_bad_if_stmt_7(self):
        try:
            self._compile_helper.compile_with_failure()
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_if_stmt_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_if_stmt_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_if_stmt_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_if_stmt_4(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_if_stmt_5(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_if_stmt_6(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_if_stmt_7(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_if_stmt_8(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_if_stmt_9(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_if_stmt_10(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_if_stmt_11(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_if_stmt_12(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_if_stmt_13(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)
