#include "c74_min.h"
#include <cmath>


using namespace c74::min;


bool only_ints(atoms const &args) {
    bool ints_only = true;
    for (int i = 0; i < args.size(); i++)
        try
        {
            float num = std::stof(args[i]);
            if (std::floor(num) != num)
            {
                ints_only = false;
                break;
            }
        }
        catch (...)
        {
            ints_only = false;
        }

    return ints_only;
}
