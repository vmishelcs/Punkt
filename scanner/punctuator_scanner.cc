#include "punctuator_scanner.h"

std::unique_ptr<PunctuatorToken> PunctuatorScanner::Scan(
    LocatedChar first_char,
    std::unique_ptr<LocatedCharStream>& input_stream
) {
    PunctuatorScanner punctuator_scanner(first_char, input_stream);
    return punctuator_scanner.ScanPunctuator();
}

std::unique_ptr<PunctuatorToken> PunctuatorScanner::ScanPunctuator() {
    std::string scanned_string = scanned.AsString();
    int num_punctuators_with_prefix = Punctuator::PunctuatorsWithPrefix(scanned_string);

    if (num_punctuators_with_prefix == 1 && Punctuator::IsPunctuator(scanned_string)) {
        Punctuator punctuator(scanned_string);
        return std::make_unique<PunctuatorToken>(
            scanned_string,
            scanned.GetLocation(),
            std::move(punctuator)
        );
    }

    if (num_punctuators_with_prefix == 0 || !input_stream->HasNext()) {
        FindLatestValidPunctuator();
        scanned_string = scanned.AsString();
        Punctuator punctuator(scanned_string);
        return std::make_unique<PunctuatorToken>(
            scanned_string,
            scanned.GetLocation(),
            std::move(punctuator)
        );
    }

    scanned.Append(input_stream->Next());
    return ScanPunctuator();
}

void PunctuatorScanner::FindLatestValidPunctuator() {
    while (!Punctuator::IsPunctuator(scanned.AsString()) && !scanned.IsEmpty()) {
        LocatedChar ch = scanned.PopBack();
        input_stream->PutBack(ch);
    }
}
