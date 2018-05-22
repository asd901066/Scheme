# include <stdlib.h>
# include <stdio.h>
# include <cstring>
# include <ctype.h>
# include <iostream>
# include <string.h>
# include <string>
# include <vector>

using namespace std;

typedef char Str100[ 100 ] ;
int gTestNum ;

enum ErrorType{
  EXIT_ERROR = 1,  EOF_ERROR = 2,
  STRING_ERROR = 3, UNEXPECTED_ERROR = 4,
  LINE_ENTER_ERROR = 5, UNEXPECTED_ATOM_FOR_DOT_IN_TREE_ERROR = 6,
  UNBOUND_SYMBOL_ERROR = 7, NON_LIST_ERROR = 8,
  APPLY_NON_FUNCTION_ERROR = 9, LEVEL_CLEAN_ENVIRONMENT_ERROR = 10,
  LEVEL_DEFINE_ERROR = 11, LEVEL_EXIT_ERROR = 12,
  FORMAT_ERROR = 13, INVALID_ARG_NUMBER_ERROR = 14,
  NO_RETURN_VALUE_ERROR = 15, DIVISION_BY_ZERO_ERROR = 16,
  COND_NUM_ERROR = 17
};

enum TokenType {
  INT = 1, STRING = 2,
  DOT = 3, FLOAT = 4,
  NIL = 5, T = 6,
  QUOTE = 7, SYMBOL = 8,
  LEFTPAREN = 9, RIGHTPAREN = 10
} ; // TokenType

struct SyntaxError {
  int mLine ;
  int mColumn ;
  char mChar ;
  ErrorType mType ;
};

struct Tokendef{
  int mark  ;
  string sToken ;
  TokenType type ;
  Tokendef * left ;
  Tokendef * right ;
} ;

typedef Tokendef* TokenPtr ;

class Token {
  public :
  char mForRead ;
  SyntaxError mForSyntaxError ;
  TokenType mForJudge ;

  void GetOneChar() {
    char lastChar = mForRead ;
    mForRead = cin.get() ;

    if ( cin.eof() ) {
      mForSyntaxError.mType = EOF_ERROR ;
      throw mForSyntaxError ;
    } // if

    if ( lastChar == '\n' ) {
      mForSyntaxError.mLine++ ;
      mForSyntaxError.mColumn = 1 ;
    } // if()
    else {
      mForSyntaxError.mColumn++ ;
    } // else ()

    // cout << mForRead << " " << mForSyntaxError.mLine << " " << mForSyntaxError.mColumn << endl ;

  } // GetOneChar()

  void SetOneChar( char c ) {
    mForRead = c ;
  } // SetOneChar()

  void GetUselessToken() { // read out the useless before tokens
    do {
      GetOneChar() ;
       // if ( mForRead == ' ' && mForSyntaxError.mColumn == 1 && mForSyntaxError.mLine == 0 ) {
       //  mForSyntaxError.mColumn = 0 ;
       // } // if()
    } while ( mForRead == ' ' || mForRead == '\n' ) ;
  } // GetUselessToken()

  void GetSurplus() { // read out the useless between tokens
    while ( mForRead != '\n' ) {
      GetOneChar() ;
    } // while
  } // GetSurplus()

  void TypeJudge( string sToken ) {
    TokenType tType ;

    if ( sToken == "(" ) {
      tType = LEFTPAREN ;
    } // if
    else if ( sToken == ")" ) {
      tType = RIGHTPAREN ;
    } // else if
    else if ( sToken == "." ) {
      tType = DOT ;
    } // else if
    else if ( sToken == "\'" ) {
      tType = QUOTE ;
    } // else if
    else if ( sToken == "nil" || sToken == "#f" ) {
      tType = NIL ;
    } // else if
    else if ( sToken == "t" || sToken == "#t" ) {
      tType = T ;
    } // else if
    else if ( sToken[0] == '\"' ) {
      tType = STRING ;
    } // else if
    else if ( sToken[0] == '+' || sToken[0] == '-' ) {
      if ( sToken.length() == 1 )
        tType = SYMBOL ;
      else if ( sToken.length() == 2 )
        if ( isdigit( sToken[1] ) )
          tType = INT ;
        else
          tType = SYMBOL ;
      else {
        bool isDot = false ;
        bool isInt = false ;
        bool isSymbol = false ;
        for ( int i = 1 ; i < sToken.length() ; i++ ) {
          if ( isdigit( sToken[i] ) ) {
            isInt = true ;
          } // if
          else if ( sToken[i] == '.' && !isDot ) {
            isDot = true ;
          } // else if
          else {
            isSymbol = true ;
          } // else
        } // for

        if ( isInt && isDot && !isSymbol )
          tType = FLOAT ;
        else if ( isInt && !isDot && !isSymbol )
          tType = INT ;
        else
          tType = SYMBOL ;
      } // else
    } // else if
    else if ( sToken[0] == '.' ) {
      bool isInt = false ;
      for ( int i = 1 ; i < sToken.length() ; i++ ) {
        if ( isdigit( sToken[i] ) )
          isInt = true ;
        else
          isInt = false ;
      } // for

      if ( isInt ) {
        tType = FLOAT ;
        sToken = "0" + sToken ;
      } // if
      else
        tType = SYMBOL ;
    } // else if
    else if ( isdigit( sToken[0] ) ) {
      if ( sToken.length() == 1 )
        tType = INT ;
      else if ( sToken.length() == 2 ) {
        if ( sToken[1] == '.' )
          tType = FLOAT ;
        else if ( isdigit( sToken[1] ) )
          tType = INT ;
        else
          tType = SYMBOL ;
      } // else if
      else {
        bool isDot = false ;
        bool isInt = false ;
        for ( int i = 1 ; i < sToken.length() ; i++ ) {
          if ( isdigit( sToken[i] ) ) {
            isInt = true ;
          } // if
          else if ( sToken[i] == '.' && !isDot ) {
            isDot = true ;
          } // else if
          else {
            isInt = false ;
            isDot = false ;
          } // else
        } // for

        int dotNum = 0 ;

        for ( int i = 0 ; i < sToken.length() ; i++ )
          if ( sToken[i] == '.' )
            dotNum++ ;

        if ( isInt && isDot )
          tType = FLOAT ;
        else if ( isInt && !isDot )
          tType = INT ;
        else
          tType = SYMBOL ;

        if ( dotNum > 1 )
          tType = SYMBOL ;
      } // else
    } // else if
    else
      tType = SYMBOL ;


    mForJudge = tType ;
  } // TypeJudge()

  TokenPtr GetToken() {
    string sForRead ;
    TokenPtr temp = new Tokendef ;
    temp->left = NULL ;
    temp->right = NULL ;
    char cForPeek ;
    bool isGotten = false ;
    GetUselessToken() ;
    while ( mForRead == ';' ) {
      GetOneChar() ;
      while ( mForRead != '\n' ) {
        GetOneChar() ;
      } // while

      GetUselessToken() ;
    } // while

    if ( mForRead == '\"' ) {
      sForRead += mForRead ;
      do {
        cForPeek = cin.peek() ;
        if ( cForPeek == '\\' ) {
          GetOneChar() ;
          cForPeek = cin.peek() ;
          if ( cForPeek == '\\' ) {
            GetOneChar() ;
            SetOneChar( '\\' ) ;
          } // if
          else if ( cForPeek == 'n' ) {
            GetOneChar() ;
            SetOneChar( '\n' ) ;
          } // else if
          else if ( cForPeek == 't' ) {
            GetOneChar() ;
            SetOneChar( '\t' ) ;
          } // else if
          else if ( cForPeek == '\"' ) {
            GetOneChar() ;
            SetOneChar( '\"' ) ;
          } // else if
        } // if
        else if ( cForPeek == '\n' ) {
          mForSyntaxError.mType = STRING_ERROR ;
          throw mForSyntaxError ;
        } // else if
        else if ( cForPeek != '\"' )
          GetOneChar() ;
        else {
          isGotten = true ;
          GetOneChar() ;
        } // else

        sForRead += mForRead ;
      } while ( !isGotten ) ;
    } // if
    else if ( mForRead == '(' ) {
      sForRead += mForRead ;
    } // else if
    else if ( mForRead == ')' ) {
      sForRead += mForRead ;
    } // else if
    else if ( mForRead == '\'' ) {
      sForRead += mForRead ;
    } // else if
    else {
      while ( !isGotten ) {

        sForRead += mForRead ;
        cForPeek = cin.peek() ;
        if ( cForPeek == ' ' )
          isGotten = true ;
        else if ( cForPeek == '(' )
          isGotten = true ;
        else if ( cForPeek == ')' )
          isGotten = true ;
        else if ( cForPeek == '\'' )
          isGotten = true ;
        else if ( cForPeek == '\"' )
          isGotten = true ;
        else if ( cForPeek == '\n' )
          isGotten = true ;
        else if ( cForPeek == ';' )
          isGotten = true ;
        else
          GetOneChar() ;


      } // while
    } // else

    TypeJudge( sForRead ) ;
    if ( mForJudge == INT )
      sForRead = TransToINT( sForRead ) ;
    else if ( mForJudge == FLOAT )
      sForRead = TransToFLOAT( sForRead ) ;
    else if ( mForJudge == T )
      sForRead = "#t" ;
    else if ( mForJudge == NIL )
      sForRead = "nil" ;
    else if ( mForJudge == QUOTE )
      sForRead = "quote" ;

    temp->sToken = sForRead ;
    temp->type = mForJudge ;
    temp->mark = 0 ;
    return temp ;
  } // GetToken()

  TokenPtr SetToken( string theToken ) {
    TokenPtr temp = new Tokendef ;
    temp->mark = 0 ;
    temp->left = NULL ;
    temp->right = NULL ;
    temp->sToken = theToken ;
    if ( theToken == "(" )
      temp->type = LEFTPAREN ;
    else if ( theToken == ")" )
      temp->type = RIGHTPAREN ;
    else if ( theToken == "quote" )
      temp->type = SYMBOL ;

    return temp ;
  } // SetToken()

