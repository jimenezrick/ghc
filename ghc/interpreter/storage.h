
/* --------------------------------------------------------------------------
 * Defines storage datatypes: Text, Name, Module, Tycon, Cell, List, Pair,
 * Triple, ...
 *
 * The Hugs 98 system is Copyright (c) Mark P Jones, Alastair Reid, the
 * Yale Haskell Group, and the Oregon Graduate Institute of Science and
 * Technology, 1994-1999, All rights reserved.  It is distributed as
 * free software under the license in the file "License", which is
 * included in the distribution.
 *
 * $RCSfile: storage.h,v $
 * $Revision: 1.32 $
 * $Date: 2000/03/10 14:53:00 $
 * ------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------
 * Typedefs for main data types:
 * Many of these type names are used to indicate the intended us of a data
 * item, rather than for type checking purposes.  Sadly (although sometimes,
 * fortunately), the C compiler cannot distinguish between the use of two
 * different names defined to be synonyms for the same types.
 * ------------------------------------------------------------------------*/

typedef Int          Text;                       /* text string            */
typedef Unsigned     Syntax;                     /* syntax (assoc,preced)  */
typedef Int          Cell;                       /* general cell value     */
typedef Cell far     *Heap;                      /* storage of heap        */
typedef Cell         Pair;                       /* pair cell              */
typedef Int          StackPtr;                   /* stack pointer          */
typedef Cell         Offset;                     /* offset/generic variable*/
typedef Int          Script;                     /* script file number     */
typedef Int          Module;                     /* module                 */
typedef Cell         Tycon;                      /* type constructor       */
typedef Cell         Type;                       /* type expression        */
typedef Cell         Kind;                       /* kind expression        */
typedef Cell         Kinds;                      /* list of kinds          */
typedef Cell         Constr;                     /* constructor expression */
typedef Cell         Name;                       /* named value            */
typedef Cell         Class;                      /* type class             */
typedef Cell         Inst;                       /* instance of type class */
typedef Cell         Triple;                     /* triple of cell values  */
typedef Cell         List;                       /* list of cells          */
typedef Cell         Bignum;                     /* bignum integer         */
typedef Cell         Float;                      /* floating pt literal    */
#if TREX
typedef Cell         Ext;                        /* extension label        */
#endif

typedef Cell         ConId;
typedef Cell         VarId;
typedef Cell         QualId;
typedef Cell         ConVarId;

/* --------------------------------------------------------------------------
 * Text storage:
 * provides storage for the characters making up identifier and symbol
 * names, string literals, character constants etc...
 * ------------------------------------------------------------------------*/

extern  String       textToStr            Args((Text));
extern  Text         findText             Args((String));
extern  Text         inventText           Args((Void));
extern  Text         inventDictText       Args((Void));
extern  Bool         inventedText         Args((Text));
extern  Text         enZcodeThenFindText  Args((String));
extern  Text         unZcodeThenFindText  Args((String));

/* Variants of textToStr and syntaxOf which work for idents, ops whether
 * qualified or unqualified.
 */
extern  String       identToStr         Args((Cell));
extern	Text	     fixLitText	 	Args((Text));
extern  Syntax       identSyntax        Args((Cell));
extern  Syntax       defaultSyntax      Args((Text));

/* --------------------------------------------------------------------------
 * Specification of syntax (i.e. default written form of application)
 * ------------------------------------------------------------------------*/

#define MIN_PREC  0                    /* weakest binding operator         */
#define MAX_PREC  9                    /* strongest binding operator       */
#define FUN_PREC  (MAX_PREC+2)         /* binding of function symbols      */
#define DEF_PREC  MAX_PREC
#define APPLIC    0                    /* written applicatively            */
#define LEFT_ASS  1                    /* left associative infix           */
#define RIGHT_ASS 2                    /* right associative infix          */
#define NON_ASS   3                    /* non associative infix            */
#define DEF_ASS   LEFT_ASS

#define UMINUS_PREC  6                  /* Change these settings at your   */
#define UMINUS_ASSOC LEFT_ASS           /* own risk; they may not work!    */

#define assocOf(x)      ((x)&NON_ASS)
#define precOf(x)       ((x)>>2)
#define mkSyntax(a,p)   ((a)|((p)<<2))
#define DEF_OPSYNTAX    mkSyntax(DEF_ASS,DEF_PREC)
#define NO_SYNTAX       (-1)

extern  Void   addSyntax  Args((Int,Text,Syntax));
extern  Syntax syntaxOf   Args((Text));

/* --------------------------------------------------------------------------
 * Heap storage:
 * Provides a garbage collectable heap for storage of expressions etc.
 * ------------------------------------------------------------------------*/

#define heapAlloc(s) (Heap)(farCalloc(s,sizeof(Cell)))
#define heapBuilt()  (heapFst)
extern  Int          heapSize;
extern  Heap         heapFst, heapSnd;
extern  Heap         heapTopFst;
extern  Heap         heapTopSnd;
extern  Bool         consGC;            /* Set to FALSE to turn off gc from*/
                                        /* C stack; use with extreme care! */
extern Int   cellsRecovered;            /* cells recovered by last gc      */

#define fst(c)       heapTopFst[c]
#define snd(c)       heapTopSnd[c]

extern  Pair         pair            Args((Cell,Cell));
extern  Void         garbageCollect  Args((Void));

extern  Void         overwrite       Args((Pair,Pair));
extern  Void         overwrite2      Args((Pair,Cell,Cell));
extern  Cell         markExpr        Args((Cell));
extern  Void         markWithoutMove Args((Cell));

#define mark(v)      v=markExpr(v)

#define isPair(c)    ((c)<0)
#define isGenPair(c) ((c)<0 && -heapSize<=(c))

extern  Cell         whatIs    Args((Cell));

/* --------------------------------------------------------------------------
 * Box cell tags are used as the fst element of a pair to indicate that
 * the snd element of the pair is to be treated in some special way, other
 * than as a Cell.  Examples include holding integer values, variable name
 * and string text etc.
 * ------------------------------------------------------------------------*/

