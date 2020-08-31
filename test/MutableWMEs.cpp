#include <iostream>
#include <fstream>

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-reasoner/AssertedEvidence.hpp"
#include "../rete-rdf/Triple.hpp"
#include "../rete-core/Accessors.hpp"
#include "../rete-reasoner/NodeBuilder.hpp"
#include "../rete-reasoner/Exceptions.hpp"
#include "../rete-reasoner/ExplanationToDotVisitor.hpp"

using namespace rete;


void save(Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}


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
            if (std::dynamic_pointer_cast<MutableWME>(wme))
            {
                propagate(wme, PropagationFlag::ASSERT);
            }
        }
        else if (flag == PropagationFlag::UPDATE)
        {
            // same check, but explicitely propagate RETRACT too, and UPDATE instead of ASSERT
            if (std::dynamic_pointer_cast<MutableWME>(wme))
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
        // need exactly one argument which is an unbound variable.
        if (args.size() != 1 || !args[0].isVariable() || args[0].getAccessor() != nullptr)
            throw NodeBuilderException("invalid use of MutableWME-condition");

        // create a new MutableAlphaNode
        nodes.push_back(MutableAlphaNode::Ptr(new MutableAlphaNode()));

        // bind the variable to a matching accessor
        args[0].bind(MutableWMEAccessor::Ptr(new MutableWMEAccessor()));
    }

};


bool containsTriple(const std::vector<WME::Ptr>& wmes,
                    const std::string& s, const std::string& p, const std::string& o)
{
    for (auto wme : wmes)
    {
        auto triple = std::dynamic_pointer_cast<Triple>(wme);
        if (triple &&
            triple->subject == s &&
            triple->predicate == p &&
            triple->object == o)
        {
            return true;
        }
    }
    return false;
}

int main()
{
    RuleParser p;
    p.registerNodeBuilder<MutableNodeBuilder>();

    Reasoner reasoner(1000);
    reasoner.setCallback(
        [](WME::Ptr wme, PropagationFlag flag)
        {
            std::cout << "CALLBACK: ";
            if (flag == rete::ASSERT) std::cout << "ASSERT";
            if (flag == rete::RETRACT) std::cout << "RETRACT";
            if (flag == rete::UPDATE) std::cout << "UPDATE";
            std::cout << " " << wme->toString() << std::endl;
        }
    );

    /**
    */
    auto rules = p.parseRules(
        // "[rule0: (?a ?b ?c) -> (<foo> <bar> <baz>)]\n"
        "[rule1: MutableWME(?value) -> (<foo> <mutable> ?value)]\n"
        "[rule2: (<foo> <mutable> <value_A>) -> (<test_1> <result> <success>)]\n"
        "[rule3: (<foo> <mutable> <value_B>) -> (<test_2> <result> <success>)]\n"
        "[rule4: (<foo> <bar> <baz>), MutableWME(?value) -> (<join> <right> ?value)]\n"
        "[rule5: MutableWME(?value), (<foo> <bar> <baz>) -> (<join> <left> ?value)]",
        reasoner.net()
    );

    // add one mutable wme
    auto wme1 = std::make_shared<MutableWME>();
    wme1->value_ = "<value_A>";
    auto ev1 = std::make_shared<AssertedEvidence>("fact-group-1");
    reasoner.addEvidence(wme1, ev1);

    // and a (static) triple, so we can get some join nodes going and check if they propertly
    // update, too.
    auto wme2 = std::make_shared<Triple>("<foo>", "<bar>", "<baz>");
    reasoner.addEvidence(wme2, ev1);

    reasoner.performInference();
    save(reasoner.net(), "mutablewmes-1.dot");
    auto wmes = reasoner.getCurrentState().getWMEs();

    ExplanationToDotVisitor visitor;
    reasoner.getCurrentState().traverseExplanation(wmes[3], visitor);
    std::ofstream("explain_wme3-1.dot") << visitor.str();

    // there should be 5 wmes:
    // MutableWME, (<foo> <bar> <baz>), and the results from rules1 to 5, where only one of rule2 and rule3 should be active at a time.
    // 2 input + rule1 + (rule2 | rule3) + rule4 + rule5 = 6
    if (wmes.size() != 6) return 2;
    // check for rule2 success
    if (!containsTriple(wmes, "<test_1>", "<result>", "<success>")) return 3;
    // check for correct value un rule4 and 5:
    if (!containsTriple(wmes, "<join>", "<right>", "<value_A>") ||
        !containsTriple(wmes, "<join>", "<left>", "<value_A>")) return 4;

    // ------------------------------------------------------------------------
    // now, modify the mutable wme, and check again.
    // ------------------------------------------------------------------------
    wme1->value_ = "<value_B>";
    reasoner.net().getRoot()->activate(wme1, PropagationFlag::UPDATE); // TODO: convenience method?
    reasoner.performInference();
    save(reasoner.net(), "mutablewmes-2.dot");
    wmes = reasoner.getCurrentState().getWMEs();


    if (wmes.size() != 6) return 5;

    visitor.reset();
    reasoner.getCurrentState().traverseExplanation(wmes[3], visitor);
    std::ofstream("explain_wme3-2.dot") << visitor.str();

    // check for rule2 success
    if (!containsTriple(wmes, "<test_2>", "<result>", "<success>")) return 6;
    // check for correct value un rule4 and 5:
    if (!containsTriple(wmes, "<join>", "<right>", "<value_B>") ||
        !containsTriple(wmes, "<join>", "<left>", "<value_B>")) return 7;


    // now I'm just curious: What happens on an UPDATE when nothing has changed?
    std::cout << "------ I changed... nothing: ------" << std::endl;
    reasoner.net().getRoot()->activate(wme1, PropagationFlag::UPDATE); // TODO: convenience method?
    reasoner.performInference();

    return 0;
}