  string TransToINT( string sForTrans ) {
    Str100 tTrans ;
    int newInt = atoi( sForTrans.c_str() ) ;
    sprintf( tTrans, "%d", newInt ) ;
    string sForReturn( tTrans ) ;
    return sForReturn ;
  } // TransToINT()

  string TransToFLOAT( string sForTrans ) {
    Str100 tTrans ;
    float newfloat = atof( sForTrans.c_str() ) ;
    sprintf( tTrans, "%0.3f", newfloat ) ;
    string sForReturn( tTrans ) ;
    return sForReturn ;
  } // TransToFLOAT()
};

enum TreeType {
  LIST = 1, EXIT = 2,
  ATOM_ONE = 3, ATOM_TWO = 4,
  DOTTED_PAIR = 5
}; //  ----------- Project Two -------------- //

struct EvalError {
  string mErrorToken ;
  TokenPtr mErrorTree ;
  ErrorType mType ;
}; //  ----------- Project Two -------------- //

struct Treedef{
  TokenPtr mTree ;
  TreeType mTypeOfTree ;
};  //  ----------- Project Two -------------- //

struct Binding_Symbol{
  int mLocation ;
  TokenPtr mForBindingThing ;
  TokenPtr mForBindingNode ;
};

struct Binding_Function{
  string mName ;
  TokenPtr mForFunctionThing ;
  TokenPtr mForFunctionNode ;
};

class Trees {
  public :
  Token mForTree ;
  EvalError mForEvalError ;
  vector< Binding_Symbol > mForUserBinding ;
  vector< Binding_Symbol > mLetUserBound ;
  vector< vector< Binding_Symbol > > mForAllUserBinding ;
  vector< Binding_Function > mForUserFunction ;
  Binding_Function mLambdaFunction ;
  bool mForGetUserNum ;
  bool mForGetLetNum ;


  //  ----------- Project One -------------- //

  void GetTree() {
    // project one

    TokenPtr root = new Tokendef ;
    mForTree.mForSyntaxError.mColumn = 0 ;
    mForTree.mForSyntaxError.mLine = 0 ;
    int mCheckLine = mForTree.mForSyntaxError.mLine ;
    int mCheckColumn = mForTree.mForSyntaxError.mColumn ;

    // project two

    Treedef  mForProjectTwo ;

    root = mForTree.GetToken() ;


    if ( mForTree.mForSyntaxError.mLine == mCheckLine ) {
      // cout << "this is test equal !\n" ;
      // cout << mForTree.mForSyntaxError.mLine << endl ;
      // cout << mCheckLine << endl ;
      mForTree.mForSyntaxError.mLine = 1 ;
      mForTree.mForSyntaxError.mColumn -= mCheckColumn ;
    } // if()
    /*

    else if ( mForTree.mForSyntaxError.mLine > mCheckLine ) {
      // cout << "this is test bigger !\n" ;
      // cout << mForTree.mForSyntaxError.mLine << endl ;
      // cout << mCheckLine << endl ;
      // mForTree.mForSyntaxError.mLine -= mCheckLine ;
    } // else if()

    */

    // cout << root->sToken << " " << root->type << " !!!!!\n" ;

    if ( root->type == INT || root->type == FLOAT || root->type == STRING ||
         root->type == T || root->type == NIL ) {
      // cout << "ATOM_ONE\n" ;
      mForProjectTwo.mTree = root ;
      mForProjectTwo.mTypeOfTree = ATOM_ONE ;
      throw mForProjectTwo ;
    } // if()
    else if ( root->type == SYMBOL ) {
      mForProjectTwo.mTree = Search_Symbol( root ) ;
      mForProjectTwo.mTypeOfTree = ATOM_TWO ;
      throw mForProjectTwo ;
    } // else if
    else if ( root->type == LEFTPAREN ) {
      BuildTree( root ) ;
      if ( root->type != NIL ) {
        Check_Whether_Is_An_End_Tree( root, mForProjectTwo ) ;
        mForProjectTwo.mTypeOfTree = DOTTED_PAIR ;
        mForProjectTwo.mTree = Turn_Into_Project_Two( root ) ;
        throw mForProjectTwo ;
      } // if()
    } // else if()
    else if ( root->type == QUOTE ) {
      root = new Tokendef ;
      root = BuildQTree() ;
      mForProjectTwo.mTree = Turn_Into_Project_Two( root ) ;
      mForProjectTwo.mTypeOfTree = LIST ;
      throw mForProjectTwo ;
    } // else if
    else {
      if ( root->type == RIGHTPAREN ) {
        mForTree.mForSyntaxError.mType = UNEXPECTED_ATOM_FOR_DOT_IN_TREE_ERROR ;
        mForTree.mForSyntaxError.mChar = ')' ;
        throw mForTree.mForSyntaxError ;
      } // if()
      else if ( root->type == DOT ) {
        mForTree.mForSyntaxError.mType = UNEXPECTED_ERROR ;
        mForTree.mForSyntaxError.mChar = '.' ;
        throw mForTree.mForSyntaxError ;
      } // else if
    } // else

    throw root ;
  } // GetTree()

  void BuildTree( TokenPtr root ) {
    TokenPtr leaf = mForTree.GetToken() ;
    TokenPtr quLeaf = new Tokendef ;
    bool isDot = true ;
    if ( leaf->sToken != ")" ) {

      if ( leaf->type != DOT ) {
        isDot = false ;
      } // if()
      else if ( leaf->type == DOT ) {
        isDot = true ;
        if ( root->left != NULL && root->right == NULL ) {
          leaf = mForTree.GetToken() ;
          if ( leaf->type == RIGHTPAREN ) {
            mForTree.mForSyntaxError.mType = UNEXPECTED_ATOM_FOR_DOT_IN_TREE_ERROR ;
            mForTree.mForSyntaxError.mChar = ')' ;
            throw mForTree.mForSyntaxError ;
          } // if()
        } // if()
        else if ( root->left != NULL && root->right != NULL ) {
          mForTree.mForSyntaxError.mType = LINE_ENTER_ERROR ;
          mForTree.mForSyntaxError.mChar = '.' ;
          throw mForTree.mForSyntaxError ;
        } // else if()
        else {
          char q = cin.peek() ;
          if ( q != '\n' )
            mForTree.mForSyntaxError.mType = UNEXPECTED_ATOM_FOR_DOT_IN_TREE_ERROR ;
          else
            mForTree.mForSyntaxError.mType = LINE_ENTER_ERROR ;

          mForTree.mForSyntaxError.mChar = '.' ;
          throw mForTree.mForSyntaxError ;
        } // else
      } // else if()

      if ( leaf->type == QUOTE ) {
        if ( root->left != NULL && root->right != NULL )  {
          mForTree.mForSyntaxError.mType = UNEXPECTED_ERROR ;
          mForTree.mForSyntaxError.mChar = '\'' ;
          throw mForTree.mForSyntaxError ;
        } // if()
        else
          quLeaf = BuildQTree() ;
      } // if()

      if ( root->left == NULL ) {
        if ( leaf->type == QUOTE )
          root->left = quLeaf ;
        else {
          root->left = leaf ;
          if ( root->left->sToken == "(" ) {
            BuildTree( root->left ) ;
          } // if()
        } // else
      } // if()
      else if ( root->right == NULL ) {
        if ( !isDot ) {
          TokenPtr newLeaf = new Tokendef ;
          newLeaf->left = NULL;
          newLeaf->right = NULL ;
          newLeaf->sToken = "(" ;
          newLeaf->type = LEFTPAREN ;

          if ( leaf->type == QUOTE )
            newLeaf->left = quLeaf ;
          else
            newLeaf->left = leaf ;

          root->right = newLeaf ;
          root = root->right ;
          if ( leaf->type != QUOTE )
            if ( root->left->sToken == "(" )
              BuildTree( root->left ) ;
        } // if()
        else if ( isDot ) {
          if ( leaf->type == QUOTE )
            root->right = quLeaf ;
          else
            root->right = leaf ;

          if ( leaf->type == LEFTPAREN )
            BuildTree( root->right ) ;
        } // else if()
      } // else if()
      else {
        mForTree.mForSyntaxError.mType = LINE_ENTER_ERROR ;
        mForTree.mForSyntaxError.mChar = leaf->sToken[0] ;
        throw mForTree.mForSyntaxError ;
      } // else

      BuildTree( root ) ;
    } // if()
    else {  // leaf->token = ")"
      if ( root->left == NULL && root->sToken == "(" ) {
        root->sToken = "nil" ;
        root->type = NIL ;
      } // if()
      else {
        if ( root->right == NULL ) {
          TokenPtr useToERROR = new Tokendef ;
          useToERROR->sToken = ")" ;
          useToERROR->type = RIGHTPAREN ;
          useToERROR->left = NULL ;
          useToERROR->right = NULL ;
          root->right = useToERROR ;
        } // if()
      } // else
    } // else


  } // BuildTree()

  TokenPtr BuildQTree() {
    TokenPtr rightOne = new Tokendef ; // )
    rightOne = mForTree.SetToken( ")" ) ;
    TokenPtr leftTwo = new Tokendef ;  // (
    leftTwo = mForTree.SetToken( "(" ) ;
    TokenPtr quotee = new Tokendef ;   // quote
    quotee = mForTree.SetToken( "quote" ) ;
    TokenPtr leftOne = new Tokendef ;  // (
    leftOne = mForTree.SetToken( "(" ) ;
    TokenPtr getOneTree = mForTree.GetToken() ;
    TokenPtr getOne = new Tokendef ;

    leftTwo->right = rightOne ;       //   (          //
    leftOne->left = quotee ;         //   /  \        //
    leftOne->right = leftTwo ;      // quote  (       //
    //                                       /  \     //
    //                                    tree    )   //

    if ( OutQTree( getOneTree ) ) {
      getOne->sToken = getOneTree->sToken ;
      getOne->type = getOneTree->type ;
      getOne->left = NULL ;
      getOne->right = NULL ;
      if ( getOneTree->type == LEFTPAREN )
        BuildTree( getOne ) ;
      else if ( getOneTree->type == QUOTE ) {
        getOne = new Tokendef ;
        getOne = BuildQTree() ;
      } // else if()

      leftTwo->left = getOne ;
      return leftOne ;
    } // if
    else
      return NULL ;


  } // BuildQTree()