#if !defined(SIZEOF_VOID_P) || !defined(SIZEOF_INT)
#error SIZEOF_VOID_P or SIZEOF_INT is not defined
#endif

#define TAGMIN       1            /* Box and constructor cell tag values   */
#define BCSTAG       30           /* Box=TAGMIN..BCSTAG-1                  */
#define isTag(c)     (TAGMIN<=(c) && (c)<SPECMIN) /* Tag cell values       */
#define isBoxTag(c)  (TAGMIN<=(c) && (c)<BCSTAG)  /* Box cell tag values   */
#define isConTag(c)  (BCSTAG<=(c) && (c)<SPECMIN) /* Constr cell tag values*/

#define FREECELL     3            /* Free list cell:          snd :: Cell  */
#define VARIDCELL    4            /* Identifier variable:     snd :: Text  */
#define VAROPCELL    5            /* Operator variable:       snd :: Text  */
#define DICTVAR      6            /* Dictionary variable:     snd :: Text  */
#define CONIDCELL    7            /* Identifier constructor:  snd :: Text  */
#define CONOPCELL    8            /* Operator constructor:    snd :: Text  */
#define STRCELL      9            /* String literal:          snd :: Text  */
#define INTCELL      10           /* Int literal:             snd :: Int   */
#define ADDPAT       11           /* (_+k) pattern discr:     snd :: Int   */
#define FLOATCELL    15           /* Floating Pt literal:     snd :: Text  */
#define BIGCELL      16           /* Integer literal:         snd :: Text  */
#define PTRCELL      17           /* C Heap Pointer           snd :: Ptr   */
#define CPTRCELL     21           /* Native code pointer      snd :: Ptr   */

#if IPARAM
#define IPCELL       19		  /* Imp Param Cell:	      snd :: Text  */
#define IPVAR	     20		  /* ?x:		      snd :: Text  */
#endif

#if TREX
#define EXTCOPY      22           /* Copy of an Ext:          snd :: Text  */
#endif

#define qmodOf(c)       (textOf(fst(snd(c))))    /* c ::  QUALIDENT        */
#define qtextOf(c)      (textOf(snd(snd(c))))    /* c ::  QUALIDENT        */
#define mkVar(t)        ap(VARIDCELL,t)
#define mkVarop(t)      ap(VAROPCELL,t)
#define mkCon(t)        ap(CONIDCELL,t)
#define mkConop(t)      ap(CONOPCELL,t)
#define mkQVar(m,t)     ap(QUALIDENT,pair(mkCon(m),mkVar(t)))
#define mkQCon(m,t)     ap(QUALIDENT,pair(mkCon(m),mkCon(t)))
#define mkQVarOp(m,t)   ap(QUALIDENT,pair(mkCon(m),mkVarop(t)))
#define mkQConOp(m,t)   ap(QUALIDENT,pair(mkCon(m),mkConop(t)))
#define mkQualId(m,t)   ap(QUALIDENT,pair(m,t))
#define intValOf(c)     (snd(c))
#define inventVar()     mkVar(inventText())
#define mkDictVar(t)    ap(DICTVAR,t)
#define inventDictVar() mkDictVar(inventDictText())
#define mkStr(t)        ap(STRCELL,t)
#if IPARAM
#define mkIParam(c)	ap(IPCELL,snd(c))
#define isIP(p)		(whatIs(p) == IPCELL)
#define ipMatch(pi, t)	(isIP(fun(pi)) && textOf(fun(pi)) == t)
#define ipVar(pi)	textOf(fun(pi))
#else
#define isIP(p)		FALSE
#endif

extern  Bool            isVar        Args((Cell));
extern  Bool            isCon        Args((Cell));
extern  Bool            isQVar       Args((Cell));
extern  Bool            isQCon       Args((Cell));
extern  Bool            isQualIdent  Args((Cell));
extern  Bool            eqQualIdent ( QualId c1, QualId c2 );
extern  Bool            isIdent      Args((Cell));
extern  String          stringNegate Args((String));
extern  Text            textOf       Args((Cell));

#define isFloat(c)       (isPair(c) && fst(c)==FLOATCELL)
#define stringToFloat(s) pair(FLOATCELL,findText(s))
#define floatToString(f) textToStr(snd(f))
#define floatOf(f)       atof(floatToString(f))
#define mkFloat(f)       (f)  /* ToDo: is this right? */
#define floatNegate(f)   stringToFloat(stringNegate(floatToString(f)))

#define stringToBignum(s) pair(BIGCELL,findText(s))
#define bignumToString(b) textToStr(snd(b))

#define isPtr(c)        (isPair(c) && fst(c)==PTRCELL)
extern  Cell            mkPtr           Args((Ptr));
extern  Ptr             ptrOf           Args((Cell));
#define isCPtr(c)       (isPair(c) && fst(c)==CPTRCELL)
extern  Cell            mkCPtr          Args((Ptr));
extern  Ptr             cptrOf          Args((Cell));

/* --------------------------------------------------------------------------
 * Constructor cell tags are used as the fst element of a pair to indicate
 * a particular syntactic construct described by the snd element of the
 * pair.
 * Note that a cell c will not be treated as an application (AP/isAp) node
 * if its first element is a constructor cell tag, whereas a cell whose fst
 * element is a special cell will be treated as an application node.
 * ------------------------------------------------------------------------*/

