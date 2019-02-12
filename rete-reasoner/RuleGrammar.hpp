#define USE_RTTI
#include <pegmatite/pegmatite.hh>

namespace rete {

using namespace pegmatite;

/**
    The rule-parsers grammar
*/
class RuleGrammar {
    RuleGrammar();

public:
    static const RuleGrammar& get()
    {
        static RuleGrammar g;
        return g;
    }

    #define TRACE_LEVEL 1
    #if TRACE_LEVEL > 0
        #define rtrace(name, expression)    (trace((name), (expression)))
    #else
        #define rtrace(name, expression)    (expression)
    #endif
    #if TRACE_LEVEL > 1
        #define r1trace(name, expression)    (trace((name), (expression)))
    #else
        #define r1trace(name, expression)    (expression)
    #endif

    Rule endl   = "\r\n"_E | "\n\r" | "\n" | "\r";
    Rule ws     = *(" \t"_S | endl);
    Rule alpha  = "[a-zA-Z]"_R;
    Rule num    = "[0-9]"_R;
    Rule alphanum = alpha | num;
    Rule echar  = '\\' >> "tbnrf\"\'\\"_S;
    // Rule iriref = rtrace("iriref", ('<'_E >> *(!("<>\"{}|^`\\"_S | ('\x00'_E -'\x20'))) >> '>'));
    Rule iriref = rtrace("iriref",
        (
            r1trace("open-bracket", '<'_E) >>
            r1trace("any-amount-of-not-stuff", *(
                r1trace("not", !
                    r1trace("stuff", ("<>\"{}|^`\\"_S))) >>
                    r1trace("followed by any (not doesnt move cursor)", any())
                      )
                  ) >>
            r1trace("close-bracket", '>'))
        );

    // e.g.: @PREFIX envire: <http://envire.semantic/>
    Rule prefixname = rtrace("prefixname", (+alphanum));
    Rule prefixuri = rtrace("prefixuri", iriref);
    Rule prefixdef = rtrace("prefixdef", "@PREFIX"_E >> prefixname >> ':' >> prefixuri);

    Rule prefixedURI = rtrace("prefixedURI", term(+alphanum >> ':' >> +alphanum));

    Rule stringliteralquote = '\"'_E >> *((!("\x22\x5C\xA\xD"_S)) | echar) >> '\"';
    Rule langtag = '@'_E >> +alpha >> *('-' >> +alphanum);

    Rule literal =          (stringliteralquote >> -("^^" >> iriref | langtag));
    Rule blank_node_label = ("_:"_E >> +alphanum);
    Rule variable =         ("?"_E >> +alphanum);

    Rule subject    = rtrace("subject",   term(variable | iriref | prefixedURI | blank_node_label));
    Rule predicate  = rtrace("predicate", term(variable | iriref | prefixedURI));
    Rule object     = rtrace("object",    term(variable | iriref | prefixedURI | blank_node_label | literal));

    Rule triple = rtrace("triple",   ('('_E >> subject >> predicate >> object >> ')'));
    Rule triples = rtrace("triples", triple >> *(',' >> triple));

    // [name: (precondition1), (precondition2) --> (effect1), (effect2)]
    Rule rulename = rtrace("rulename", (+alphanum >> ':'));
    Rule rule = rtrace("rule", ('['_E >> -rulename >> triples >> "->"_E >> triples >> ']'));
    Rule rules = rtrace("rules", *prefixdef >> +rule);

    // TODO: Overhaul for builtins, other WMEs than triples, ...
};


} /* rete */