  bool OutQTree( TokenPtr root ) {
    if ( root->type != RIGHTPAREN && root->type != DOT )
      return true ;
    else {
      char checkError = cin.peek() ;
      if ( root->type == RIGHTPAREN ) {
        mForTree.mForSyntaxError.mType = UNEXPECTED_ERROR ;
        mForTree.mForSyntaxError.mChar = ')' ;
        throw mForTree.mForSyntaxError ;
      } // if()

      if ( checkError == '\n' ) {
        mForTree.mForSyntaxError.mType = LINE_ENTER_ERROR ;
        throw mForTree.mForSyntaxError ;
      } // if()

      mForTree.mForSyntaxError.mType = UNEXPECTED_ERROR ;
      mForTree.mForSyntaxError.mChar = checkError ;
      throw mForTree.mForSyntaxError ;

    } // else
  } // OutQTree()

  void PrintTree( TokenPtr root, bool isLeft, int TreeHieght ) {
    if ( root != NULL ) {
      if ( !isLeft ) {
        cout << endl ;
        if ( root->type != RIGHTPAREN ) {   // handle this situation like ( 1 . 2 )
          if ( root->type != NIL ) {
            for ( int i = 0 ; i < TreeHieght ; i++ )
              cout << "  " ;
            if ( root->type != LEFTPAREN ) {
              cout << "." << endl ;
              for ( int i = 0 ; i < TreeHieght ; i++ )
                cout << "  " ;
              PrintSpecialTree( root ) ;
              cout << endl ;
              for ( int i = 0 ; i < TreeHieght-1 ; i++ )
                cout << "  " ;
              cout << ")" ;
            } // if()
          } // if()
          else {
            for ( int i = 0 ; i < TreeHieght-1 ; i++ )
              cout << "  " ;
            cout << ")" ;
          } // else
        } // if()
        else {
          for ( int i = 0 ; i < TreeHieght-1 ; i++ )
            cout << "  " ;
          cout << ")" ;
        } // else
      } // if()
      else {
        PrintSpecialTree( root )  ;
        if ( root->type == LEFTPAREN )
          cout << " " ;
      } // else

      PrintTree( root->left, true, TreeHieght+1 ) ;
      PrintTree( root->right, false, TreeHieght ) ;
    } // if
  } // PrintTree()

  void PrintSpecialTree( TokenPtr root ) {
    if ( SymbolBinding( root->sToken ) ) {
      if ( root->mark == 0 )
        cout << "#<procedure " << root->sToken << ">" ;
      else
        cout << root->sToken ;
    } // if()
    else {
      cout << root->sToken ;
    } // else
  } // PrintSpecialTree()

  void PrintPureTree( TokenPtr root, bool isLeft, int TreeHieght ) {
    if ( root != NULL ) {
      if ( !isLeft ) {
        cout << endl ;
        if ( root->type != RIGHTPAREN ) {   // handle this situation like ( 1 . 2 )
          if ( root->type != NIL ) {
            for ( int i = 0 ; i < TreeHieght ; i++ )
              cout << "  " ;
            if ( root->type != LEFTPAREN ) {
              cout << "." << endl ;
              for ( int i = 0 ; i < TreeHieght ; i++ )
                cout << "  " ;
              cout << root->sToken ;
              cout << endl ;
              for ( int i = 0 ; i < TreeHieght-1 ; i++ )
                cout << "  " ;
              cout << ")" ;
            } // if()
          } // if()
          else {
            for ( int i = 0 ; i < TreeHieght-1 ; i++ )
              cout << "  " ;
            cout << ")" ;
          } // else
        } // if()
        else {
          for ( int i = 0 ; i < TreeHieght-1 ; i++ )
            cout << "  " ;
          cout << ")" ;
        } // else
      } // if()
      else {
        cout << root->sToken  ;
        if ( root->type == LEFTPAREN )
          cout << " " ;
      } // else

      PrintPureTree( root->left, true, TreeHieght+1 ) ;
      PrintPureTree( root->right, false, TreeHieght ) ;
    } // if
  } // PrintPureTree()

  //  ----------- Project Two -------------- //

  vector< TokenPtr > ArithAssistant( string RunType, TokenType tType,
                                          TokenPtr root, vector< TokenPtr > vvAriTree ) {
    int iArithType = ArithDetection( RunType ) ;
    if ( iArithType == 4 || root->sToken != "(" ) {
      if ( iArithType == 1 ) {
        if ( tType == INT || tType == FLOAT )
          vvAriTree.push_back( root );
        else {
          CatchError( root, RunType ) ;
        } // else
      } // if()
      else if ( iArithType == 2 ) {
        if ( tType == STRING )
          vvAriTree.push_back( root ) ;
        else {
          CatchError( root, RunType ) ;
        } // else
      } // else if()
      else if ( iArithType == 3 || iArithType == 4 ) {
        vvAriTree.push_back( root ) ;
      } // else if()
    } // if()
    else if ( iArithType == 3 )
      vvAriTree.push_back( root ) ;
    else {
      CatchError( root, RunType ) ;
    } // else

    return vvAriTree ;
  } // ArithAssistant()

  bool SymbolBinding( string FunctionName ) {
    if ( FunctionName == "cons" || FunctionName == "list" || FunctionName == "quote" ||
         FunctionName == "define" || FunctionName == "clean-environment" || FunctionName == "car" ||
         FunctionName == "cdr" || FunctionName == "equal?" || FunctionName == "eqv?" ||
         FunctionName == "cond" || FunctionName == "if" )
      return true ;
    else if ( PrimDetection( FunctionName ) || ArithDetection( FunctionName ) != 0 )
      return true ;
    else
      return false ;
  } // SymbolBinding()

  bool PrimDetection( string FunctionName ) {
    if ( FunctionName == "pair?" || FunctionName == "null?" || FunctionName == "integer?" ||
         FunctionName == "real?" || FunctionName ==  "number?" || FunctionName == "string?" ||
         FunctionName == "boolean?" || FunctionName == "symbol?" )  {
      return true ;
    } // if()
    else
      return false ;
  } // PrimDetection()

  bool Body_Equal( TokenPtr root, TokenPtr thing ) {
    vector < Binding_Symbol > a ;
    TokenPtr use = new Tokendef, sRoot = root, sthing = thing ;
    Binding_Symbol save ;
    if ( thing->type != NIL ) {
      while ( root->sToken != ")" && thing->sToken != ")" ) {
        save.mForBindingThing = thing->left ;
        if ( root->left->sToken == "(" ) {
          Error_Time_Reading( root->left->left->sToken ) ;
          TokenPtr tWork = Turn_Into_Project_Two( root->left ) ;
          use = tWork ;
        } // if()
        else if ( root->left->type == SYMBOL )
          use = Search_Symbol( root->left ) ;
        else
          use = root->left ;

        save.mForBindingNode = use ;
        a.push_back( save ) ;
        root = root->right ;
        thing = thing->right ;
      } // while()
    } // if()

    mForAllUserBinding.push_back( a ) ;
    if ( root->sToken == ")" && thing->sToken == ")" ) {
      root = sRoot ;
      thing = sthing ;
      return true ;
    } // if()
    else if ( root->sToken == ")" && thing->type == NIL )
      return true ;
    else {
      root = sRoot ;
      thing = sthing ;
      return false ;
    } // else
  } // Body_Equal()


  int ArithDetection( string FunctionName ) {
    int num = 0 ;
    if ( FunctionName == "+" || FunctionName == "-" || FunctionName == "*" ||
         FunctionName == "/" || FunctionName == ">" || FunctionName == "<" ||
         FunctionName == "<=" || FunctionName == ">=" || FunctionName == "=" ) {
      num = 1 ;
    } // if()
    else if ( FunctionName == "string-append" || FunctionName == "string>?" ||
              FunctionName == "string<?" || FunctionName == "string=?" )
      num = 2 ;
    else if ( FunctionName == "not" || FunctionName == "and" || FunctionName == "or" )
      num = 3 ;
    else if ( FunctionName == "begin" )
      num = 4 ;

    return num ;
  } // ArithDetection()

  int Take_Binding_Symbol_Num( string FunctionName ) {
    int num = -1 ;
    mForGetUserNum = false ;

    int iUserNum = Take_User_Symbol_Num( FunctionName ) ;
    if ( iUserNum != -1 ) {
      num = iUserNum ;
      mForGetUserNum = true ;
    } // if()

    if ( !mForGetUserNum ) {
      for ( int i = 0 ; i < mForUserBinding.size() ; i++ ) {
        if ( mForUserBinding[i].mForBindingThing->sToken == FunctionName )
          num = i ;
      } // for()

    } // if()

    return num ;
  } // Take_Binding_Symbol_Num()

  int Take_User_Symbol_Num( string FunctionName ) {
    int num = -1 ;
    vector < Binding_Symbol > save ;
    if ( mForAllUserBinding.size() != 0 ) {
      save = mForAllUserBinding.back() ;
      for ( int i = 0; i < save.size() ; i++ ) {
        if ( save[i].mForBindingThing->sToken == FunctionName )
          num = i ;
      } // for
    } // if()

    return num ;
  } // Take_User_Symbol_Num()

  int Take_User_Function_Num( string FunctionName ) {
    int num = -1 ;
    for ( int i = 0 ; i < mForUserFunction.size() ; i++ ) {
      if ( mForUserFunction[i].mName == FunctionName ) {
        num = i ;
      } // if()
    } // for()

    return num ;
  } // Take_User_Function_Num()