#define LETREC       30           /* LETREC     snd :: ([Decl],Exp)        */
#define COND         31           /* COND       snd :: (Exp,Exp,Exp)       */
#define LAMBDA       32           /* LAMBDA     snd :: Alt                 */
#define FINLIST      33           /* FINLIST    snd :: [Exp]               */
#define DOCOMP       34           /* DOCOMP     snd :: (Exp,[Qual])        */
#define BANG         35           /* BANG       snd :: Type                */
#define COMP         36           /* COMP       snd :: (Exp,[Qual])        */
#define ASPAT        37           /* ASPAT      snd :: (Var,Exp)           */
#define ESIGN        38           /* ESIGN      snd :: (Exp,Type)          */
#define RSIGN        39           /* RSIGN      snd :: (Rhs,Type)          */
#define CASE         40           /* CASE       snd :: (Exp,[Alt])         */
#define NUMCASE      41           /* NUMCASE    snd :: (Exp,Disc,Rhs)      */
#define FATBAR       42           /* FATBAR     snd :: (Exp,Exp)           */
#define LAZYPAT      43           /* LAZYPAT    snd :: Exp                 */
#define DERIVE       45           /* DERIVE     snd :: Cell                */
#define BOOLQUAL     49           /* BOOLQUAL   snd :: Exp                 */
#define QWHERE       50           /* QWHERE     snd :: [Decl]              */
#define FROMQUAL     51           /* FROMQUAL   snd :: (Exp,Exp)           */
#define DOQUAL       52           /* DOQUAL     snd :: Exp                 */
#define MONADCOMP    53           /* MONADCOMP  snd :: ((m,m0),(Exp,[Qual])*/
#define GUARDED      54           /* GUARDED    snd :: [guarded exprs]     */
#define ARRAY        55           /* Array      snd :: (Bounds,[Values])   */
#define MUTVAR       56           /* Mutvar     snd :: Cell                */
#define HUGSOBJECT   57           /* HUGSOBJECT snd :: Cell                */

#if IPARAM
#define WITHEXP      58 	  /* WITHEXP    snd :: [(Var,Exp)]	   */
#endif

#define POLYTYPE     60           /* POLYTYPE   snd :: (Kind,Type)         */
#define QUAL         61           /* QUAL       snd :: ([Classes],Type)    */
#define RANK2        62           /* RANK2      snd :: (Int,Type)          */
#define EXIST        63           /* EXIST      snd :: (Int,Type)          */
#define POLYREC      64           /* POLYREC    snd :: (Int,Type)          */
#define BIGLAM       65           /* BIGLAM     snd :: (vars,patterns)     */
#define CDICTS       66           /* CDICTS     snd :: ([Pred],Type)       */

#define LABC         67           /* LABC       snd :: (con,[(Vars,Type)]) */
#define CONFLDS      68           /* CONFLDS    snd :: (con,[Field])       */
#define UPDFLDS      69           /* UPDFLDS    snd :: (Exp,[con],[Field]) */
#if TREX
#define RECORD       70           /* RECORD     snd :: [Val]               */
#define EXTCASE      71           /* EXTCASE    snd :: (Exp,Disc,Rhs)      */
#define RECSEL       72           /* RECSEL     snd :: Ext                 */
#endif
#define IMPDEPS      73           /* IMPDEPS    snd :: [Binding]           */

#define QUALIDENT    74           /* Qualified identifier  snd :: (Id,Id)  */
#define HIDDEN       75           /* hiding import list    snd :: [Entity] */
#define MODULEENT    76           /* module in export list snd :: con      */

#define INFIX        77           /* INFIX      snd :: (see tidyInfix)     */
#define ONLY         78           /* ONLY       snd :: Exp                 */
#define NEG          79           /* NEG        snd :: Exp                 */

/* Used when parsing GHC interface files */
#define DICTAP       80           /* DICTAP     snd :: (QClassId,[Type])   */
#define UNBOXEDTUP   81           /* UNBOXEDTUP snd :: [Type]              */

#if SIZEOF_VOID_P != SIZEOF_INT
#define PTRCELL      82           /* C Heap Pointer snd :: (Int,Int)       */
#endif

/* STG syntax */
#define STGVAR       92           /* STGVAR     snd :: (StgRhs,info)       */
#define STGAPP       93           /* STGAPP     snd :: (StgVar,[Arg])      */
#define STGPRIM      94           /* STGPRIM    snd :: (PrimOp,[Arg])      */
#define STGCON       95           /* STGCON     snd :: (StgCon,[Arg])      */
#define PRIMCASE     96           /* PRIMCASE   snd :: (Expr,[PrimAlt])    */
#define DEEFALT      97           /* DEEFALT    snd :: (Var,Expr)          */
#define CASEALT      98           /* CASEALT    snd :: (Con,[Var],Expr)    */
#define PRIMALT      99           /* PRIMALT    snd :: ([Var],Expr)        */


/* 
   Top-level interface entities 
   type Line             = Int  -- a line number 
   type ConVarId         = CONIDCELL | VARIDCELL
   type <a>              = ZList a
   type ExportListEntry  = ConVarId | (ConId, <ConVarId>) 
   type Associativity    = mkInt of LEFT_ASS | RIGHT_ASS | NON_ASS
   type Constr           = ((ConId, [((Type,VarId,Int))]))
               ((constr name, [((type, field name if any, strictness))]))
               strictness: 0 => none, 1 => !, 2 => !! (unpacked)
   All 2/3/4/5 tuples in the interface abstract syntax are done with
   z-tuples.
*/

#define I_INTERFACE  109  /* snd :: ((ConId, [I_IMPORT..I_VALUE])) 
                                    interface name, list of iface entities */

#define I_IMPORT     110  /* snd :: ((ConId, [ConVarId]))
                                    module name, list of entities          */

#define I_INSTIMPORT 111  /* snd :: NIL    -- not used at present          */

#define I_EXPORT     112  /* snd :: ((ConId, [ExportListEntry]))
                                    this module name?, entities to export  */

#define I_FIXDECL    113  /* snd :: ((NIL|Int, Associativity, ConVarId))   
                                    fixity, associativity, name            */

#define I_INSTANCE   114 /* snd :: ((Line, 
                                     [((VarId,Kind))], 
                                     Type, VarId, Inst))
                   lineno, 
                   forall-y bit (eg __forall [a b] =>),
                   other bit, eg { C a1 } -> { C2 a2 } -> ... -> { Cn an },
                   name of dictionary builder,
                   (after startGHCInstance) the instance table location    */

#define I_TYPE       115 /* snd :: ((Line, ConId, [((VarId,Kind))], Type))
                            lineno, tycon, kinded tyvars, the type expr    */

