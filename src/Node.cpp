#include "../include/Node.hpp"
#include "../include/Util.hpp"

namespace rete {

std::string Node::getDOTId() const
{
    return "\"" + util::ptrToStr(this) + "\"";
}

} /* rete */