  int Check_Number_Error( TokenPtr root, string FunctionName, string name ) {
    int num = 0 ;
    TokenPtr tBaseNode = root ;
    mForEvalError.mType = INVALID_ARG_NUMBER_ERROR ;
    tBaseNode = tBaseNode->right ;
    for ( bool isErrors = false ; tBaseNode != NULL ; tBaseNode = tBaseNode->right ) {
      num++ ;
      if ( tBaseNode->type == RIGHTPAREN ) {
         // cout << "Yes?\n" ;
         // cout << FunctionName ;
         // cout << ArithDetection( FunctionName ) ;
        if ( FunctionName == "cons" || FunctionName == "eqv?" || FunctionName == "equal?" ) {
          // cout << "cons\n" ;
          if ( num != 3 ) {
            isErrors = true ;
            mForEvalError.mErrorToken = name ;
          } // if()
        } // if()
        else if ( FunctionName == "cond_in_two" ) {
          if ( num == 1 ) {
            isErrors = true ;
            mForEvalError.mType = COND_NUM_ERROR ;
            mForEvalError.mErrorTree = tBaseNode ;
          } // if()
        } // else if()
        else if ( FunctionName == "cond" || FunctionName == "cond_in" ) {

        } // else if()
        else if ( FunctionName == "list" || FunctionName == "begin" ) {
          // cout << "list" ;
          if ( num == 1 ) {
            isErrors = true ;
            mForEvalError.mErrorToken = name ;
          } // if()
        } // else if()
        else if ( FunctionName == "define" ) {
          // cout << "define" ;
          if ( num != 3 ) {
            isErrors = true ;
            mForEvalError.mType = FORMAT_ERROR ;
            mForEvalError.mErrorToken = "define" ;
          } // if()
        } // else if()
        else if ( FunctionName == "car" || FunctionName == "cdr" || FunctionName == "quote"
                  || FunctionName == "not" || PrimDetection( FunctionName ) ) {
          if ( num != 2 ) {
            isErrors = true ;
            mForEvalError.mErrorToken = name ;
          } // if()
        } // else if()
        else if ( ArithDetection( FunctionName ) != 0 ) {
          // cout << "ari" ;
          if ( num == 1 || num == 2 ) {
            isErrors = true ;
            mForEvalError.mErrorToken = name ;
          } // if()
        } // else if()
        else if ( FunctionName == "clean-environment" ) {
          if ( num != 1 ) {
            isErrors = true ;
            mForEvalError.mErrorToken = "clean_environment" ;
          } // if()
        } // else if()
        else if ( FunctionName == "if" ) {
          // cout << "if" ;
          if ( num != 4 && num != 3 ) {
            isErrors = true ;
            mForEvalError.mErrorToken = name ;
          } // if()
        } // else if()

        if ( isErrors )
          throw mForEvalError ;

      } // if()
    } // for()

    return num ;
  } // Check_Number_Error()

  int Take_Binding_Memory_Num( int num ) {
    return mForUserBinding[num].mLocation ;
  } // Take_Binding_Memory_Num()

  int Cond_Layers( TokenPtr root ) {
    int num = 1 ;
    TokenPtr tBaseTree = new Tokendef ;
    tBaseTree = root->right ;

    for ( ; tBaseTree != NULL ; tBaseTree = tBaseTree->right ) {
      num++ ;
    } // for()

    return num-1 ;

  } // Cond_Layers()


  TokenPtr Turn_Into_Project_Two( TokenPtr root ) {
    TokenPtr tTransTree = new Tokendef ;
    root = Check_Project_Two_Type( root ) ;

    if ( root->left->type != LEFTPAREN ) {
      if ( root->left->type != SYMBOL ) {
        mForEvalError.mType = APPLY_NON_FUNCTION_ERROR ;
        mForEvalError.mErrorTree = root->left ;
        throw mForEvalError ;
      } // if()
      else {

        string ssBaseName = root->left->sToken ;
        root->left = Search_Symbol( root->left ) ;
        if ( !SymbolBinding( root->left->sToken ) ) {
          if ( Take_User_Function_Num( root->left->sToken ) == -1 ) {
            mForEvalError.mType = UNBOUND_SYMBOL_ERROR ;
            mForEvalError.mErrorToken = root->left->sToken ;
            mForEvalError.mErrorTree = root->left ;
            throw mForEvalError ;
          } // if()
          else {
            tTransTree = RunUserDefinition( root, ssBaseName ) ;
          } // else
        } // if()
        else {
          string sCheckName = root->left->sToken ;
          if ( sCheckName == "cons" || sCheckName == "equal?" || sCheckName == "eqv?" ) {
            tTransTree = Run_Cons_Eqv_Equal( root, sCheckName, ssBaseName );
          } // if()
          else if ( sCheckName == "list" ) {
            // cout << "Call list Function!!\n\n" ;
            tTransTree = Run_List_Arith( root, sCheckName, ssBaseName );
          } // else if()
          else if ( sCheckName == "quote" ) {
            // cout << "Call quote Function!!\n\n" ;
            tTransTree = Run_Quote( root, ssBaseName );
          } // else if()
          else if ( sCheckName == "car" || sCheckName == "cdr" ) {
            // cout << "Call car_cdr Function!!\n\n" ;
            tTransTree = Run_Car_Cdr( root, ssBaseName );
          } // else if()
          else if ( sCheckName == "define" ) {
            // cout << "Call define Function!!\n\n" ;
            Run_Define( root ) ;
          } // else if()
          else if ( sCheckName == "clean-environment" ) {
            // cout << "Call Clean-Environment Function!!\n\n" ;
            Run_Clean( root );
          } // else if()
          else if ( sCheckName == "cond" ) {
            ;
          } // else if()
          else if ( sCheckName == "if" ) {
            cout << "Call If Function!!\n\n" ;
            tTransTree = Run_If( root, sCheckName, ssBaseName );
          } // else if()
          else if (  ArithDetection( sCheckName ) != 0 ) {
            // cout << "Call Arithmetic Function!!\n\n" ;
            tTransTree = Run_List_Arith( root, sCheckName, ssBaseName );
          } // else if()
          else if ( PrimDetection( sCheckName ) ) {
            // cout << "Call PrimDetection Function!!\n\n" ;
            tTransTree = Run_Primitive( root, sCheckName, ssBaseName );
          } // else if()
        } // else
      } // else()
    } // if()

    return tTransTree ;
  } // Turn_Into_Project_Two()

  TokenPtr Check_Project_Two_Type( TokenPtr root ) {
    if ( root->left->type == LEFTPAREN ) {
      Error_Time_Reading( root->left->left->sToken ) ;
      TokenPtr tLeftTree = Turn_Into_Project_Two( root->left ) ;
      mForEvalError.mType = APPLY_NON_FUNCTION_ERROR ;
      mForEvalError.mErrorTree = tLeftTree ;
      if ( SymbolBinding( tLeftTree->sToken ) ) {
        if ( tLeftTree->mark == 0 )
          root->left = tLeftTree ;
        else
          throw mForEvalError ;
      } // if()
      else {
        if ( Take_User_Function_Num( tLeftTree->sToken ) != -1 )
          root->left = tLeftTree ;
        else
          throw mForEvalError ;
      } // else
    } // if()
    else if ( root->left->type == SYMBOL ) {
      TokenPtr tSymbolTree = Search_Symbol( root->left ) ;
      if ( tSymbolTree->type != SYMBOL ) {
        mForEvalError.mType = APPLY_NON_FUNCTION_ERROR ;
        mForEvalError.mErrorTree = tSymbolTree ;
        throw mForEvalError ;
      } // if()
      else if ( SymbolBinding( tSymbolTree->sToken ) ) {
        if ( tSymbolTree->mark != 0 ) {
          mForEvalError.mType = APPLY_NON_FUNCTION_ERROR ;
          mForEvalError.mErrorTree = tSymbolTree ;
          throw mForEvalError ;
        } // if()
      } // else if()
    } // else if()

    return root ;
  } // Check_Project_Two_Type()

  TokenPtr Search_Symbol( TokenPtr root ) {
    int num = Take_Binding_Symbol_Num( root->sToken ) ;
    if ( num == -1 ) {
      if ( !SymbolBinding( root->sToken ) ) {
        mForEvalError.mType = UNBOUND_SYMBOL_ERROR ;
        mForEvalError.mErrorToken = root->sToken ;
        throw mForEvalError ;
      } // if()
      else
        return root ;
    } // if()
    else
      return Take_The_Bound_Node( num ) ;
  } // Search_Symbol()

  TokenPtr RunUserDefinition( TokenPtr root, string FunctionName ) {
    int num_save = mForAllUserBinding.size() ;
    TokenPtr out = new Tokendef, thing = new Tokendef, sthing = new Tokendef ;
    TokenPtr tfunction = new Tokendef, sfunction = new Tokendef ;
    tfunction->left = NULL, tfunction->right = NULL, tfunction->sToken = "(", tfunction->type = LEFTPAREN ;
    string s_name = root->left->sToken ;
    bool watch_equal = false ;
    root = root->right ;
    if ( s_name != "#<user function>" ) {
      thing = mForUserFunction[ Take_User_Function_Num( s_name ) ].mForFunctionThing ;
      sthing = thing ;
      sfunction = mForUserFunction[ Take_User_Function_Num( s_name ) ].mForFunctionNode ;
      Copy_Tree( tfunction, sfunction, 0 ) ;
    } // if()

    try {
      watch_equal = Body_Equal( root, thing ) ;
      if ( watch_equal == true ) {
        if ( tfunction->sToken == "(" ) {
          Error_Time_Reading( tfunction->left->sToken ) ;
          TokenPtr tWork = Turn_Into_Project_Two( tfunction ) ;
          out = tWork ;
        } // if
        else if ( tfunction->type == SYMBOL )
          out = Search_Symbol( tfunction ) ;
        else
          out = tfunction ;
      } // if()
      else {
        mForEvalError.mType = INVALID_ARG_NUMBER_ERROR ;
        mForEvalError.mErrorToken = FunctionName ;
        throw mForEvalError ;
      } // else
    } // try
    catch ( EvalError t ) {
      if ( mForAllUserBinding.size() == num_save + 1 )
        mForAllUserBinding.pop_back() ;
      throw t ;
    } // catch
    catch ( SyntaxError e ) {
      if ( mForAllUserBinding.size() == num_save + 1 )
        mForAllUserBinding.pop_back() ;
      throw e ;
    } // catch

    mForAllUserBinding.pop_back() ;
    if ( s_name != "#<user function>" ) {
      mForUserFunction[ Take_User_Function_Num( s_name ) ].mForFunctionThing = sthing ;
    } // if()

    return out ;

  } // RunUserDefinition()

