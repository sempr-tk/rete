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

    /*
        Some basic definitions of whitespace, alphanumeric and escaped characters, numbers and
        quoted strings.
    */
    Rule endl   = "\r\n"_E | "\n\r" | "\n" | "\r";
    Rule ws     = *(" \t"_S | endl);
    Rule alpha  = "[a-zA-Z]"_R;
    Rule num    = "[0-9]"_R;
    Rule alphanum = alpha | num;
    Rule echar  = '\\' >> "tbnrf\"\'\\"_S;

    /*
    // while num is just a single digit,
    // number is a complete, terminal value.
    Rule number = term( -("+-"_S) >> +num >> // some digits, plus optionally...
                  -('.'_E >> +num >>  // dot and numbers, plus optionally...
                    -("eE"_S >> -("+-"_S) >> +num) // an optionally signed exponent
                    ));
    */

    // integer: optional sign + digits, *not* followed by a dot.
    // e.g.  -42e3 = -42*10^3 = -42000
    Rule integer = term(-("+-"_S) >> +num >> (!"."_E));

    // float: optional sign, digites, optional dot+digits,
    //        optional exponent which may be signed.
    // e.g. -1.2e-3 = -0.0012
    Rule floating = term(-("+-"_S) >> +num >> -('.'_E >> +num) >> -("eE"_S >> -("+-"_S) >> +num));

    Rule number = rtrace("number",
                    rtrace("integer", integer) |
                    rtrace("floating", floating)
                  );


    // to allow whitespaces etc as a single argument, define a quoted string.
    // starts with a ", ends with a ", and everything inbetween is either \" or just not "
    Rule quotedString = "\"" >> *("\\\""_E | (!"\""_E >> any())) >> "\"";

    /*
        In the world of RDF there are some special things to consider: IRIs, typed string literals,
        language tags and the structure of triples.
    */
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

    Rule prefixedURI = rtrace("prefixedURI", term(+alphanum >> ':' >> +(alphanum | "_"_E)));

    Rule stringliteralquote = '\"'_E >> *((!("\x22\x5C\xA\xD"_S) >> any()) | echar) >> '\"';
    Rule langtag = '@'_E >> +alpha >> *('-' >> +alphanum);

    Rule literal =          (stringliteralquote >> -("^^" >> iriref | langtag));
    Rule blank_node_label = ("_:"_E >> +alphanum);
    Rule variable =         ("?"_E >> +alphanum);
    Rule uri =              prefixedURI | iriref;

    Rule subject    = rtrace("subject",   term(variable | iriref | prefixedURI | blank_node_label | globalConstID));
    Rule predicate  = rtrace("predicate", term(variable | iriref | prefixedURI | globalConstID));
    Rule object     = rtrace("object",    term(variable | iriref | prefixedURI | blank_node_label | literal | number | globalConstID));

    /**
    Modifier to negate joins (forward only when there is no match)
    */
    Rule noValue = rtrace("noValue", "noValue "_E | "no " | "novalue ");
    Rule triple = rtrace("triple", '('_E >> subject >> predicate >> object >> ')');


    /*
        Definition of global constants
    */
    Rule globalConstID = rtrace("globalConstID", term("$"_E >> +alphanum));
    Rule globalConstDef = rtrace("globalConstDef", globalConstID >> ":" >> (quotedString | number | uri));

    /*
        Here comes the more general stuff: Basically, rules are created from preconditions and
        effects. Preconditions are subdivided into alpha-conditions, which are criteria on a single
        WME and are chained together using joins, and builtins, which are more or less simple
        computations on the matched values (e.g., compute a sum of two values, concatenate a
        string, etc...).
        Special cases are triples, which do not need a name (as they are the default condition, and
        also the default effect) and for which we specified a more sophisticated grammar above.
        Further extensions to alpha conditions can be implemented without changing the grammar by
        simply accepting arguments as defined below.
    */
    Rule argument = rtrace("argument", term(quotedString | number | variable | uri | globalConstID));

    // cannot reuse triple for infertriple, would lead to triple being constructed before infertriple, and the infertriple being empty...
    Rule inferTriple = rtrace("inferTriple", '('_E >> subject >> predicate >> object >> ')');
    /*
        Generic conditions and effects
    */
    // alpha conditions
    Rule alphaConditionName = rtrace("alphaConditionName", !noValue >> term(+(alphanum | "<>"_S)));
    Rule genericAlphaCondition = rtrace("genericAlphaCondition",
                                         alphaConditionName >> "(" >> *argument >> ")");

    // builtins
    // TODO: distinguish between alpha conditions and builtins?
    //       Not important for the parser to work, only to fail fast during parsing instead of construction. alpha- and builtin-builders reside in the same pool of node builders.
    Rule builtinName = rtrace("builtinName", !noValue >> term(+alphanum) >> -(":"_E >> term(+alphanum)));
    Rule builtin = rtrace("builtin", builtinName >> "(" >> *argument >> ")");

    // effects
    Rule effectName = rtrace("effectName", term(+(alphanum | "<>"_S) >> -(":"_E >> +(alphanum | "<>"_S))));
    Rule genericEffect = rtrace("genericEffect", effectName >> "(" >> *argument >> ")");


    /**
        GROUP BY (?var1 ?var2 ...)
        groups matches by identical values for every variable given
    */
    Rule groupBy = rtrace("group by", "GROUP BY"_E >> "(" >> *term(variable) >> ")");

    /**
        With the new noValue syntax we allow complex noValue-groups and nesting
        them. To do this, a precondition can now also be a noValueGroup, which
        itself is composed of preconditions.
    */
    Rule precondition = rtrace("precondition", triple | builtin | genericAlphaCondition | noValueGroup | groupBy);
    Rule noValueGroup = rtrace("noValueGroup",
            noValue >> "{" >> precondition >> *(',' >> precondition) >> "}"
    );


    Rule effect = rtrace("effect", inferTriple | genericEffect);

    // [name: (precondition1), (precondition2) --> (effect1), (effect2)]
    Rule rulename = rtrace("rulename", +alphanum);
    Rule rule = rtrace("rule", ('['_E >> -(rulename >> ':') >> precondition >> *(',' >> precondition) >> "->" >> effect >> *(',' >> effect) >> ']'));
    Rule rules = rtrace("rules", *prefixdef >> *globalConstDef >> +rule);

    // TODO: Overhaul for builtins, other WMEs than triples, ...
};


} /* rete */