#define I_DATA       116 /* snd :: ((Line, [((QConId,VarId))], ConId, 
                                          [((VarId,Kind))], [Constr]) 
                            lineno, context, tycon, kinded tyvars, constrs 
                           An empty constr list means exported abstractly. */

#define I_NEWTYPE    117 /* snd :: ((Line, [((QConId,VarId))], ConId,
                                    [((VarId,Kind))], ((ConId,Type)) ))
                             lineno, context, tycon, kinded tyvars, constr 
                                    constr==NIL means exported abstractly. */

#define I_CLASS      118 /* snd :: ((Line, [((QConId,VarId))], ConId,
                                    [((VarId,Kind))], [((VarId,Type))]))
                            lineno, context, classname, 
                                      kinded tyvars, method sigs           */

#define I_VALUE      119 /* snd :: ((Line, VarId, Type))                   */



/* Generic syntax */
#if 0
#define ZCONS        190          /* snd :: (Cell,Cell)                    */
#endif


#define ZTUP2        192          /* snd :: (Cell,Cell)                    */
#define ZTUP3        193          /* snd :: (Cell,(Cell,Cell))             */
#define ZTUP4        194          /* snd :: (Cell,(Cell,(Cell,Cell)))      */
#define ZTUP5        195       /* snd :: (Cell,(Cell,(Cell,(Cell,Cell))))  */

/* Last constructor tag must be less than SPECMIN */

/* --------------------------------------------------------------------------
 * Special cell values:
 * ------------------------------------------------------------------------*/

#define SPECMIN      201

#if TREX
#define isSpec(c)    (SPECMIN<=(c) && (c)<EXTMIN)/* Special cell values    */
#else
#define isSpec(c)    (SPECMIN<=(c) && (c)<OFFMIN)
#endif

#define NONE         201          /* Dummy stub                            */
#define STAR         202          /* Representing the kind of types        */
#if TREX
#define ROW          203          /* Representing the kind of rows         */
#endif
#define WILDCARD     204          /* Wildcard pattern                      */
#define SKOLEM       205          /* Skolem constant                       */

#define DOTDOT       206          /* ".." in import/export list            */

#define NAME         210          /* whatIs code for isName                */
#define TYCON        211          /* whatIs code for isTycon               */
#define CLASS        212          /* whatIs code for isClass               */
#define MODULE       213          /* whatIs code for isModule              */
#define INSTANCE     214          /* whatIs code for isInst                */
#define TUPLE        215          /* whatIs code for tuple constructor     */
#define OFFSET       216          /* whatis code for offset                */
#define AP           217          /* whatIs code for application node      */
#define CHARCELL     218          /* whatIs code for isChar                */
#if TREX
#define EXT          219          /* whatIs code for isExt                 */
#endif

#define SIGDECL      220          /* Signature declaration                 */
#define FIXDECL      221          /* Fixity declaration                    */
#define FUNBIND      222          /* Function binding                      */
#define PATBIND      223          /* Pattern binding                       */

#define DATATYPE     230          /* Datatype type constructor             */
#define NEWTYPE      231          /* Newtype type constructor              */
#define SYNONYM      232          /* Synonym type constructor              */
#define RESTRICTSYN  233          /* Synonym with restricted scope         */

#define NODEPENDS    235          /* Stop calculation of deps in type check*/
#define PREDEFINED   236          /* Predefined name, not yet filled       */

/* --------------------------------------------------------------------------
 * Tuple data/type constructors:
 * ------------------------------------------------------------------------*/

extern Text ghcTupleText    Args((Tycon));
extern Text ghcTupleText_n  Args((Int));



#if TREX
#define EXTMIN       301
#define isExt(c)     (EXTMIN<=(c) && (c)<OFFMIN)
#define extText(e)   tabExt[(e)-EXTMIN]
#define extField(c)  arg(fun(c))
#define extRow(c)    arg(c)

extern Text          DECTABLE(tabExt);
extern Ext           mkExt Args((Text));
#else
#define mkExt(t) NIL
#endif

extern Module findFakeModule ( Text t );
extern Tycon addTupleTycon ( Int n );
extern Name addWiredInBoxingTycon
               ( String modNm, String typeNm, String constrNm,
                 Int rep, Kind kind );
Tycon addWiredInEnumTycon ( String modNm, String typeNm, 
                            List /*of Text*/ constrs );

/* --------------------------------------------------------------------------
 * Offsets: (generic types/stack offsets)
 * ------------------------------------------------------------------------*/

#if TREX
#define OFFMIN       (EXTMIN+NUM_EXT)
#else
#define OFFMIN       301
#endif
#define isOffset(c)  (OFFMIN<=(c) && (c)<MODMIN)
#define offsetOf(c)  ((c)-OFFMIN)
#define mkOffset(o)  (OFFMIN+(o))


/* --------------------------------------------------------------------------
 * Modules:
 * ------------------------------------------------------------------------*/

#define MODMIN        (OFFMIN+NUM_OFFSETS)

#define isModule(c)   (MODMIN<=(c) && (c)<TYCMIN)
#define mkModule(n)   (MODMIN+(n))
#define module(n)     tabModule[(n)-MODMIN]

/* Import defns for the ObjectCode struct in Module. */
#include "object.h"

/* Under Haskell 1.3, the list of qualified imports is always a subset
 * of the list of unqualified imports.  For simplicity and flexibility,
 * we do not attempt to exploit this fact - when a module is imported
 * unqualified, it is added to both the qualified and unqualified
 * import lists.
 * Similarily, Haskell 1.3 does not allow a constructor to be imported
 * or exported without exporting the type it belongs to but the export
 * list is just a flat list of Texts (before static analysis) or
 * Tycons, Names and Classes (after static analysis).
 */
struct Module {
    Text  text;
    /* Lists of top level objects (local defns + imports)                  */
    List  tycons;
    List  names;
    List  classes;
    List  exports; /* [ Entity | (Entity, NIL|DOTDOT) ] */
    /* List of qualified imports.  Used both during compilation and when
     * evaluating an expression in the context of the current module.
     */
    List  qualImports;

