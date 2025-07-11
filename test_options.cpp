#include <iostream>
#include "Options.h"

int main() {
    Options* opts = Options::getInstance();

    opts->setProp("label");
    opts->setLabel("main");

    std::cout << "Prop: " << opts->getProp() << std::endl;
    std::cout << "Label: " << opts->getLabel() << std::endl;

    std::cout << "Option Flags: " << opts->getOptionFlags() << std::endl;

    opts->setProp("overflow");
    std::cout << "Option Flags (overflow): " << opts->getOptionFlags() << std::endl;

    return 0;
}