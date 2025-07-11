#include "Options.h"

Options* Options::instance = nullptr;

//fLabel 
Options::Options() : prop(PropertyType::Unknown) {
    optionMap["label"]        = "--error-label";
    optionMap["unreach_call"] = "";
    optionMap["termination"]  = "";
    optionMap["overflow"]     = "--signed-overflow-check --no-assertions";
    optionMap["memsafety"]    = "--pointer-check --memory-leak-check --bounds-check --no-assertions";
    optionMap["memcleanup"]   = "--pointer-check --memory-leak-check --memory-cleanup-check --bounds-check --no-assertions";

    
    f_label = f_unreach_call = f_termination = f_overflow = f_memsafety = f_memcleanup = false;
}


Options* Options::getInstance() {
    if (instance == nullptr) {
        instance = new Options();
    }
    return instance;
}


void Options::setProp(PropertyType p) {
    prop = p;

    
    f_label = f_unreach_call = f_termination = f_overflow = f_memsafety = f_memcleanup = false;

    // Setting corresponding flags
    switch (p) {
        case PropertyType::Label:        f_label = true; break;
        case PropertyType::UnreachCall:  f_unreach_call = true; break;
        case PropertyType::Termination:  f_termination = true; break;
        case PropertyType::Overflow:     f_overflow = true; break;
        case PropertyType::Memsafety:    f_memsafety = true; break;
        case PropertyType::Memcleanup:   f_memcleanup = true; break;
        default: break;
    }
}

void Options::setLabel(const std::string& l) {
    label = l;
}

void Options::setEntry(const std::string& e) {
    entry = e;
}

void Options::setFailFunction(const std::string& f) {
    failFunction = f;
}


PropertyType Options::getProp() const {
    return prop;
}

const std::string& Options::getLabel() const {
    return label;
}

const std::string& Options::getEntry() const {
    return entry;
}

const std::string& Options::getFailFunction() const {
    return failFunction;
}
// conversion ot string for quick checks
std::string Options::propToString(PropertyType p) {
    switch (p) {
        case PropertyType::Label:        return "label";
        case PropertyType::UnreachCall:  return "unreach_call";
        case PropertyType::Termination:  return "termination";
        case PropertyType::Overflow:     return "overflow";
        case PropertyType::Memsafety:    return "memsafety";
        case PropertyType::Memcleanup:   return "memcleanup";
        default:                         return "";
    }
}


std::string Options::getOptionFlags() const {
    if (prop == PropertyType::Label) {
        return optionMap.at("label") + " " + label;
    }

    std::string key = propToString(prop);
    if (optionMap.count(key)) {
        return optionMap.at(key);
    }

    return "";
}


