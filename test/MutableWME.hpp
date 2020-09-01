#include "../rete-core/Accessors.hpp"
#include "../rete-core/WME.hpp"
#include "../rete-core/AlphaNode.hpp"
#include "../rete-reasoner/NodeBuilder.hpp"
#include "../rete-reasoner/Exceptions.hpp"

using namespace rete;

/**
    A very simple mutable WME.
*/
class MutableWME : public WME {
    static const std::string type_;
public:
    using Ptr = std::shared_ptr<MutableWME>;
    std::string value_;

    std::string toString() const override { return "Mutable: " + value_; }

    const std::string& type() const override
    {
        return type_;
    }

    bool operator < (const WME& other) const override
    {
        if (type() != other.type()) return type() < other.type();

        auto o = dynamic_cast<const MutableWME*>(&other);
        if (o)
        {
            return this < o;
        }
        throw std::exception();
    }
};
const std::string MutableWME::type_("MutableWME");


/**
    An accessor for the MutableWME
*/
class MutableWMEAccessor : public Accessor<MutableWME, std::string> {
    bool equals(const AccessorBase& other) const override
    {
        return nullptr != dynamic_cast<const MutableWMEAccessor*>(&other);
    }

    void getValue(MutableWME::Ptr wme, std::string& value) const override
    {
        value = wme->value_;
    }

    MutableWMEAccessor* clone() const override
    {
        auto acc = new MutableWMEAccessor();
        acc->index() = index_;
        return acc;
    }
};

/**
    An alpha-node to get access to MutableWMEs -- actually: Just check *if* a WME is a
    MutableAlphaNode.
*/
class MutableAlphaNode : public AlphaNode {
    bool existenceCheck_;
    std::string valueToExist_;

public:
    MutableAlphaNode()
        : existenceCheck_(false), valueToExist_("")
    {
    }
    MutableAlphaNode(const std::string& val)
        : existenceCheck_(true), valueToExist_(val)
    {
    }

    std::string getDOTAttr() const override
    {
        return "[label=\"MutableAlphaNode\"]";
    }

    void activate(WME::Ptr wme, PropagationFlag flag) override
    {
        if (flag == rete::PropagationFlag::RETRACT)
        {
            // shortcut: just propagate the retract
            propagate(wme, PropagationFlag::RETRACT);
        }
        else if (flag == rete::PropagationFlag::ASSERT)
        {
            // check the type, only propagate on match
            auto ptr = std::dynamic_pointer_cast<MutableWME>(wme);
            if (ptr && (!existenceCheck_ || ptr->value_ == valueToExist_))
            {
                propagate(wme, PropagationFlag::ASSERT);
            }
        }
        else if (flag == PropagationFlag::UPDATE)
        {
            // same check, but explicitely propagate RETRACT too, and UPDATE instead of ASSERT
            auto ptr = std::dynamic_pointer_cast<MutableWME>(wme);
            if (ptr && (!existenceCheck_ || ptr->value_ == valueToExist_))
            {
                propagate(wme, PropagationFlag::UPDATE);
            }
            else
            {
                propagate(wme, PropagationFlag::RETRACT);
            }
        }
    }

    bool operator == (const AlphaNode& other) const override
    {
        return nullptr != dynamic_cast<const MutableAlphaNode*>(&other);
    }
};

/**
    Last but not least: A node builder that enables us to use the node in a rule.
*/
class MutableNodeBuilder : public NodeBuilder {
public:
    MutableNodeBuilder() : NodeBuilder("MutableWME", BuilderType::ALPHA)
    {
    }

    void buildAlpha(ArgumentList& args, std::vector<AlphaNode::Ptr>& nodes) const override
    {
        if (args.size() == 1 && args[0].isVariable() && args[0].getAccessor() == nullptr)
        {
            // create a new MutableAlphaNode
            nodes.push_back(MutableAlphaNode::Ptr(new MutableAlphaNode()));

            // bind the variable to a matching accessor
            args[0].bind(MutableWMEAccessor::Ptr(new MutableWMEAccessor()));
        }
        else if (args.size() == 1 && args[0].isConst())
        {
            nodes.push_back(MutableAlphaNode::Ptr(new MutableAlphaNode(std::string(args[0].getAST()))));
        }
        else
        {
            throw NodeBuilderException("invalid use of MutableWME-condition");
        }
    }

};

