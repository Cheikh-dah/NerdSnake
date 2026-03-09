#ifndef TERMS_H
#define TERMS_H

#include <string>
#include <vector>

struct Term {
    std::string word;
    std::string definition;
};

std::vector<Term> GetTerms();

#endif
