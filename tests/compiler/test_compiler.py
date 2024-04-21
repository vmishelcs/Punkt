from punkt_compile_test_helper import PunktCompileTestHelper


class TestEmptyMain(object):
    """ Empty 'main' function test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompileTestHelper()

    def test_good_empty_main_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_empty_main_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_empty_main_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_empty_main_4(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_empty_main_5(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)


class TestDeclarationStmt(object):
    """ General declaration statement test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompileTestHelper()

    def test_good_decl_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_decl_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_decl_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_decl_4(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_decl_5(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_decl_6(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_decl_7(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_decl_8(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_decl_9(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_char_decl_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_char_decl_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_char_decl_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_char_decl_4(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_char_decl_5(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_bool_decl_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_bool_decl_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_bool_decl_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_bool_decl_4(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_bool_decl_5(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_bool_decl_6(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)


class TestVarDecl(object):
    """ Mutable variable test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompileTestHelper()

    def test_good_var_decl_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_var_decl_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_var_decl_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_var_decl_4(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_var_decl_5(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_var_decl_6(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_var_decl_7(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_var_decl_8(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)


class TestIdentifiers(object):
    """ Identifiers test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompileTestHelper()

    def test_good_identifiers_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_identifiers_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_identifiers_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)


class TestExpressions(object):
    """ General expression test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompileTestHelper()

    def test_good_expression_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_expression_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_bool_expr_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_bool_expr_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_bool_expr_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_bool_expr_4(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_bool_expr_5(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_bool_expr_6(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)


class TestCodeBlock(object):
    """ Code block test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompileTestHelper()

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


class TestIntOps(object):
    """ Integer operations test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompileTestHelper()

    def test_good_int_ops_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_int_ops_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_int_ops_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_int_ops_4(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_int_ops_5(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_int_ops_6(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_int_ops_7(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)


class TestStrDecl(object):
    """ String declaration test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompileTestHelper()

    def test_good_str_decl_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_str_decl_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_str_decl_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_str_decl_4(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_str_decl_5(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_str_decl_6(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)


class TestComments(object):
    """ Comment test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompileTestHelper()

    def test_good_comment_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_comment_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_comment_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_comment_4(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_comment_5(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_comment_6(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_comment_7(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_comment_8(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_comment_9(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)


class TestFunction(object):
    """ General function test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompileTestHelper()

    def test_good_func_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_func_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_func_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_func_4(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_func_5(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_func_6(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_func_7(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_func_8(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_func_9(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_func_10(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_func_11(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_func_12(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_func_13(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_func_14(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_func_15(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_func_16(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)


class TestLambda(object):
    """ Lambda test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompileTestHelper()

    def test_good_lambda_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_lambda_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_lambda_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_lambda_4(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_lambda_5(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_lambda_6(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)


class TestIfStmt(object):
    """ If statement test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompileTestHelper()

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


class TestForStmt(object):
    """ For statement test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompileTestHelper()

    def test_good_for_stmt_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_for_stmt_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_for_stmt_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_for_stmt_4(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_for_stmt_5(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_for_stmt_6(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_for_stmt_7(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_for_stmt_8(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_for_stmt_9(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_for_stmt_10(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_for_stmt_11(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_for_stmt_12(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_for_stmt_13(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_for_stmt_14(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_for_stmt_15(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)


class TestReturnStmt(object):
    """ Return statement test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompileTestHelper()

    def test_good_return_stmt_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_return_stmt_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_return_stmt_3(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_return_stmt_4(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_return_stmt_5(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_return_stmt_6(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_return_stmt_7(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_return_stmt_8(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_return_stmt_9(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)


class TestCallStmt(object):
    """ Call statement test suite. """

    def setup_class(self):
        self._compile_helper = PunktCompileTestHelper()

    def test_good_call_stmt_1(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)

    def test_good_call_stmt_2(self):
        try:
            actual_output_path = self._compile_helper.compile_with_success()
            self._compile_helper.verify_actual_output(actual_output_path)
        except RuntimeError as ex:
            assert False, "Exception was raised: " + repr(ex)
