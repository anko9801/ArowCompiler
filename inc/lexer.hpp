#ifndef LEXER_HPP
#define LEXER_HPP
	
#include<cstdio>
#include<cstdlib>
#include<fstream>
#include<list>
#include<string>
#include<vector>
#include"APP.hpp"


/**
  * トークン種別
  */
enum TokenType{
	TOK_IDENTIFIER,		//識別子
	TOK_DIGIT,			//数字
	TOK_FLOAT,
	TOK_STRING,
	TOK_TRUTH,
	TOK_IMPORT,
	TOK_SYMBOL,			//記号
	TOK_TYPE,			//型
	TOK_IF,
	TOK_WHILE,
	TOK_FOR,
	TOK_MATCH,
	TOK_ASYNC,
	TOK_AWAIT,
	TOK_RETURN,			//RETURN
	TOK_CAST,
	TOK_NL,
	TOK_EOF				//EOF
};


/**
  *個別トークン格納クラス
  */
typedef class Token{
	public:

	private:
	std::string TokenString;
	TokenType Type;
	std::string DataType;
	int Number;
	bool Bool;
	int Line;
	std::string Filename;


	public:
	Token(std::string string, TokenType type, int line, std::string filename) 
		: TokenString(string), Type(type), Line(line), Filename(filename) {
		if (type == TOK_DIGIT || type == TOK_FLOAT) {
			if (string[0] == '0' && string[1] == 'b') {
				string.erase(string.begin(), string.begin() + 2);
				Number = strtol(string.c_str(), NULL, 2);
			}else{
				Number = atof(string.c_str());
			}
		}else{
			Number = 0x7fffffff;
		}
		if (type == TOK_TRUTH)
			Bool = true;
		else
			Bool = false;
	};
	~Token() {};


	TokenType getTokenType() {return Type;};
	std::string getTokenDataType() {return DataType;};
	std::string getTokenString() {return TokenString;};
	int getNumberValue() {return Number;};
	bool getBoolValue() {return Bool;}
	bool setLine(int line) {Line = line;return true;}
	int getLine() {return Line;}
	std::string getFile() {return Filename;}
	
}Token;



/**
  * 切り出したToken格納用クラス
  */
class TokenStream{
	public:

	private:
		std::vector<Token*> Tokens;
		int CurIndex;

	protected:

	public:
		TokenStream():CurIndex(0) {}
		~TokenStream();

		bool ungetToken(int Times=1);
		bool getNextToken();
		bool pushToken(Token *token) {
			Tokens.push_back(token);
			return true;
		}
		Token getToken();
		TokenType getCurType() {return Tokens[CurIndex]->getTokenType();}
		std::string getCurDataType() {return Tokens[CurIndex]->getTokenDataType();}
		std::string getCurString() {return Tokens[CurIndex]->getTokenString();}
		int getCurNumVal() {return Tokens[CurIndex]->getNumberValue();}
		bool getCurBoolVal() {return Tokens[CurIndex]->getBoolValue();}
		bool printTokens();
		int getCurIndex() {return CurIndex;}
		int getLine() {return Tokens[CurIndex]->getLine()+1;}
		std::string getFile() {return Tokens[CurIndex]->getFile();}
		bool applyTokenIndex(int index) {CurIndex = index;return true;}

	private:

	protected:


};

TokenStream *LexicalAnalysis(std::string input_filename);
#endif
