#include <iomanip>

#include "InferTriple.hpp"
#include "../rete-core/Util.hpp"
#include "TriplePart.hpp"

namespace rete {

ToTriplePartConversion::ToTriplePartConversion(
        std::unique_ptr<AccessorBase>&& acc)
    :
        AccessorConversion(std::move(acc))
{
}


void ToTriplePartConversion::convert(const std::string& src, TriplePart& dest) const
{
    std::stringstream ss;
    ss << std::quoted(src);
    dest.value = ss.str();
}

void ToTriplePartConversion::convert(const float& src, TriplePart& dest) const
{
    dest.value = std::to_string(src);
}

void ToTriplePartConversion::convert(const double& src, TriplePart& dest) const
{
    dest.value = std::to_string(src);
}

void ToTriplePartConversion::convert(const int& src, TriplePart& dest) const
{
    dest.value = std::to_string(src);
}

void ToTriplePartConversion::convert(const long& src, TriplePart& dest) const
{
    dest.value = std::to_string(src);
}



InferTriple::InferTriple(
        ToTriplePartConversion&& sub,
        ToTriplePartConversion&& pred,
        ToTriplePartConversion&& obj)
    :
        subject_(std::move(sub)),
        predicate_(std::move(pred)),
        object_(std::move(obj))
{
}


std::string InferTriple::toString() const
{
    std::string s, p, o;
    s = util::dotEscape(subject_.toString());
    p = util::dotEscape(predicate_.toString());
    o = util::dotEscape(object_.toString());

    return "InferTriple (\n" + s + "\n " + p + "\n " + o + ")";
}

void InferTriple::execute(Token::Ptr token, PropagationFlag flag, std::vector<WME::Ptr>& inferred)
{
    if (flag == PropagationFlag::ASSERT || flag == PropagationFlag::UPDATE)
    {
        TriplePart s, p, o;
        subject_.getValue(token, s);
        predicate_.getValue(token, p);
        object_.getValue(token, o);

        auto wme = std::make_shared<Triple>(s.value, p.value, o.value);

        inferred.push_back(wme);
    }
}


} /* rete */
