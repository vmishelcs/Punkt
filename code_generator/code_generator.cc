#include <fcntl.h>

#include <logging/punkt_logger.h>

#include "code_generation_visitor.h"
#include "code_generator.h"

void CodeGenerator::WriteIR(fs::path input_file_path, std::unique_ptr<ParseNode> decorated_ast,
        fs::path output_file_path)
{
    CodeGenerator code_generator(input_file_path, std::move(decorated_ast), output_file_path);
    code_generator.WriteIRToOutputFile();
}

CodeGenerator::CodeGenerator(fs::path input_file_path, std::unique_ptr<ParseNode> decorated_ast,
        fs::path output_file_path) 
    : input_file_path(input_file_path)
    , output_file_path(output_file_path)
    , decorated_ast(std::move(decorated_ast))
{}

void CodeGenerator::WriteIRToOutputFile() {
    CodeGenerationVisitor code_generation_visitor(input_file_path.string());
    decorated_ast->GenerateCode(code_generation_visitor);

    // Create output file
	int output_fd = open(output_file_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0660);
	if (output_fd == -1) {
		PunktLogger::LogFatal("unable to open output file " + output_file_path.string()
				+ " errno: " + strerror(errno));
	}

    code_generation_visitor.WriteIRToFd(output_fd);

    close(output_fd);
}