  TokenPtr Take_The_Bound_Node( int num ) {
    // cout << "Get in \"Take_The_Bound_Node\" !!!!\n" ;
    // cout << mForGetUserNum << endl ;
    // cout << mForUserBinding[num].mForBindingNode->type << endl << endl ;
    if ( mForGetUserNum )
      return mForAllUserBinding.back()[num].mForBindingNode ;
    else
      return mForUserBinding[num].mForBindingNode ;
  } // Take_The_Bound_Node()

  TokenPtr Run_Cons_Eqv_Equal( TokenPtr root, string RunType, string FunctionName ) {
    Check_Number_Error( root, RunType, FunctionName ) ;
    TokenPtr out = new Tokendef ;
    TokenPtr tBaseTree = new Tokendef ;
    bool isUserSymbol = false ;
    int num = 1, e = 0 ;
    int mem_one = -2, mem_two = -1 ;

    out->sToken = "(" ;
    out->type = LEFTPAREN ;
    tBaseTree = root ;
    for ( tBaseTree = tBaseTree->right ; tBaseTree != NULL ; tBaseTree = tBaseTree->right ) {
      num++ ;
      if ( tBaseTree->type == LEFTPAREN ) {
        if ( tBaseTree->left->type == SYMBOL ) {
          if ( RunType == "cons" ) {
            if ( num == 2 )
              out->left = Search_Symbol( tBaseTree->left ) ;
            else if ( num == 3 )
              out->right = Search_Symbol( tBaseTree->left ) ;
          } // if()
          else if ( RunType == "eqv?" ) {
            if ( num == 2 ) {
              out->left = Search_Symbol( tBaseTree->left ) ;
              e = Take_Binding_Symbol_Num( tBaseTree->left->sToken ) ;
              if ( e != -1 ) {
                mem_one = Take_Binding_Memory_Num( e ) ;
                isUserSymbol = true ;
              } // if()
            } // if()
            else if ( num == 3 ) {
              out->right = Search_Symbol( tBaseTree->left ) ;
              e = Take_Binding_Symbol_Num( tBaseTree->left->sToken ) ;
              if ( e != -1 ) {
                mem_two = Take_Binding_Memory_Num( e ) ;
                isUserSymbol = true ;
              } // if()
            } // else if()
          } // else if()
          else {
            if ( num == 2 ) {
              out->left = Search_Symbol( tBaseTree->left ) ;
              e = Take_Binding_Symbol_Num( tBaseTree->left->sToken ) ;
              if ( e != -1 )
                mem_one = Take_Binding_Memory_Num( e ) ;
            } // if()
            else if ( num == 3 ) {
              out->right = Search_Symbol( tBaseTree->left ) ;
              e = Take_Binding_Symbol_Num( tBaseTree->left->sToken ) ;
              if ( e != -1 )
                mem_two = Take_Binding_Memory_Num( e ) ;
            } // else if()
          } // else
        } // if()
        else if ( tBaseTree->left->type == LEFTPAREN ) {
          Error_Time_Reading( tBaseTree->left->left->sToken ) ;
          if ( num == 2 )
            out->left = Turn_Into_Project_Two( tBaseTree->left ) ;
          else if ( num == 3 )
            out->right = Turn_Into_Project_Two( tBaseTree->left ) ;
        } // else if()
        else {
          if ( num == 2 )
            out->left = tBaseTree->left ;
          else if ( num == 3 )
            out->right = tBaseTree->left ;
        } // else
      } // if()
    } // for


    if ( RunType != "cons" ) {
      if ( RunType == "equal?" ) {
        try {
          Run_Equal( out->right, out->left ) ;
          out->left = NULL ;
          out->right = NULL ;
          out->sToken = "#t" ;
          out->type = T ;
        } // try()
        catch ( bool isNot ) {
          out->left = NULL ;
          out->right = NULL ;
          out->sToken = "nil" ;
          out->type = NIL ;
        } // catch
      } // if()
      else {
        if ( isUserSymbol ) {
          if ( mem_one == mem_two ) {
            out->left = NULL ;
            out->right = NULL ;
            out->sToken = "#t" ;
            out->type = T ;
          } // if()
          else {
            out->left = NULL ;
            out->right = NULL ;
            out->sToken = "nil" ;
            out->type = NIL ;
          } // else
        } // if()
        else if ( out->right->type == out->left->type ) {
          if ( out->right->type == STRING || out->right->type == LEFTPAREN ) {
            out->left = NULL ;
            out->right = NULL ;
            out->sToken = "nil" ;
            out->type = NIL ;
          } // if()
          else {
            if ( out->right->sToken == out->left->sToken ) {
              out->left = NULL ;
              out->right = NULL ;
              out->sToken = "#t" ;
              out->type = T ;
            } // if()
            else {
              out->left = NULL ;
              out->right = NULL ;
              out->sToken = "nil" ;
              out->type = NIL ;
            } // else
          } // else
        } // else if()
        else {
          out->left = NULL ;
          out->right = NULL ;
          out->sToken = "nil" ;
          out->type = NIL ;
        } // else
      } // else
    } // if()

    return out ;

    /*

    */
  } // Run_Cons_Eqv_Equal()

