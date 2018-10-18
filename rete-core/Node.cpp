#include "Node.hpp"
#include "Util.hpp"

namespace rete {

std::string Node::getDOTId() const
{
    return "\"" + util::ptrToStr(this) + "\"";
}

Node::~Node()
{
}

} /* rete */
