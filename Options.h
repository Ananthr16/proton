#ifndef OPTIONIS_H
#define OPTIONIS_H

#include <string> 
#include <map>

// Enum for properties
enum class PropertyType {
    Label,
    UnreachCall,
    Termination,
    Overflow,
    Memsafety,
    Memcleanup,
    Unknown
};

class Options { 
private:
    // Singleton instance
    static Options* instance;

    // Private constructor
    Options();

    // Enum based properties
    PropertyType prop;

    // Metadata fields
    std::string label;
    std::string entry;
    std::string failFunction;

    // Flags
    bool f_label = false;
    bool f_unreach_call = false;
    bool f_termination = false;
    bool f_overflow = false;
    bool f_memsafety = false;
    bool f_memcleanup = false;

    // Maps string keys to flags used in command-line options
    std::map<std::string, std::string> optionMap;

public:

    static Options* getInstance();

    // Setters
    void setProp(PropertyType p);
    void setLabel(const std::string& l);
    void setEntry(const std::string& e);
    void setFailFunction(const std::string& f);

  
    PropertyType getProp() const;

    // Getters
    const std::string& getLabel() const;
    const std::string& getEntry() const;
    const std::string& getFailFunction() const;


    std::string getOptionFlags() const;
//optional
    static std::string propToString(PropertyType p);
};
#endif