  TokenPtr Run_List_Arith( TokenPtr root, string RunType, string FunctionName ) {
    int total = Check_Number_Error( root, RunType, FunctionName ) ;
    TokenPtr base1 = new Tokendef ;
    TokenPtr out = base1 ;
    TokenPtr base2 = new Tokendef ;
    base1->sToken = "(" ;
    base1->type = LEFTPAREN ;
    base1->left = NULL ;
    base1->right = NULL ;
    int num = 1 ;
    vector < TokenPtr > vAriTree ;
    TokenPtr tBaseTree = root ;
    for ( tBaseTree = tBaseTree->right ; tBaseTree != NULL ; tBaseTree = tBaseTree->right ) {
      num++ ;
      if ( tBaseTree->sToken == "(" ) {
        if ( tBaseTree->left->type == SYMBOL ) {
          base1->left = Search_Symbol( tBaseTree->left ) ;
          if ( RunType != "list" ) {
            if ( SymbolBinding( base1->left->sToken ) ) {
              if ( ArithDetection( RunType ) == 1 ||
                   ArithDetection( RunType ) == 2 ) {
                CatchError( base1->left, RunType ) ;
              } // if()
              else if ( ArithDetection( RunType ) == 3 ||
                        ArithDetection( RunType ) == 4 ) {
                vAriTree.push_back( base1->left ) ;
              } // else if()
            } // if()
            else {
              TokenType tType = base1->left->type ;
              vAriTree = ArithAssistant( RunType, tType, base1->left, vAriTree ) ;
            } // else
          } // if()
        } // if()
        else if ( tBaseTree->left->type == LEFTPAREN ) {
          // cout  << "does it come?\n" ;
          Error_Time_Reading( tBaseTree->left->left->sToken ) ;
          base1->left = Turn_Into_Project_Two( tBaseTree->left ) ;
          TokenType tType = base1->left->type ;
          if ( RunType != "list" )
            vAriTree = ArithAssistant( RunType, tType, base1->left, vAriTree ) ;
        } // else if()
        else {
          base1->left = tBaseTree->left ;
          TokenType tType = base1->left->type ;
          if ( RunType != "list" )
            vAriTree = ArithAssistant( RunType, tType, base1->left, vAriTree ) ;
        } // else

        if ( RunType == "list" ) {
          if ( total != num ) {
            base2 = new Tokendef ;
            base2->sToken = "(" ;
            base2->type = LEFTPAREN ;
            base2->left = NULL ;
            base2->right = NULL ;
            base1->right = base2 ;
            base1 = base1->right ;
          } // if()
        } // if()
      } // if()
      else {
        if ( RunType == "list" ) {
          base2 = new Tokendef ;
          base2->sToken = tBaseTree->sToken ;
          base2->type = tBaseTree->type ;
          base2->left = NULL ;
          base2->right = NULL ;
          base1->right = base2 ;
        } // if()
      } // else
    } // for

    // cout << "does it get rid of for-while? \n" ;

    if ( RunType != "list" ) {
      bool isFloat = false ;
      float fNum = 0.0 ;
      int iNum = 0 ;

      out = new Tokendef ;
      out->left = NULL ;
      out->right = NULL ;
      out->sToken = "#t" ;
      out->type = T ;

      if ( RunType == "+" || RunType == "-" || RunType == "*" || RunType == "/" ) {
        Str100 temp ;

        if ( vAriTree[0]->type == FLOAT ) {
          fNum = atof( vAriTree[0]->sToken.c_str() ) ;
          isFloat = true ;
        } // if()
        else if ( vAriTree[0]->type == INT )
          iNum = atoi( vAriTree[0]->sToken.c_str() ) ;

        for ( int i = 1 ; i < vAriTree.size() ; i++ ) {

          if ( vAriTree[i]->type == FLOAT ) {
            float fTemp = atof( vAriTree[i]->sToken.c_str() ) ;
            if ( fTemp == 0.0 ) {
              mForEvalError.mType = DIVISION_BY_ZERO_ERROR ;
              throw mForEvalError ;
            } // if()
          } // if()
          else if ( vAriTree[i]->type == INT ) {
            int iTemp = atoi( vAriTree[i]->sToken.c_str() ) ;
            if ( iTemp == 0 ) {
              mForEvalError.mType = DIVISION_BY_ZERO_ERROR ;
              throw mForEvalError ;
            } // if()
          } // else if()


          if ( vAriTree[i]->type == FLOAT || isFloat ) {
            isFloat = true ;

            if ( iNum != 0 ) {
              fNum = ( float ) iNum ;
              iNum = 0 ;
            } // if()

            if ( RunType == "+" )
              fNum += atof( vAriTree[i]->sToken.c_str() ) ;
            else if ( RunType == "-" )
              fNum -= atof( vAriTree[i]->sToken.c_str() ) ;
            else if ( RunType == "*" )
              fNum *= atof( vAriTree[i]->sToken.c_str() ) ;
            else if ( RunType == "/" )
              fNum /= atof( vAriTree[i]->sToken.c_str() ) ;
          } // if()
          else if ( vAriTree[i]->type == INT && !isFloat ) {
            if ( RunType == "+" )
              iNum += atoi( vAriTree[i]->sToken.c_str() ) ;
            else if ( RunType == "-" )
              iNum -= atoi( vAriTree[i]->sToken.c_str() ) ;
            else if ( RunType == "*" )
              iNum *= atoi( vAriTree[i]->sToken.c_str() ) ;
            else if ( RunType == "/" )
              iNum /= atoi( vAriTree[i]->sToken.c_str() ) ;
          } // else if()
        } // for()

        if ( !isFloat ) {
          sprintf( temp, "%d", iNum ) ;
          string put( temp ) ;
          out->sToken = put ;
          out->type = INT ;
        } // if()
        else  {
          sprintf( temp, "%.3f", fNum ) ;
          string put( temp ) ;
          out->sToken = put ;
          out->type = FLOAT ;
        } // else
      } // if()
      else if ( RunType == "begin" )
        return vAriTree[vAriTree.size()-1] ;
      else if ( RunType == "not" ) {
        if ( vAriTree[0]->type != NIL ) {
          out->sToken = "nil" ;
          out->type = NIL ;
        } // if()
      } // else if()
      else if ( RunType  == "and" || RunType == "or" ) {
        bool isOr = false, isError = false ;
        for ( int i = 0 ; i < vAriTree.size() && !isError ; i++ ) {
          if ( vAriTree[i]->type == NIL ) {
            if ( RunType == "and" ) {
              out->sToken = "nil" ;
              out->type = NIL ;
              isError = true ;
            } // if()
          } // if()
          else if ( vAriTree[i]->type != NIL ) {
            if ( RunType == "or" ) {
              isOr = true ;
              isError = true ;
            } // if()
          } // else if()
        } // for()

        if ( RunType == "or" ) {
          if ( !isOr ) {
            out->sToken = "nil" ;
            out->type = NIL ;
          } // if()
        } // if()
      } // else if()
      else if ( RunType == "string-append" || RunType == "string>?" ||
                RunType == "string<?" || RunType == "string=?" ) {
        string sGet = "", use_one = "" ;
        int iLength = vAriTree[0]->sToken.length(), iTemp = 0 ;
        bool isAnError = false ;
        if ( RunType == "string>?" ||
             RunType == "string<?" ||
             RunType == "string=?" ) {
          use_one = vAriTree[0]->sToken.substr( 1, iLength-2 ) ;
          iTemp = 1 ;
        } // if()

        for ( ; iTemp < vAriTree.size() && !isAnError ; iTemp++ ) {
          iLength = vAriTree[iTemp]->sToken.length() ;
          if ( RunType == "string>?" ) {
            sGet = vAriTree[iTemp]->sToken.substr( 1, iLength-2 ) ;
            if ( use_one <= sGet )
              isAnError = true ;
            use_one = sGet ;
          } // if()
          else if ( RunType == "string<?" ) {
            sGet = vAriTree[iTemp]->sToken.substr( 1, iLength-2 ) ;
            if ( use_one >= sGet )
              isAnError = true ;
            use_one = sGet ;
          } // else if()
          else if ( RunType == "string=?" ) {
            sGet = vAriTree[iTemp]->sToken.substr( 1, iLength-2 ) ;
            if ( use_one != sGet )
              isAnError = true ;
            use_one = sGet ;
          } // else if()
          else
            sGet += vAriTree[iTemp]->sToken.substr( 1, iLength-2 ) ;
        } // for()

        if ( RunType == "string-append" ) {
          out->sToken = "\"" + sGet + "\"" ;
          out->type = STRING ;
        } // if()
        else {
          if ( isAnError ) {
            out->sToken = "nil" ;
            out->type = NIL ;
          } // if()
        } // else
      } // else if()
      else if ( RunType == ">" || RunType == "<" || RunType == "=" ||
                RunType == ">=" || RunType == "<=" ) {
        bool isAnError = false ;
        fNum = atof( vAriTree[0]->sToken.c_str() ) ;
        for ( int i = 1 ; i < vAriTree.size() && !isAnError ; i++ ) {
          if ( RunType == "<" ) {
            if ( fNum >= atof( vAriTree[i]->sToken.c_str() ) )
              isAnError = true ;
          } // if()
          else if ( RunType == ">" ) {
            if ( fNum <= atof( vAriTree[i]->sToken.c_str() ) )
              isAnError = true ;
          } // else if()
          else if ( RunType == "=" ) {
            if ( fNum != atof( vAriTree[i]->sToken.c_str() ) )
              isAnError = true ;
          } // else if()
          else if ( RunType == "<=" ) {
            if ( fNum > atof( vAriTree[i]->sToken.c_str() ) )
              isAnError = true ;
          } // else if()
          else if ( RunType == ">=" ) {
            if ( fNum < atof( vAriTree[i]->sToken.c_str() ) )
              isAnError = true ;
          } // else if()

          fNum = atof( vAriTree[i]->sToken.c_str() ) ;
        } // for()

        if ( isAnError ) {
          out->sToken = "nil" ;
          out->type = NIL ;
        } // if()
      } // else if()
    } // if()

    return out ;


  } // Run_List_Arith()

  TokenPtr Run_Quote( TokenPtr root, string FunctionName ) {
    TokenPtr tBaseTree = new Tokendef ;
    int temp = Check_Number_Error( root, "quote", FunctionName ) ;

    root = root->right ;
    tBaseTree = root->left ;
    if ( tBaseTree->sToken == "(" )
      MarkTree( tBaseTree );
    else
      tBaseTree->mark = 1 ;

    return tBaseTree ;

  } // Run_Quote()

  TokenPtr Run_Car_Cdr( TokenPtr root, string FunctionName ) {
    string sBaseName = root->left->sToken ;
    Check_Number_Error( root, sBaseName, FunctionName ) ;
    TokenPtr tBaseTree = root->right, tSaveTree = new Tokendef ;
    for ( int num = 1 ; tBaseTree != NULL ; tBaseTree = tBaseTree->right ) {
      num++ ;
      if ( tBaseTree->sToken == "(" ) {
        if ( tBaseTree->left->type == SYMBOL )
          tSaveTree = Search_Symbol( tBaseTree->left ) ;
        else if ( tBaseTree->left->type == LEFTPAREN ) {
          Error_Time_Reading( tBaseTree->left->left->sToken ) ;
          tSaveTree = Turn_Into_Project_Two( tBaseTree->left ) ;
        } // else if()
        else {
          mForEvalError.mType = INVALID_ARG_NUMBER_ERROR ;
          mForEvalError.mErrorTree = tBaseTree->left ;
          mForEvalError.mErrorToken = sBaseName ;
          throw mForEvalError ;
        } // else
      } // if()
    } // for


    if ( tSaveTree->sToken == "(" ) {
      if ( sBaseName == "car" )
        tSaveTree = tSaveTree->left ;
      else if ( sBaseName == "cdr" ) {
        if ( tSaveTree->right->sToken != ")" )
          tSaveTree = tSaveTree->right ;
        else {
          TokenPtr tTemp = new Tokendef ;
          tTemp->sToken = "nil" ;
          tTemp->type = NIL ;
          tTemp->left = NULL ;
          tTemp->right = NULL ;
          tSaveTree = tTemp ;
        } // else
      } // else if()
    } // if()
    else {
      mForEvalError.mType = INVALID_ARG_NUMBER_ERROR ;
      mForEvalError.mErrorTree = tSaveTree ;
      mForEvalError.mErrorToken = sBaseName ;
      throw mForEvalError ;
    } // else

    return tSaveTree ;
  } // Run_Car_Cdr()

  TokenPtr Run_Primitive( TokenPtr root, string RunType, string FunctionName ) {
    // cout << "Get in Run_Primitive!!!\n" ;
    string sBaseName = root->left->sToken ;
    TokenPtr sBaseTree = root ;
    Check_Number_Error( root, sBaseName, FunctionName ) ;
    TokenType tType ;
    int num = 1 ;
    TokenPtr tOut = new Tokendef ;
    tOut->left = NULL ;
    tOut->right = NULL ;
    tOut->sToken = "#t" ;
    tOut->type = T ;
    tOut->mark = 0 ;

    for ( sBaseTree = sBaseTree->right ; sBaseTree != NULL ; sBaseTree = sBaseTree->right ) {
      num++ ;
      if ( sBaseTree->sToken == "(" ) {
        if ( sBaseTree->left->type == SYMBOL ) {
          sBaseTree->left = Search_Symbol( sBaseTree->left ) ;
          tType = sBaseTree->left->type ;
        } // if()
        else if ( sBaseTree->left->type == LEFTPAREN ) {
          Error_Time_Reading( sBaseTree->left->left->sToken ) ;
          tType = Turn_Into_Project_Two( sBaseTree->left )->type ;
        } // else if()
        else
          tType = sBaseTree->left->type ;
      } // if()
    } // for()

    if ( RunType == "symbol?" && tType == SYMBOL )
      return tOut ;
    else if ( RunType == "boolean?" && tType == T )
      return tOut ;
    else if ( RunType == "boolean?" && tType == NIL )
      return tOut ;
    else if ( RunType == "string?" && tType == STRING )
      return tOut ;
    else if ( RunType == "number?" && tType == INT )
      return tOut ;
    else if ( RunType == "number?" && tType == FLOAT )
      return tOut ;
    else if ( RunType == "real?" && tType == INT )
      return tOut ;
    else if ( RunType == "real?" && tType == FLOAT )
      return tOut ;
    else if ( RunType == "integer?" && tType == INT )
      return tOut ;
    else if ( RunType == "null?" && tType == NIL )
      return tOut ;
    else if ( RunType == "pair?" && tType == LEFTPAREN )
      return tOut ;
    else {
      tOut->sToken = "nil" ;
      tOut->type = NIL ;
    } // else

    return tOut ;
  } // Run_Primitive()