    /* TRUE if module exists only via GHC primop defn; usually FALSE */
    Bool  fake; 

    /* The primary object file for this module. */
    ObjectCode* object;

    /* And any extras it might need. */
    ObjectCode* objectExtras;
    List        objectExtraNames;   /* :: [Text] -- names of extras */
};


extern Module currentModule;           /* Module currently being processed */
extern struct Module DECTABLE(tabModule);

extern Bool   isValidModule Args((Module));
extern Module newModule     Args((Text));
extern Module findModule    Args((Text));
extern Module findModid     Args((Cell));
extern Void   setCurrModule Args((Module));

extern void      addOTabName     Args((Module,char*,void*));
extern void*     lookupOTabName  Args((Module,char*));
extern char*     nameFromOPtr    Args((void*));

extern void          addSection    Args((Module,void*,void*,OSectionKind));
extern OSectionKind  lookupSection Args((void*));
extern void* lookupOExtraTabName ( char* sym );

#define isPrelude(m) (m==modulePrelude)

#define N_PRELUDE_SCRIPTS (combined ? 32 : 1)

/* --------------------------------------------------------------------------
 * Type constructor names:
 * ------------------------------------------------------------------------*/

#define TYCMIN       (MODMIN+NUM_MODULE)
#define isTycon(c)   (TYCMIN<=(c) && (c)<NAMEMIN && tabTycon[(c)-TYCMIN].tuple==-1)
#define tycon(n)     tabTycon[(n)-TYCMIN]

#define isTuple(c)   (TYCMIN<=(c) && (c)<NAMEMIN && tabTycon[(c)-TYCMIN].tuple>=0)
#define tupleOf(n)   (tabTycon[(n)-TYCMIN].tuple)
extern Tycon mkTuple ( Int );


struct strTycon {
    Text   text;
    Int    line;
    Module mod;                         /* module that defines it          */
    Int    tuple;                      /* tuple number, or -1 if not tuple */
    Int    arity;
    Kind   kind;                        /* kind (includes arity) of Tycon  */
    Cell   what;                        /* DATATYPE/SYNONYM/RESTRICTSYN... */
    Cell   defn;
    Name   conToTag;                    /* used in derived code            */
    Name   tagToCon;
    void*  itbl;                       /* For tuples, the info tbl pointer */
    Tycon  nextTyconHash;
};

extern struct strTycon DECTABLE(tabTycon);

extern Tycon newTycon     Args((Text));
extern Tycon findTycon    Args((Text));
extern Tycon addTycon     Args((Tycon));
extern Tycon findQualTycon Args((Cell));
extern Tycon addPrimTycon Args((Text,Kind,Int,Cell,Cell));

#define isSynonym(h)    (isTycon(h) && tycon(h).what==SYNONYM)
#define isQualType(t)	(isPair(t) && fst(t)==QUAL)
#define mkPolyType(n,t) pair(POLYTYPE,pair(n,t))
#define isPolyType(t)   (isPair(t) && fst(t)==POLYTYPE)
#define isPolyOrQualType(t) (isPair(t) && (fst(t)==POLYTYPE || fst(t)==QUAL))
#define polySigOf(t)    fst(snd(t))
#define monotypeOf(t)   snd(snd(t))

#define bang(t)         ap(BANG,t)
extern Tycon findQualTyconWithoutConsultingExportList ( QualId q );

/* --------------------------------------------------------------------------
 * Globally defined name values:
 * ------------------------------------------------------------------------*/

#define NAMEMIN      (TYCMIN+NUM_TYCON)
#define isName(c)    (NAMEMIN<=(c) && (c)<INSTMIN)
#define mkName(n)    (NAMEMIN+(n))
#define name(n)      tabName[(n)-NAMEMIN]

struct strName {
    Text   text;
    Int    line;
    Module mod;                         /* module that defines it          */
    Syntax syntax;
    Cell   parent; 
    Int    arity;
    Int    number;
    Cell   type;
    Cell   defn;
    Cell   stgVar;                                      /* really StgVar   */
    Text   callconv;                          /* for foreign import/export */
    void*  primop;                                      /* really StgPrim* */
    void*  itbl;                 /* For constructors, the info tbl pointer */
    Name   nextNameHash;
};

extern int numNames Args(( Void ));

extern struct strName DECTABLE(tabName);

/* The number field in a name is used to distinguish various kinds of name:
 *   mfunNo(i) = code for member function, offset i
 *               members that are sole elements of dict use mfunNo(0)
 *               members of dicts with more than one elem use mfunNo(n), n>=1
 *   EXECNAME  = code for executable name (bytecodes or primitive)
 *   SELNAME   = code for selector function
 *   DFUNNAME  = code for dictionary builder or selector
 *   cfunNo(i) = code for data constructor
 *               datatypes with only one constructor uses cfunNo(0)
 *               datatypes with multiple constructors use cfunNo(n), n>=1
 */

#define EXECNAME        0
#define SELNAME         1
#define DFUNNAME        2
#define CFUNNAME        3

#define isSfun(n)       (name(n).number==SELNAME)
#define isDfun(n)       (name(n).number==DFUNNAME)

#define isCfun(n)       (name(n).number>=CFUNNAME)
#define cfunOf(n)       (name(n).number-CFUNNAME)
#define cfunNo(i)       ((i)+CFUNNAME)
#define hasCfun(cs)     (nonNull(cs) && isCfun(hd(cs)))

#define isMfun(n)       (name(n).number<0)
#define mfunOf(n)       ((-1)-name(n).number)
#define mfunNo(i)       ((-1)-(i))

extern Name   newName         Args((Text,Cell));
extern Name   findName        Args((Text));
extern Name   addName         Args((Name));
extern Name   findQualName    Args((Cell));
extern Name   addPrimCfun     Args((Text,Int,Int,Cell));
extern Name   addPrimCfunREP  Args((Text,Int,Int,Int));
extern Int    sfunPos         Args((Name,Name));
extern Name   nameFromStgVar  Args((Cell));
extern Name   jrsFindQualName Args((Text,Text));

