# include <stdlib.h>
# include <stdio.h>
# include <cstring>
# include <ctype.h>
# include <iostream>
# include <string.h>
# include <string>

using namespace std;

typedef char Str100[ 100 ] ;
int gTestNum ;

enum ErrorType{
  EXIT_ERROR = 1,  EOF_ERROR = 2,
  STRING_ERROR = 3, UNEXPECTED_ERROR = 4,
  LINE_ENTER_ERROR = 5, UNEXPECTED_ATOM_FOR_DOT_IN_TREE_ERROR = 6
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
  string sToken ;
  TokenType type ;
  Tokendef * left ;
  Tokendef * right ;
} ;

typedef Tokendef* TokenPtr ;

class Token {
  public :
  char mForRead ;
  SyntaxError mForError ;
  TokenType mForJudge ;

  void GetOneChar() {
    char lastChar = mForRead ;
    mForRead = cin.get() ;

    if ( cin.eof() ) {
      mForError.mType = EOF_ERROR ;
      throw mForError ;
    } // if

    if ( lastChar == '\n' ) {
      mForError.mLine++ ;
      mForError.mColumn = 1 ;
    } // if()
    else {
      mForError.mColumn++ ;
    } // else ()

    // cout << mForRead << " " << mForError.mLine << " " << mForError.mColumn << endl ;

  } // GetOneChar()

  void SetOneChar( char c ) {
    mForRead = c ;
  } // SetOneChar()

  void GetUselessToken() { // read out the useless before tokens
    do {
      GetOneChar() ;
       // if ( mForRead == ' ' && mForError.mColumn == 1 && mForError.mLine == 0 ) {
       //  mForError.mColumn = 0 ;
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
          mForError.mType = STRING_ERROR ;
          throw mForError ;
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
    return temp ;
  } // GetToken()

  TokenPtr SetToken( string theToken ) {
    TokenPtr temp = new Tokendef ;
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

class Trees {
  public :
  Token mForTree ;

  void GetTree() {
    TokenPtr root = new Tokendef ;
    mForTree.mForError.mColumn = 0 ;
    mForTree.mForError.mLine = 0 ;
    int mCheckLine = mForTree.mForError.mLine ;
    int mCheckColumn = mForTree.mForError.mColumn ;
    root = mForTree.GetToken() ;

    if ( mForTree.mForError.mLine == mCheckLine ) {
      //cout << "this is test equal !\n" ;
      //cout << mForTree.mForError.mLine << endl ;
      //cout << mCheckLine << endl ;
      mForTree.mForError.mLine = 1 ;
      mForTree.mForError.mColumn -= mCheckColumn ;
    } // if()
   /* else if ( mForTree.mForError.mLine > mCheckLine ) {
      //cout << "this is test bigger !\n" ;
      //cout << mForTree.mForError.mLine << endl ;
      //cout << mCheckLine << endl ;
      mForTree.mForError.mLine -= mCheckLine ;
    } // else if()  */

    if ( root->type == INT || root->type == FLOAT || root->type == STRING ||
         root->type == T || root->type == NIL || root->type == SYMBOL ) {
      throw root ;
    } // if()
    else if ( root->type == LEFTPAREN ) {
      BuildTree( root ) ;
    } // else if()
    else if ( root->type == QUOTE ) {
      root = new Tokendef ;
      root = BuildQTree() ;
    } // else if
    else {
      if ( root->type == RIGHTPAREN ) {
        mForTree.mForError.mType = UNEXPECTED_ATOM_FOR_DOT_IN_TREE_ERROR ;
        mForTree.mForError.mChar = ')' ;
        throw mForTree.mForError ;
      } // if()
      else if ( root->type == DOT ) {
        mForTree.mForError.mType = UNEXPECTED_ERROR ;
        mForTree.mForError.mChar = '.' ;
        throw mForTree.mForError ;
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
            mForTree.mForError.mType = UNEXPECTED_ATOM_FOR_DOT_IN_TREE_ERROR ;
            mForTree.mForError.mChar = ')' ;
            throw mForTree.mForError ;
          } // if()
        } // if()
        else if ( root->left != NULL && root->right != NULL ) {
          mForTree.mForError.mType = LINE_ENTER_ERROR ;
          mForTree.mForError.mChar = '.' ;
          throw mForTree.mForError ;
        } // else if()
        else {
          char q = cin.peek() ;
          if ( q != '\n' )
            mForTree.mForError.mType = UNEXPECTED_ATOM_FOR_DOT_IN_TREE_ERROR ;
          else
            mForTree.mForError.mType = LINE_ENTER_ERROR ;

          mForTree.mForError.mChar = '.' ;
          throw mForTree.mForError ;
        } // else
      } // else if()

      if ( leaf->type == QUOTE ) {
        if ( root->left != NULL && root->right != NULL )  {
          mForTree.mForError.mType = UNEXPECTED_ERROR ;
          mForTree.mForError.mChar = '\'' ;
          throw mForTree.mForError ;
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
        mForTree.mForError.mType = LINE_ENTER_ERROR ;
        mForTree.mForError.mChar = leaf->sToken[0] ;
        throw mForTree.mForError ;
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

    leftTwo->right = rightOne ;       //   (          //
    leftOne->left = quotee ;         //   /  \        //
    leftOne->right = leftTwo ;      // quote  (       //
                                   //        /  \     //
                                  //      tree    )   //

    if ( getOneTree->type != RIGHTPAREN && getOneTree->type != DOT ) {
      if ( getOneTree->type == LEFTPAREN )
        BuildTree( getOneTree ) ;
      else if ( getOneTree->type == QUOTE ) {
        getOneTree = new Tokendef ;
        getOneTree = BuildQTree() ;
      } // else if()

      leftTwo->left = getOneTree ;
      return leftOne ;
    } // if
    else {
      char checkError = cin.peek() ;
      if ( getOneTree->type == RIGHTPAREN ) {
        mForTree.mForError.mType = UNEXPECTED_ERROR ;
        mForTree.mForError.mChar = ')' ;
        throw mForTree.mForError ;
      } // if()

      if ( checkError == '\n' ) {
        mForTree.mForError.mType = LINE_ENTER_ERROR ;
        throw mForTree.mForError ;
      } // if()
      else {
        mForTree.mForError.mType = UNEXPECTED_ERROR ;
        mForTree.mForError.mChar = checkError ;
        throw mForTree.mForError ;
      } // else

      return NULL ;
    } // else

  } // BuildQTree()



  void PrintTree( TokenPtr root, bool isLeft, int TreeHieght ) {
    if ( root == NULL ) {
      return ;
    } // if
    else {
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
              cout << root->sToken  ;
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

      PrintTree( root->left, true, TreeHieght+1 ) ;
      PrintTree( root->right, false, TreeHieght ) ;
    } // else
  } // PrintTree()


  void PrintSpecialTree( TokenPtr root ) {
    cout << root->sToken  ;
  } // PrintSpecialTree()


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

    cout << "\n\n> " ;
  } // while




  return 0;
} // main()