  TokenPtr Run_If( TokenPtr root, string RunType, string FunctionName ) {
    int total = Check_Number_Error( root, RunType, FunctionName ) ;
    TokenPtr tBaseTree = root ;
    int num = 1 ;
    bool isTrue = false ;

    for ( tBaseTree = tBaseTree->right ; tBaseTree != NULL ; tBaseTree = tBaseTree->right ) {
      num++ ;
      if ( tBaseTree->sToken == "(" ) {
        if ( tBaseTree->left->type == SYMBOL ) {
          tBaseTree->left = Search_Symbol( tBaseTree->left ) ;

          if ( num == 2 ) {
            if ( tBaseTree->left->type != NIL || SymbolBinding( tBaseTree->left->sToken ) )
              isTrue = true ;
          } // if()
          else {
            return tBaseTree->left ;
          } // else
        } // if()
        else if ( tBaseTree->left->type == LEFTPAREN ) {
          Error_Time_Reading( tBaseTree->left->left->sToken ) ;
          tBaseTree->left = Turn_Into_Project_Two( tBaseTree->left ) ;
          if ( num == 2 ) {
            if ( tBaseTree->left->type != NIL )
              isTrue = true ;
          } // if()
          else {
            return tBaseTree->left ;
          } // else
        } // else if()
        else {
          if (  num == 2 ) {
            if ( tBaseTree->left->type != NIL )
              isTrue = true ;
          } // if()
          else {
            return tBaseTree->left ;
          } // else
        } // else
      } // if()
      else if ( tBaseTree->sToken == ")" ) {
        if ( num == 3 ) {
          mForEvalError.mErrorTree = root ;
          mForEvalError.mType = NO_RETURN_VALUE_ERROR ;
          throw mForEvalError ;
        } // if()
      } // else if()

      if ( !isTrue )
        tBaseTree = tBaseTree->right ;
    } // for()

    return tBaseTree ;
  } // Run_If()

  TokenPtr Run_Cond( TokenPtr root, string RunType, string FunctionName ) {
    int total = Check_Number_Error( root, RunType, FunctionName ) ;
    int cond_level = Cond_Layers( root ) ;
    int num = 1 ;
    TokenPtr tBaseTree = new Tokendef ;

    bool isCorrectElse = false ;
    tBaseTree = root->right ;

    for ( ; tBaseTree != NULL ; tBaseTree = tBaseTree->right ) {
      num++ ;
      if ( tBaseTree->left->sToken == "(" ) {
        // if ( i )
        ;
      } // if()
    } // for()


  } // Run_Cond()



  void Check_Whether_Is_An_End_Tree( TokenPtr MyToken, Treedef TheEndTree ) {
    bool check = false ;
    if ( MyToken->type == LEFTPAREN && MyToken->left->sToken == "exit"
         && MyToken->right->type == RIGHTPAREN ) {
      TheEndTree.mTypeOfTree = EXIT ;
      check = true ;
    } // if()
    else if ( MyToken->type == LEFTPAREN && MyToken->left->sToken == "exit"
              && MyToken->right->type == NIL ) {
      TheEndTree.mTypeOfTree = EXIT ;
      check = true ;
    } // else if()

    if ( check == true ) throw TheEndTree ;

  } // Check_Whether_Is_An_End_Tree()

  void Error_Time_Reading( string FunctionName ) {
    // deal with the situation like ( (define a b ) f )
    if ( FunctionName == "exit" || FunctionName == "define" || FunctionName == "clean-environment" ) {
      mForEvalError.mType = FORMAT_ERROR ;
      mForEvalError.mErrorToken = FunctionName ;
      throw mForEvalError ;
    } // if()
  } // Error_Time_Reading()

  void Copy_Tree( TokenPtr CopyTree, TokenPtr root, int isLeft ) {
    if ( root != NULL ) {
      TokenPtr add = new Tokendef ;
      add->left = NULL ;
      add->right = NULL ;
      add->mark = root->mark ;
      add->sToken = root->sToken ;
      add->type = root->type ;
      if ( isLeft == 1 ) {
        CopyTree->left = add ;
        CopyTree = CopyTree->left ;
      } // if
      else if ( isLeft == 2 ) {
        CopyTree->right = add ;
        CopyTree = CopyTree->right ;
      } // else if()

      Copy_Tree( CopyTree, root->left, 1 ) ;
      Copy_Tree( CopyTree, root->right, 2 ) ;
    } // if()
  } // Copy_Tree()

  void Clean_already_define_function( string FunctionName ) {
    int i = -1 ;
    for ( int j = 0 ; j < mForUserFunction.size() ; j++ ) {
      if ( mForUserFunction[j].mName == FunctionName )
        i = j ;
    } // for()

    if ( i != -1 )
      mForUserFunction.erase( mForUserFunction.begin() + i ) ;
  } // Clean_already_define_function()

  void Run_Define( TokenPtr root ) {
    int temp = Check_Number_Error( root, "define", "define" ) ;
    TokenPtr use = NULL ;
    TokenPtr tBaseTree = root ;
    Binding_Symbol add ;
    Binding_Function add_function ;
    string out = "" ;
    int num = 1, vector_num = -1, memory = 0 ;
    bool bAlready_Bound = false, bFind_Define_Function = false ;
    tBaseTree = tBaseTree->right ;

    // for ( num = 1 ; tBaseTree != NULL ; tBaseTree = tBaseTree->right ) {
      // cout << num << "    " << tBaseTree->sToken << endl ;
      // num++ ;
    // } // for

    tBaseTree = root->right ;
    root = root->right ;
    TokenPtr tUsingTree = new Tokendef ;
    for ( num = 1 ; tBaseTree != NULL ; tBaseTree = tBaseTree->right, root = root->right ) {
      tUsingTree = tBaseTree ;
      num++ ;
      if ( tUsingTree->type == LEFTPAREN ) {
        if ( tUsingTree->left->type == SYMBOL ) {
          int e = Take_Binding_Symbol_Num( tUsingTree->left->sToken ) ;
          if ( num == 2 ) {
            if ( e == -1 ) {
              if ( SymbolBinding( tUsingTree->left->sToken ) ) {
                mForEvalError.mType = FORMAT_ERROR ;
                mForEvalError.mErrorToken = "define" ;
                throw mForEvalError ;
              } // if()
              else
                add.mForBindingThing = tUsingTree->left ;
            } // if()
            else
              vector_num = e ;
          } // if()
          else if ( num == 3 ) {
            if ( bFind_Define_Function )
              add_function.mForFunctionNode = tUsingTree->left ;
            else if ( e != -1 ) {
              memory = mForUserBinding[e].mLocation ;
              root->left = Take_The_Bound_Node( e ) ;
              if ( vector_num == -1 )
                add.mForBindingNode = tUsingTree->left ;
              else {
                use = tUsingTree->left ;
                bAlready_Bound = true ;
              } // else
            } // else if()
            else {
              if ( vector_num == -1 )
                add.mForBindingNode = tUsingTree->left ;
              else {
                use = tUsingTree->left ;
                bAlready_Bound = true ;
              } // else
            } // else
          } // else if()
        } // if()
        else if ( tUsingTree->left->type == LEFTPAREN ) {
          Error_Time_Reading( tUsingTree->left->left->sToken ) ;
          if ( num == 2 ) {
            if ( !SymbolBinding( tUsingTree->left->left->sToken ) ) {
              int save_num  = Take_Binding_Symbol_Num( tUsingTree->left->left->sToken ) ;
              if ( save_num == -1 )
                add.mForBindingThing = tUsingTree->left->left ;
              else
                vector_num = save_num ;

              add_function.mName = tUsingTree->left->left->sToken ;
              add_function.mForFunctionThing = tUsingTree->left->right ;
              bFind_Define_Function = true ;
            } // if()
            else {
              mForEvalError.mType = FORMAT_ERROR ;
              mForEvalError.mErrorToken = "define" ;
              throw mForEvalError ;
            } // else
          } // if()
          else if ( num == 3 ) {
            if ( !bFind_Define_Function ) {
              TokenPtr tWork = Turn_Into_Project_Two( tUsingTree->left ) ;
              if ( vector_num == -1 ) {
                if ( tWork->sToken != "#<user function>" )
                  add.mForBindingNode = tWork ;
                else {
                  add_function.mName = add.mForBindingThing->sToken ;
                  add_function.mForFunctionThing = mLambdaFunction.mForFunctionThing ;
                  add_function.mForFunctionNode = mLambdaFunction.mForFunctionNode ;
                  bFind_Define_Function = true ;
                } // else
              } // if()
              else {
                bAlready_Bound = true ;
                if ( tWork->sToken != "#<user function>" )
                  use = tWork ;
                else {
                  add_function.mName = mForUserBinding[vector_num].mForBindingThing->sToken ;
                  add_function.mForFunctionThing = mLambdaFunction.mForFunctionThing ;
                  add_function.mForFunctionNode = mLambdaFunction.mForFunctionNode ;
                  bFind_Define_Function = true ;
                } // else
              } // else
            } // if()
            else
              add_function.mForFunctionNode = tUsingTree->left ;
          } // else if()
        } // else if()
        else {
          if ( num == 2 ) {
            mForEvalError.mType = FORMAT_ERROR ;
            mForEvalError.mErrorToken = "define" ;
            throw mForEvalError ;
          } // if()
          else if ( num == 3 ) {
            if ( !bFind_Define_Function ) {
              if ( vector_num == -1 )
                add.mForBindingNode = tUsingTree->left ;
              else {
                bAlready_Bound = true ;
                use = tUsingTree->left ;
              } // else
            } // if()
            else
              add_function.mForFunctionNode = tUsingTree->left ;
          } // else if()
        } // else

        if ( num == 3 ) {
          if ( bFind_Define_Function ) {
            if ( vector_num == -1 ) {
              add.mForBindingNode = add.mForBindingThing ;
              add.mForBindingNode->mark = 2 ;
            } // if()
            else {
              bAlready_Bound = true ;
              use = mForUserBinding[vector_num].mForBindingThing ;
              use->mark = 2 ;
            } // else

            Clean_already_define_function( add_function.mName ) ;
            mForUserFunction.push_back( add_function ) ;
          } // if()

          if ( vector_num == -1 )
            out = add.mForBindingThing->sToken ;
          else
            out = mForUserBinding[vector_num].mForBindingThing->sToken ;
        } // if()
      } // if()

    } // for()

    if ( bAlready_Bound ) {
      if ( memory != 0 )
        mForUserBinding[vector_num].mLocation = memory ;
      else
        mForUserBinding[vector_num].mLocation = memory + 1 ;
      mForUserBinding[vector_num].mForBindingNode = use ;
      // throw out ;
    } // if
    else {
      if ( memory == 0 )
        add.mLocation = mForUserBinding.size() + 1 ;
      else
        add.mLocation = memory ;
      mForUserBinding.push_back( add ) ;
      // throw out ;
    } // else

    // cout << mForUserBinding[0].mForBindingNode->sToken << endl ;
    // cout << mForUserBinding[0].mForBindingNode->type << endl ;
    // cout << mForUserBinding[0].mForBindingThing->sToken << endl ;
    throw out ;
  } // Run_Define()

