#include "InferTriple.hpp"
#include "../rete-core/Util.hpp"


namespace rete {

InferTriple::ConstructHelper::ConstructHelper(const std::string& predefined)
    : isPredefined_(true), string_(predefined), accessor_(nullptr)
{
}

InferTriple::ConstructHelper::ConstructHelper(const char* predefined)
    : isPredefined_(true), string_(predefined), accessor_(nullptr)
{
}

InferTriple::ConstructHelper::ConstructHelper(std::unique_ptr<Accessor> accessor)
    : isPredefined_(false), string_("")
{
    if (!accessor->canAs<StringAccessor>())
    {
        throw std::exception(); // see init(...) method
    }
    accessor_ = std::move(accessor);
}

InferTriple::ConstructHelper::ConstructHelper()
    : isPredefined_(true), string_(""), accessor_(nullptr)
{
}

InferTriple::ConstructHelper::ConstructHelper(InferTriple::ConstructHelper&& other)
    : isPredefined_(other.isPredefined_),
      string_(std::move(other.string_)),
      accessor_(std::move(other.accessor_))
{
}

void InferTriple::ConstructHelper::init(const std::string& predefined)
{
    isPredefined_ = true;
    string_ = predefined;
}

void InferTriple::ConstructHelper::init(const char* predefined)
{
    isPredefined_ = true;
    string_ = std::string(predefined);
}

void InferTriple::ConstructHelper::init(std::unique_ptr<Accessor> accessor)
{
    if (!accessor->canAs<StringAccessor>())
    {
        throw std::exception(); // InferTriple only works with strings
        // TODO: Implement some kind of "NumberAccessor" that works with float, int, ... but also allows conversion to string.
    }

    isPredefined_ = false;
    accessor_ = std::move(accessor);
}

std::string InferTriple::ConstructHelper::constructFrom(Token::Ptr token) const
{
    if (isPredefined_) return string_;
    // NOTE: Assumption is that accessor_->canAs<StringAccessor>()! This is checked in the ctor and init() method.
    return accessor_->as<StringAccessor>()->getString(token);
}

std::string InferTriple::ConstructHelper::toString() const
{
    if (isPredefined_) return string_;
    return accessor_->toString();
}


InferTriple::InferTriple(ConstructHelper&& sub, ConstructHelper&& pred, ConstructHelper&& obj)
    : subject_(std::move(sub)), predicate_(std::move(pred)), object_(std::move(obj))
{
}

std::string InferTriple::toString() const
{
    std::string s, p, o;
    s = util::dotEscape(subject_.toString());
    p = util::dotEscape(predicate_.toString());
    o = util::dotEscape(object_.toString());

    return "InferTriple (" + s + " " + p + " " + o + ")";
}


void InferTriple::execute(Token::Ptr token, PropagationFlag flag, std::vector<WME::Ptr>& inferred)
{
    if (flag == PropagationFlag::ASSERT)
    {
        auto wme = std::make_shared<Triple>(
            subject_.constructFrom(token),
            predicate_.constructFrom(token),
            object_.constructFrom(token)
        );

        inferred.push_back(wme);
    }
}


} /* rete */
