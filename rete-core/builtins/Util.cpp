#include "Util.hpp"
#include "../../rete-core/TupleWME.hpp"
#include <map>
#include <iostream>
#include <sstream>

namespace rete {
namespace builtin {


Compare::Compare(Mode mode, std::unique_ptr<Accessor> left, std::unique_ptr<Accessor> right)
    : Builtin(Compare::ModeName(mode)),
      left_(std::move(left)), right_(std::move(right)), mode_(mode)
{
    if (left_->canAs<NumberAccessor>() && right_->canAs<NumberAccessor>())
    {
        compareNumbers_ = true;
    }
    else
    {
        compareNumbers_ = false;
    }
}

std::string Compare::getDOTAttr() const
{
    std::string l = left_->toString() + " " + Compare::ModeName(mode_) + " " + right_->toString();
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
    if (!o) return false;

    if (o->left_ == this->left_ &&
        o->right_ == this->right_ &&
        o->mode_ == this->mode_)
    {
        return true;
    }
    return false;
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
        auto l = left_->as<NumberAccessor>();
        auto r = right_->as<NumberAccessor>();

        if (compare(mode_, l->getFloat(token), r->getFloat(token)))
        {
            return std::make_shared<EmptyWME>();
        }
    }
    else
    {
        auto l = left_->as<StringAccessor>();
        auto r = right_->as<StringAccessor>();

        if (compare(mode_, l->getString(token), r->getString(token)))
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
        std::cout << val->getString(token) << ", ";
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
        if (!(*values_[i] == *o->values_[i])) return false;
    }

    return true;
}

std::string Print::getDOTAttr() const
{
    std::stringstream s;
    s << "[label=\"print ";
    for (auto& val : values_)
    {
        s << util::dotEscape(val->toString()) << ", ";
    }
    s << "\b\b"
      << "\"]";
    return s.str();
}

void Print::add(std::unique_ptr<StringAccessor> accessor)
{
    values_.push_back(std::move(accessor));
}

void Print::add(const std::string& str)
{
    values_.push_back(std::unique_ptr<ConstantStringAccessor>(new ConstantStringAccessor(str)));
    (*values_.rbegin())->index() = 0;
}

// -------------------------
// PrintEffect
// -------------------------
PrintEffect::PrintEffect()
    : Production(0, "print")
{
}

void PrintEffect::add(std::unique_ptr<StringAccessor> accessor)
{
    values_.push_back(std::move(accessor));
}

void PrintEffect::add(const std::string& str)
{
    values_.push_back(std::unique_ptr<ConstantStringAccessor>(new ConstantStringAccessor(str)));
    (*values_.rbegin())->index() = 0;
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
        std::cout << val->getString(token) << ", ";
    }
    std::cout << "\b\b  \b\b" << std::endl;
}

} /* builtin */
} /* rete */