extern Name findQualNameWithoutConsultingExportList ( QualId q );

/* --------------------------------------------------------------------------
 * Type class values:
 * ------------------------------------------------------------------------*/

#define INSTMIN      (NAMEMIN+NUM_NAME) /* instances                       */
#define isInst(c)    (INSTMIN<=(c) && (c)<CLASSMIN)
#define mkInst(n)    (INSTMIN+(n))
#define instOf(c)    ((Int)((c)-INSTMIN))
#define inst(in)     tabInst[(in)-INSTMIN]

struct strInst {
    Class  c;                           /* class C                         */
    Int    line;
    Module mod;                         /* module that defines it          */
    Kinds  kinds;                       /* Kinds of variables in head      */
    Cell   head;                        /* :: Pred                         */
    List   specifics;                   /* :: [Pred]                       */
    Int    numSpecifics;                /* length(specifics)               */
    List   implements;
    Name   builder;                     /* Dictionary constructor function */
};

/* a predicate (an element :: Pred) is an application of a Class to one or
 * more type expressions
 */

#define CLASSMIN     (INSTMIN+NUM_INSTS)
#define isClass(c)   (CLASSMIN<=(c) && (c)<CHARMIN)
#define mkClass(n)   (CLASSMIN+(n))
#define cclass(n)    tabClass[(n)-CLASSMIN]

struct strClass {
    Text   text;                        /* Name of class                   */
    Int    line;                        /* Line where declaration begins   */
    Module mod;                         /* module that declares it         */
    Int    level;                       /* Level in class hierarchy        */
    Int    arity;                       /* Number of arguments             */
    Kinds  kinds;                       /* Kinds of constructors in class  */
    List   fds;				/* Functional Dependencies	   */
    List   xfds;			/* Xpanded Functional Dependencies */
    Cell   head;                        /* Head of class                   */
    Name   dcon;                        /* Dictionary constructor function */
    List   supers;                      /* :: [Pred]                       */
    Int    numSupers;                   /* length(supers)                  */
    List   dsels;                       /* Superclass dictionary selectors */
    List   members;                     /* :: [Name]                       */
    Int    numMembers;                  /* length(members)                 */
    List   defaults;                    /* :: [Name]                       */
    List   instances;                   /* :: [Inst]                       */
};

extern struct strClass    DECTABLE(tabClass);
extern struct strInst far *tabInst;

extern Class newClass      Args((Text));
extern Class classMax      Args((Void));
extern Class findClass     Args((Text));
extern Class addClass      Args((Class));
extern Class findQualClass Args((Cell));
extern Inst  newInst       Args((Void));
extern Inst  findFirstInst Args((Tycon));
extern Inst  findNextInst  Args((Tycon,Inst));
extern List getAllKnownTyconsAndClasses ( void );
extern Class findQualClassWithoutConsultingExportList ( QualId q );

/* --------------------------------------------------------------------------
 * Character values:
 * ------------------------------------------------------------------------*/

#define CHARMIN      (CLASSMIN+NUM_CLASSES)
#define MAXCHARVAL   (NUM_CHARS-1)
#define isChar(c)    (CHARMIN<=(c) && (c)<INTMIN)
#define charOf(c)    ((Char)(c-CHARMIN))
#define mkChar(c)    ((Cell)(CHARMIN+(((unsigned)(c))%NUM_CHARS)))

/* --------------------------------------------------------------------------
 * Small Integer values:
 * ------------------------------------------------------------------------*/

#define INTMIN       (CHARMIN+NUM_CHARS)
#define INTMAX       (MAXPOSINT)
#define isSmall(c)   (INTMIN<=(c))
#define INTZERO      (INTMIN/2 + INTMAX/2)
#define MINSMALLINT  (INTMIN - INTZERO)
#define MAXSMALLINT  (INTMAX - INTZERO)
#define mkDigit(c)   ((Cell)((c)+INTMIN))
#define digitOf(c)   ((Int)((c)-INTMIN))

extern  Bool isInt    Args((Cell));
extern  Int  intOf    Args((Cell));
extern  Cell mkInt    Args((Int));

/* --------------------------------------------------------------------------
 * Implementation of triples:
 * ------------------------------------------------------------------------*/

#define triple(x,y,z) pair(x,pair(y,z))
#define fst3(c)      fst(c)
#define snd3(c)      fst(snd(c))
#define thd3(c)      snd(snd(c))

/* --------------------------------------------------------------------------
 * Implementation of lists:
 * ------------------------------------------------------------------------*/

#define NIL          0
#define isNull(c)    ((c)==NIL)
#define nonNull(c)   (c)
#define cons(x,xs)   pair(x,xs)
#define singleton(x)     cons(x,NIL)
#define doubleton(x,y)   cons(x,cons(y,NIL))
#define tripleton(x,y,z) cons(x,cons(y,cons(z,NIL)))
#define hd(c)        fst(c)
#define tl(c)        snd(c)

extern  Int          length       Args((List));
extern  List         appendOnto   Args((List,List));    /* destructive     */
extern  List         dupOnto      Args((List,List));
extern  List         dupList      Args((List));
extern  List         revOnto      Args((List, List));   /* destructive     */
#define rev(xs)      revOnto((xs),NIL)                  /* destructive     */
#define reverse(xs)  revOnto(dupList(xs),NIL)           /* non-destructive */
extern  Cell         cellIsMember Args((Cell,List));
extern  Cell         cellAssoc    Args((Cell,List));
extern  Cell         cellRevAssoc Args((Cell,List));
extern  Bool         eqList       Args((List,List));
extern  Cell         varIsMember  Args((Text,List));
extern  Name         nameIsMember Args((Text,List));
extern  QualId       qualidIsMember ( QualId, List );
extern  Cell         intIsMember  Args((Int,List));
extern  List         replicate    Args((Int,Cell));
extern  List         diffList     Args((List,List));    /* destructive     */
extern  List         deleteCell   Args((List,Cell));    /* non-destructive */
extern  List         take         Args((Int,List));     /* destructive     */
extern  List         splitAt      Args((Int,List));     /* non-destructive */
extern  Cell         nth          Args((Int,List));
extern  List         removeCell   Args((Cell,List));    /* destructive     */
extern  List         dupListOnto  Args((List,List));    /* non-destructive */ 
extern  List         nubList      Args((List));         /* non-destructive */

