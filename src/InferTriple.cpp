#include "../include/InferTriple.hpp"
#include "../include/Util.hpp"


namespace rete {

InferTriple::ConstructHelper::ConstructHelper(const std::string& predefined)
    : isPredefined_(true), string_(predefined), tokenOffset_(0), field_(Triple::SUBJECT)
{
}

InferTriple::ConstructHelper::ConstructHelper(const char* predefined)
    : isPredefined_(true), string_(predefined), tokenOffset_(0), field_(Triple::SUBJECT)
{
}

InferTriple::ConstructHelper::ConstructHelper(int offset, Triple::Field field)
    : isPredefined_(false), string_(""), tokenOffset_(offset), field_(field)
{
}

InferTriple::ConstructHelper::ConstructHelper()
    : isPredefined_(true), string_(""), tokenOffset_(0), field_(Triple::SUBJECT)
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

void InferTriple::ConstructHelper::init(int offset, Triple::Field field)
{
    isPredefined_ = false;
    tokenOffset_ = offset;
    field_ = field;
}

std::string InferTriple::ConstructHelper::constructFrom(Token::Ptr token) const
{
    if (isPredefined_) return string_;

    int cnt = 0;
    Token::Ptr sel = token;
    while (cnt < tokenOffset_ && sel)
    {
        sel = sel->parent;
        ++cnt;
    }

    if (!sel || !sel->wme) throw std::exception(); // ill formed network! no wme
    auto triple = std::dynamic_pointer_cast<Triple>(sel->wme);
    if (!triple) throw std::exception(); // ill formed network! wme not a triple

    return triple->getField(field_);
}

std::string InferTriple::ConstructHelper::getName() const
{
    if (isPredefined_) return string_;
    return std::to_string(tokenOffset_) + "." + Triple::fieldName(field_);
}


InferTriple::InferTriple(const ConstructHelper& sub, const ConstructHelper& pred, const ConstructHelper& obj)
    : subject_(sub), predicate_(pred), object_(obj)
{
}

std::string InferTriple::getName() const
{
    std::string s, p, o;
    s = util::dotEscape(subject_.getName());
    p = util::dotEscape(predicate_.getName());
    o = util::dotEscape(object_.getName());

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