  void Run_Clean( TokenPtr root ) {
    Check_Number_Error( root, "clean-environment", "clean-environment" ) ;
    mForUserBinding.clear() ;
    string out = "clean-environment" ;
    throw out ;
  } // Run_Clean()

  void Run_Equal( TokenPtr mRight, TokenPtr mLeft ) {
    if ( mRight != NULL && mLeft != NULL ) {
      if ( mRight->sToken != mLeft->sToken )
        throw false ;
      Run_Equal( mRight->left, mLeft->left ) ;
      Run_Equal( mRight->right, mLeft->right ) ;
    } // if()
    else if ( mRight == NULL && mLeft != NULL )
      throw false ;
    else if ( mRight != NULL && mLeft == NULL )
      throw false ;
  } // Run_Equal()

  void CatchError( TokenPtr root, string FunctionName ) {
    mForEvalError.mType = INVALID_ARG_NUMBER_ERROR ;
    mForEvalError.mErrorToken = FunctionName ;
    mForEvalError.mErrorTree = root ;
    throw mForEvalError ;
  } // CatchError()

  void MarkTree( TokenPtr root ) {
    if ( root != NULL ) {
      root->mark = 1 ;
      MarkTree( root->left ) ;
      MarkTree( root->right ) ;
    } // if()
  } // MarkTree()

};


int main()
{
  Trees myTree ;
  bool isDone = false ;
  cin >> gTestNum ;
  cout <<  "Welcome to OurScheme!\n\n> " ;
  while ( !isDone ) {
    try {
      myTree.GetTree() ;
    } // try
    catch ( SyntaxError isError ) {

      if ( isError.mLine == 0 )
        isError.mLine = 1 ;

      if ( isError.mType == EXIT_ERROR ) {
        isDone = true ;
        cout << "\nThanks for using OurScheme!\n" ;
        return 0 ;
      } // if
      else if ( isError.mType == EOF_ERROR ) {
        isDone = true ;
        cout << "ERROR (no more input) : END-OF-FILE encountered" ;
        cout << "\nThanks for using OurScheme!\n" ;
        return 0 ;
      } // else if
      else if ( isError.mType == STRING_ERROR ) {
        cout << "ERROR (no closing quote) : " ;
        cout << "END-OF-LINE encountered at Line " ;
        cout << isError.mLine << " Column " << isError.mColumn+1  ;
      } // else if()
      else if ( isError.mType == LINE_ENTER_ERROR ) {
        cout << "ERROR (unexpected token) : " ;
        cout << "')' expected when token at Line " ;
        cout << isError.mLine << " Column " << isError.mColumn ;
        cout << " is >>" << isError.mChar << "<<"  ;
      } // else if()
      else if ( isError.mType == UNEXPECTED_ERROR ) {
        cout << "ERROR (unexpected token) : " ;
        cout << "atom or '(' expected when token at Line " ;

        cout << isError.mLine << " Column " << isError.mColumn ;
        cout << " is >>" << isError.mChar << "<<" ;
      } // else if()
      else if ( isError.mType == UNEXPECTED_ATOM_FOR_DOT_IN_TREE_ERROR ) {
        cout << "ERROR (unexpected token) : " ;
        cout << "atom or '(' expected when token at Line " ;
        cout << isError.mLine << " Column " << isError.mColumn ;
        cout << " is >>" << isError.mChar << "<<" ;
      } // else if()

      if ( isError.mType == UNEXPECTED_ERROR ||
           isError.mType == UNEXPECTED_ATOM_FOR_DOT_IN_TREE_ERROR ||
           isError.mType == LINE_ENTER_ERROR ) {
        try {
          myTree.mForTree.GetSurplus() ;
        } // try
        catch ( SyntaxError isError ) {
          isDone = true ;
          cout << "\n\n> ERROR (no more input) : END-OF-FILE encountered" ;
          cout << "\nThanks for using OurScheme!\n" ;
          return 0 ;
        } // catch()
      } // if()


    } // catch
    catch ( EvalError isErrors ) {  // ------ Project Two ------ //
      if ( isErrors.mType == UNBOUND_SYMBOL_ERROR ) {
        cout << "ERROR (unbound symbol) : " << isErrors.mErrorToken ;
      } // if()
      else if ( isErrors.mType == NON_LIST_ERROR ) {
        cout << "ERROR (non-list) : " ;
        myTree.PrintTree( isErrors.mErrorTree, true, 1 );
      } // else if()
      else if ( isErrors.mType == APPLY_NON_FUNCTION_ERROR ) {
        cout << "ERROR (attempt to apply non-function) : " ;
        cout << isErrors.mErrorToken ;
      } // else if()
      else if ( isErrors.mType == LEVEL_CLEAN_ENVIRONMENT_ERROR ) {
        cout << "ERROR (level of CLEAN_ENVIRONMENT)" ;
      } // else if()
      else if ( isErrors.mType == LEVEL_DEFINE_ERROR ) {
        cout << "ERROR (level of DEFINE)" ;
      } // else if()
      else if ( isErrors.mType == LEVEL_EXIT_ERROR ) {
        cout << "ERROR (level of EXIT)" ;
      } // else if()
      else if ( isErrors.mType == FORMAT_ERROR ) {
        cout << "ERROR (" << isErrors.mErrorToken << " format)" ;
      } // else if()
      else if ( isErrors.mType == INVALID_ARG_NUMBER_ERROR ) {
        cout << "ERROR (incorrect number of arguments) : " ;
        cout << isErrors.mErrorToken ;
      } // else if()
      else if ( isErrors.mType == NO_RETURN_VALUE_ERROR ) {
        cout << "ERROR (no return value) : " ;
        myTree.PrintPureTree( isErrors.mErrorTree, true, 1 ) ;
      } // else if()
      else if ( isErrors.mType == DIVISION_BY_ZERO_ERROR ) {
        cout << "ERROR (division by zero) : /" ;
      } // else if()
      else if ( isErrors.mType == COND_NUM_ERROR ) {
        cout << "ERROR" ;
      } // else if()


    } // catch
    catch ( TokenPtr MyToken ) {
      if ( MyToken->type != LEFTPAREN ) {
        cout << MyToken->sToken ;
      } // if()
      else if ( MyToken->type == LEFTPAREN && MyToken->left->sToken == "exit"
                && MyToken->right->type == RIGHTPAREN ) {
        cout << "\nThanks for using OurScheme!\n" ;
        return 0 ;
      } // else if()
      else if ( MyToken->type == LEFTPAREN && MyToken->left->sToken == "exit"
                && MyToken->right->type == NIL ) {
        cout << "\nThanks for using OurScheme!\n" ;
        return 0 ;
      } // else if()
      else {

        myTree.PrintTree( MyToken, true, 1 );
      } // else
    } // catch
    catch ( Treedef MyCompleteTree ) { // ------ Project Two ------ //
      if ( MyCompleteTree.mTypeOfTree == ATOM_ONE ) {
        cout << MyCompleteTree.mTree->sToken ;
      } // if()
      else if ( MyCompleteTree.mTypeOfTree == ATOM_TWO ||
                MyCompleteTree.mTypeOfTree == LIST ||
                MyCompleteTree.mTypeOfTree == DOTTED_PAIR ) {
        myTree.PrintTree( MyCompleteTree.mTree, true, 1 );
      } // else if()
      else if ( MyCompleteTree.mTypeOfTree == EXIT ) {
        cout << "\nThanks for using OurScheme!\n" ;
        return 0 ;
      } // else if()
    } // catch
    catch ( string Operations ) {
      if ( Operations == "clean-environment" )
        cout << "environment cleaned" ;
      else
        cout << Operations << " defined" ;
    } // catch

    cout << "\n\n> " ;
  } // while




  return 0;
} // main()