/* The following macros provide `inline expansion' of some common ways of
 * traversing, using and modifying lists:
 *
 * N.B. We use the names _f, _a, _xs, Zs, in an attempt to avoid clashes
 *      with identifiers used elsewhere.
 */

#define mapBasic(_init,_step)           {List Zs=(_init);\
                                         for(;nonNull(Zs);Zs=tl(Zs))  \
                                         _step;}
#define mapModify(_init,_step)          mapBasic(_init,hd(Zs)=_step)

#define mapProc(_f,_xs)                 mapBasic(_xs,_f(hd(Zs)))
#define map1Proc(_f,_a,_xs)             mapBasic(_xs,_f(_a,hd(Zs)))
#define map2Proc(_f,_a,_b,_xs)          mapBasic(_xs,_f(_a,_b,hd(Zs)))
#define map3Proc(_f,_a,_b,_c,_xs)       mapBasic(_xs,_f(_a,_b,_c,hd(Zs)))
#define map4Proc(_f,_a,_b,_c,_d,_xs)    mapBasic(_xs,_f(_a,_b,_c,_d,hd(Zs)))

#define mapOver(_f,_xs)                 mapModify(_xs,_f(hd(Zs)))
#define map1Over(_f,_a,_xs)             mapModify(_xs,_f(_a,hd(Zs)))
#define map2Over(_f,_a,_b,_xs)          mapModify(_xs,_f(_a,_b,hd(Zs)))
#define map3Over(_f,_a,_b,_c,_xs)       mapModify(_xs,_f(_a,_b,_c,hd(Zs)))
#define map4Over(_f,_a,_b,_c,_d,_xs)    mapModify(_xs,_f(_a,_b,_c,_d,hd(Zs)))

/* This is just what you want for functions with accumulating parameters */
#define mapAccum(_f,_acc,_xs)           mapBasic(_xs,_acc=_f(_acc,hd(Zs)))
#define map1Accum(_f,_acc,_a,_xs)       mapBasic(_xs,_acc=_f(_acc,_a,hd(Zs)))
#define map2Accum(_f,_acc,_a,_b,_xs)    mapBasic(_xs,_acc=_f(_acc,_a,_b,hd(Zs)))
#define map3Accum(_f,_acc,_a,_b,_c,_xs) mapBasic(_xs,_acc=_f(_acc,_a,_b,_c,hd(Zs)))


/* --------------------------------------------------------------------------
 * Strongly-typed lists (z-lists) and tuples (experimental)
 * ------------------------------------------------------------------------*/

typedef Cell ZPair;
typedef Cell ZTriple;
typedef Cell Z4Ble;
typedef Cell Z5Ble;

#if 0
typedef Cell ZList;
extern Cell  zcons ( Cell x, Cell xs );
extern Cell  zhd ( Cell xs );
extern Cell  ztl ( Cell xs );
extern Cell  zsingleton ( Cell x );
extern Cell  zdoubleton ( Cell x, Cell y );
extern Int   zlength ( ZList xs );
extern ZList zreverse ( ZList xs );
#endif

extern Cell zpair ( Cell x1, Cell x2 );
extern Cell zfst ( Cell zpair );
extern Cell zsnd ( Cell zpair );

extern Cell ztriple ( Cell x1, Cell x2, Cell x3 );
extern Cell zfst3 ( Cell zpair );
extern Cell zsnd3 ( Cell zpair );
extern Cell zthd3 ( Cell zpair );

extern Cell z4ble ( Cell x1, Cell x2, Cell x3, Cell x4 );
extern Cell zsel14 ( Cell zpair );
extern Cell zsel24 ( Cell zpair );
extern Cell zsel34 ( Cell zpair );
extern Cell zsel44 ( Cell zpair );

extern Cell z5ble ( Cell x1, Cell x2, Cell x3, Cell x4, Cell x5 );
extern Cell zsel15 ( Cell zpair );
extern Cell zsel25 ( Cell zpair );
extern Cell zsel35 ( Cell zpair );
extern Cell zsel45 ( Cell zpair );
extern Cell zsel55 ( Cell zpair );

extern Cell unap ( int tag, Cell c );
#define isZPair(c) (whatIs((c))==ZTUP2)

/* --------------------------------------------------------------------------
 * Implementation of function application nodes:
 * ------------------------------------------------------------------------*/

#define ap(f,x)      pair(f,x)
#define ap1(f,x)     ap(f,x)
#define ap2(f,x,y)   ap(ap(f,x),y)
#define ap3(f,x,y,z) ap(ap(ap(f,x),y),z)
#define fun(c)       fst(c)
#define arg(c)       snd(c)
#define isAp(c)      (isPair(c) && !isTag(fst(c)))
extern  Cell         getHead     Args((Cell));
extern  List         getArgs     Args((Cell));
extern  Int          argCount;
extern  Cell         nthArg      Args((Int,Cell));
extern  Int          numArgs     Args((Cell));
extern  Cell         applyToArgs Args((Cell,List));

/* --------------------------------------------------------------------------
 * Stack implementation:
 *
 * NB: Use of macros makes order of evaluation hard to predict.
 *     For example, "push(1+pop());" doesn't increment TOS.
 * ------------------------------------------------------------------------*/

extern  Cell DECTABLE(cellStack);
extern  StackPtr sp;

#define clearStack() sp=(-1)
#define stackEmpty() (sp==(-1))
#define stack(p)     cellStack[p]
#define chkStack(n)  if (sp>=NUM_STACK-(n)) hugsStackOverflow()
#define push(c)      \
  do {               \
    chkStack(1);     \
    onto(c);         \
  } while (0)
#define onto(c)      stack(++sp)=(c);
#define pop()        stack(sp--)
#define drop()       sp--
#define top()        stack(sp)
#define pushed(n)    stack(sp-(n))
#define topfun(f)    top()=ap((f),top())
#define toparg(x)    top()=ap(top(),(x))

