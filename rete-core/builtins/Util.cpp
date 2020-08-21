#include "Util.hpp"
#include "../../rete-core/TupleWME.hpp"
#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace rete {
namespace builtin {


Compare::Compare(Mode mode,
        PersistentInterpretation<float>&& left,
        PersistentInterpretation<float>&& right)
    :
        Builtin(Compare::ModeName(mode)),
        leftNum_(std::move(left)),
        rightNum_(std::move(right)),
        mode_(mode),
        compareNumbers_(true)
{
}

Compare::Compare(Mode mode,
        PersistentInterpretation<std::string>&& left,
        PersistentInterpretation<std::string>&& right)
    :
        Builtin(Compare::ModeName(mode)),
        leftStr_(std::move(left)),
        rightStr_(std::move(right)),
        mode_(mode),
        compareNumbers_(false)
{
}


std::string Compare::getDOTAttr() const
{
    std::string left = (compareNumbers_ ? leftNum_.accessor->toString()
                                        : leftStr_.accessor->toString());
    std::string right = (compareNumbers_ ? rightNum_.accessor->toString()
                                        : rightStr_.accessor->toString());

    std::string l = left + " " + Compare::ModeName(mode_) + " " + right;
    return "[label=\"" + util::dotEscape(l) + "\"]";
}

std::string Compare::ModeName(Mode m)
{
    static std::map<Mode, std::string> names_ =
    {
        {Mode::LT, "LT"},
        {Mode::LE, "LE"},
        {Mode::EQ, "EQ"},
        {Mode::NEQ,"NEQ"},
        {Mode::GE, "GE"},
        {Mode::GT, "GT"}
    };

    return names_[m];
}

bool Compare::operator == (const BetaNode& other) const
{
    auto o = dynamic_cast<const Compare*>(&other);
    if (!o)
    {
        return false;
    }
    else if ((o->compareNumbers_ != this->compareNumbers_) ||
             (o->mode_ != this->mode_))
    {
        return false;
    }
    else if (o->compareNumbers_ && this->compareNumbers_)
    {
        return (*o->leftNum_.accessor == *this->leftNum_.accessor) &&
               (*o->rightNum_.accessor == *this->rightNum_.accessor);
    }
    else if (!o->compareNumbers_ && !this->compareNumbers_)
    {
        return (*o->leftStr_.accessor == *this->leftStr_.accessor) &&
               (*o->rightStr_.accessor == *this->rightStr_.accessor);
    }
    else
    {
        return false;
    }
}


template <class T>
bool compare(Compare::Mode m, const T& l, const T& r)
{
    if (m == Compare::Mode::EQ) return l == r;
    else if (m == Compare::Mode::GE) return l >= r;
    else if (m == Compare::Mode::GT) return l > r;
    else if (m == Compare::Mode::LE) return l <= r;
    else if (m == Compare::Mode::LT) return l < r;
    else /*if (m == Compare::Mode::NEQ)*/ return l != r;
}

WME::Ptr Compare::process(Token::Ptr token)
{
    if (compareNumbers_)
    {
        float l, r;
        leftNum_.interpretation->getValue(token, l);
        rightNum_.interpretation->getValue(token, r);

        if (compare(mode_, l, r))
        {
            return std::make_shared<EmptyWME>();
        }
    }
    else
    {
        std::string l, r;
        leftStr_.interpretation->getValue(token, l);
        rightStr_.interpretation->getValue(token, r);

        if (compare(mode_, l, r))
        {
            return std::make_shared<EmptyWME>();
        }
    }

    return nullptr;
}


// ---------------------------------------------------
// ---------------------------------------------------

Print::Print()
    : Builtin("print")
{
}


WME::Ptr Print::process(Token::Ptr token)
{
    for (auto& val : values_)
    {
        std::string str;
        val.interpretation->getValue(token, str);
        std::cout << str << ", ";
    }
    std::cout << "\b\b  \b\b" << std::endl;

    return std::make_shared<EmptyWME>();
}

bool Print::operator == (const BetaNode& other) const
{
    auto o = dynamic_cast<const Print*>(&other);
    if (!o) return false;
    if (o->values_.size() != this->values_.size()) return false;

    // equal if accessors are pairwise equal
    for (size_t i = 0; i < values_.size(); i++)
    {
        if (!(*values_[i].accessor == *o->values_[i].accessor)) return false;
    }

    return true;
}

std::string Print::getDOTAttr() const
{
    std::stringstream s;
    s << "[label=\"print ";
    for (auto& val : values_)
    {
        s << util::dotEscape(val.accessor->toString()) << ", ";
    }
    s << "\b\b"
      << "\"]";
    return s.str();
}

void Print::add(PersistentInterpretation<std::string>&& accessor)
{
    values_.push_back(std::move(accessor));
}

void Print::add(const std::string& str)
{
    ConstantAccessor<std::string> acc(str);
    acc.index() = 0;

    values_.push_back(acc.getInterpretation<std::string>()->makePersistent());
}

// ---------------------------------------------------
// ---------------------------------------------------
PrintGroup::PrintGroup()
    : Builtin("printGroup")
{
}

WME::Ptr PrintGroup::process(Token::Ptr token)
{
    if (groupAccessors_.empty()) return nullptr;

    TokenGroup::Ptr group;
    groupAccessors_[0]->getInterpretation<TokenGroup::Ptr>()->getValue(token, group);

    // prepare interpretations to not having to search for them for every group entry
    std::vector<const Interpretation<std::string>*> strGetter;
    for (auto& acc : groupAccessors_)
    {
        auto i = acc->childAccessor()->getInterpretation<std::string>();
        strGetter.push_back(i);
    }

    // iterate the group
    std::cout << "TokenGroup[" << group->token_.size() << "]:" << std::endl;
    for (auto token : group->token_)
    {
        std::cout << std::setw(20);
        for (auto i : strGetter)
        {
            std::string str;
            i->getValue(token, str);
            std::cout << str;
        }
        std::cout << std::endl;
    }
    std::cout << "End of group." << std::endl;

    return std::make_shared<EmptyWME>();
}

void PrintGroup::add(std::unique_ptr<TokenGroupAccessor> acc)
{
    groupAccessors_.push_back(std::move(acc));
}

bool PrintGroup::operator == (const BetaNode& other) const
{
    auto o = dynamic_cast<const PrintGroup*>(&other);
    if (!o || o->groupAccessors_.size() != this->groupAccessors_.size())
    {
        return false;
    }
    else
    {
        for (size_t i = 0; i < groupAccessors_.size(); i++)
        {
            if (!(*groupAccessors_[i] == *o->groupAccessors_[i]))
                return false;
        }
        return true;
    }
}

std::string PrintGroup::getDOTAttr() const
{
    std::stringstream s;
    s << "[label=\"print group ";
    for (auto& acc : groupAccessors_)
    {
        s << util::dotEscape(acc->toString()) << ", ";
    }
    s << "\b\b" << "\"]";
    return s.str();
}

// -------------------------
// PrintEffect
// -------------------------
PrintEffect::PrintEffect()
    : Production(0, "print")
{
}

void PrintEffect::add(PersistentInterpretation<std::string>&& accessor)
{
    values_.push_back(std::move(accessor));
}

void PrintEffect::add(const std::string& str)
{
    ConstantAccessor<std::string> acc(str);
    acc.index() = 0;

    values_.push_back(acc.getInterpretation<std::string>()->makePersistent());
}

void PrintEffect::execute(Token::Ptr token, PropagationFlag flag, std::vector<WME::Ptr>&)
{
    switch (flag) {
        case PropagationFlag::ASSERT:
            std::cout << "ASSERT: "; break;
        case PropagationFlag::RETRACT:
            std::cout << "RETRACT: "; break;
        case PropagationFlag::UPDATE:
            std::cout << "UPDATE: "; break;
    }

    for (auto& val : values_)
    {
        std::string str;
        val.interpretation->getValue(token, str);
        std::cout << str << ", ";
    }
    std::cout << "\b\b  \b\b" << std::endl;
}

} /* builtin */
} /* rete */