extern  Void hugsStackOverflow Args((Void));

#if SYMANTEC_C
#include <Memory.h>
#define STACK_HEADROOM 16384
#define STACK_CHECK if (StackSpace() <= STACK_HEADROOM) \
		      internal("Macintosh function parameter stack overflow.");
#else
#define STACK_CHECK
#endif

/* --------------------------------------------------------------------------
 * Script file control:
 * The implementation of script file storage is hidden.
 * ------------------------------------------------------------------------*/

extern Script      startNewScript   Args((String));
extern Bool        moduleThisScript Args((Module));
extern Module      moduleOfScript   Args((Script));
extern Bool        isPreludeScript  Args((Void));
extern Module      lastModule       Args((Void));
extern Script      scriptThisFile   Args((Text));
extern Script      scriptThisName   Args((Name));
extern Script      scriptThisTycon  Args((Tycon));
extern Script      scriptThisInst   Args((Inst));
extern Script      scriptThisClass  Args((Class));
extern String      fileOfModule     Args((Module));
extern Void        dropScriptsFrom  Args((Script));


/* --------------------------------------------------------------------------
 * Plugins
 * ------------------------------------------------------------------------*/

#if PLUGINS
/* This is an exact copy of the declaration found in GreenCard.h */

typedef int     HugsStackPtr;
typedef int     HugsStablePtr;
typedef Pointer HugsForeign;

typedef struct {

  /* evaluate next argument */
  int            (*getInt   )     Args(());  
  unsigned int   (*getWord  )     Args(());
  void*          (*getAddr  )     Args(());
  float          (*getFloat )     Args(());
  double         (*getDouble)     Args(());
  char           (*getChar  )     Args(());
  HugsForeign    (*getForeign)    Args(());
  HugsStablePtr  (*getStablePtr)  Args(());

  /* push part of result   */
  void           (*putInt   )     Args((int));           
  void           (*putWord  )     Args((unsigned int));
  void           (*putAddr  )     Args((void*));
  void           (*putFloat )     Args((double));
  void           (*putDouble)     Args((double));
  void           (*putChar  )     Args((char));
  void           (*putForeign)    Args((HugsForeign, void (*)(HugsForeign)));
  void           (*putStablePtr)  Args((HugsStablePtr));

  /* return n values in IO monad or Id monad */
  void           (*returnIO)      Args((HugsStackPtr, int));
  void           (*returnId)      Args((HugsStackPtr, int));
  int            (*runIO)         Args((int));

  /* free a stable pointer */                            
  void           (*freeStablePtr) Args((HugsStablePtr));

  /* register the prim table */                          
  void           (*registerPrims) Args((struct primInfo*));
                           
  /* garbage collect */
  void           (*garbageCollect) Args(());

} HugsAPI2;

extern  HugsAPI2* hugsAPI2     Args((Void));
typedef Void (*InitModuleFun2) Args((HugsAPI2*));

typedef struct {
  Name  nameTrue, nameFalse;
  Name  nameNil,  nameCons;
  Name  nameJust, nameNothing;
  Name  nameLeft, nameRight;
  Name  nameUnit;
  Name  nameIORun;

  Cell  (*makeInt)         Args((Int));
                           
  Cell  (*makeChar)        Args((Char));
  Char  (*CharOf)          Args((Cell));
                           
  Cell  (*makeFloat)       Args((FloatPro));
  Cell  (*makeTuple)       Args((Int));
  Pair  (*pair)            Args((Cell,Cell));
                           
  Cell  (*mkMallocPtr)     Args((Void *, Void (*)(Void *)));
  Void *(*derefMallocPtr)  Args((Cell));
                           
  Int   (*mkStablePtr)     Args((Cell));
  Cell  (*derefStablePtr)  Args((Int));
  Void  (*freeStablePtr)   Args((Int));
                           
  Void  (*eval)            Args((Cell));
  Cell  (*evalWithNoError) Args((Cell));
  Void  (*evalFails)       Args((StackPtr));
  Int   *whnfArgs;         
  Cell  *whnfHead;         
  Int   *whnfInt;          
  Float *whnfFloat;        
                           
  Void  (*garbageCollect)  Args(());
  Void  (*stackOverflow)   Args(());
  Void  (*internal)        Args((String)) HUGS_noreturn;

  Void  (*registerPrims)   Args((struct primInfo*));
  Name  (*addPrimCfun)     Args((Text,Int,Int,Cell));
  Text  (*inventText)      Args(());

  Cell *(*Fst)             Args((Cell));
  Cell *(*Snd)             Args((Cell));

  Cell  *cellStack;
  StackPtr *sp;
} HugsAPI1;

extern  HugsAPI1* hugsAPI1     Args((Void));
typedef Void (*InitModuleFun1) Args((HugsAPI1*));
#endif /* PLUGINS */


/* --------------------------------------------------------------------------
 * Misc:
 * ------------------------------------------------------------------------*/

extern  Void   setLastExpr       Args((Cell));
extern  Cell   getLastExpr       Args((Void));
extern  List   addTyconsMatching Args((String,List));
extern  List   addNamesMatching  Args((String,List));

extern Tycon findTyconInAnyModule ( Text t );
extern Class findClassInAnyModule ( Text t );
extern Name  findNameInAnyModule ( Text t );
extern Void  print Args((Cell, Int));
extern void dumpTycon ( Int t );
extern void dumpName ( Int n );
extern void dumpClass ( Int c );
extern void dumpInst ( Int i );
extern void locateSymbolByName ( Text t );

#if LEADING_UNDERSCORE
#define MAYBE_LEADING_UNDERSCORE(sss)     _##sss
#define MAYBE_LEADING_UNDERSCORE_STR(sss) "_" sss
#else
#define MAYBE_LEADING_UNDERSCORE(sss)     sss
#define MAYBE_LEADING_UNDERSCORE_STR(sss) sss
#endif

/*-------------------------------------------------------------------------*/
