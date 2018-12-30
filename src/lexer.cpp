#include "lexer.hpp"

/**
 * トークン切り出し関数
 * @param 字句解析対象ファイル名
 * @return 切り出したトークンを格納したTokenStream
 */
TokenStream *LexicalAnalysis(std::string input_filename){
	bool printLex = true;
	TokenStream *tokens = new TokenStream();
	std::ifstream ifs;
	std::string cur_line;
	std::string token_str;
	int line_num = 0;
	bool iscomment = false;

	ifs.open(input_filename.c_str(), std::ios::in);
	if(!ifs)
		return NULL;
	while(ifs && getline(ifs, cur_line)){
		char next_char;
		std::string line;
		Token *next_token;
		int index = 0;
		int length = cur_line.length();
	
		while(index < length){
			next_char = cur_line.at(index++);

			//コメントアウト読み飛ばし
			if(iscomment){
				if( (length-index) < 2 
					|| (cur_line.at(index) != '*') 
					|| (cur_line.at(index++) != '/') ){
					continue;
				}else{
					iscomment=false;
				}
			}
		
			//EOF
			if(next_char == EOF){
				token_str = EOF;
				next_token = new Token(token_str, TOK_EOF, line_num);
		
			}else if(isspace(next_char)){
				continue;

			//IDENTIFIER
			}else if(isalpha(next_char)){
				token_str += next_char;
				if(index < length) {
					next_char = cur_line.at(index++);
					while(isalnum(next_char)){
						token_str += next_char;
						if(index == length){index++;break;}
						next_char = cur_line.at(index++);
						if (isnumber(next_char) && (token_str == "int" || token_str == "float" || token_str == "uint"))
							break;
					}
					index--;
				}

				// switch使いてぇええ
				// 予約語と識別子
				if(token_str == "int" || token_str == "bool" || token_str == "float" || token_str == "uint"){
					next_token = new Token(token_str, TOK_TYPE, line_num);
				}else if(token_str == "if") {
					next_token = new Token(token_str, TOK_IF, line_num);
				}else if(token_str == "while") {
					next_token = new Token(token_str, TOK_WHILE, line_num);
				}else if(token_str == "for") {
					next_token = new Token(token_str, TOK_FOR, line_num);
				}else if(token_str == "match") {
					next_token = new Token(token_str, TOK_MATCH, line_num);
				}else if(token_str == "return") {
					next_token = new Token(token_str, TOK_RETURN, line_num);
				}else if (token_str == "true") {
					next_token = new Token(token_str, TOK_TRUE, line_num);
				}else if (token_str == "false") {
					next_token = new Token(token_str, TOK_FALSE, line_num);
				}else if (token_str == "is") {
					next_token = new Token(token_str, TOK_CAST, line_num);
				}else if (token_str == "as") {
					next_token = new Token(token_str, TOK_CAST, line_num);
				}else{
					next_token = new Token(token_str, TOK_IDENTIFIER, line_num);
				}
		
			//数字
			}else if(isdigit(next_char)){
				if(next_char == '0'){
					token_str += next_char;
				}else{
					token_str += next_char;
					if(index < length) {
						next_char = cur_line.at(index++);
						while(isdigit(next_char)){
							token_str += next_char;
							if(index == length){index++;break;}
							next_char = cur_line.at(index++);
						}
						index--;
					}
				}
				
				if (next_char == '.') {
					index++;
					next_char = cur_line.at(index++);
					while(isdigit(next_char)){
						token_str += next_char;
						if(index == length){index++;break;}
						next_char = cur_line.at(index++);
					}
					index--;
					next_token = new Token(token_str, TOK_FLOAT, line_num);
				}else{
					next_token = new Token(token_str, TOK_DIGIT, line_num);
				}

			}else if(next_char == '/') {
				token_str += next_char;
				next_char = cur_line.at(index++);

				//コメントの場合
				if(next_char == '/'){
					break;

					//コメントの場合
				}else if(next_char == '*'){
					iscomment=true;
					continue;

				}else if(next_char == '='){
					token_str += next_char;
					next_token = new Token(token_str, TOK_SYMBOL, line_num);
					//DIVIDER('/')
				}else{
					index--;
					next_token = new Token(token_str, TOK_SYMBOL, line_num);
				}

			//それ以外(記号)
			}else{
				switch (next_char) {
					case '+':
						token_str += next_char;
						next_char = cur_line.at(index++);
						if (next_char == '=') {
							token_str += next_char;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}else if (next_char == '+') {
							token_str += next_char;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}else{
							index--;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}
						break;

					case '-':
						token_str += next_char;
						next_char = cur_line.at(index++);
						if (next_char == '=') {
							token_str += next_char;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}else if (next_char == '-') {
							token_str += next_char;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}else{
							index--;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}
						break;

					case '*':
						token_str += next_char;
						next_char = cur_line.at(index++);
						if (next_char == '=') {
							token_str += next_char;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}else{
							index--;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}
						break;

					case '%':
						token_str += next_char;
						next_char = cur_line.at(index++);
						if (next_char == '=') {
							token_str += next_char;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}else{
							index--;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}
						break;

					case '=':
						token_str += next_char;
						next_char = cur_line.at(index++);
						if (next_char == '=') {
							token_str += next_char;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}else if (next_char == '>') {
							token_str += next_char;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}else{
							index--;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}
						break;

					case '!':
						token_str += next_char;
						next_char = cur_line.at(index++);
						if (next_char == '=') {
							token_str += next_char;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}else{
							index--;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}
						break;

					case '>':
						token_str += next_char;
						next_char = cur_line.at(index++);
						if (next_char == '=') {
							token_str += next_char;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}else{
							index--;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}
						break;

					case '<':
						token_str += next_char;
						next_char = cur_line.at(index++);
						if (next_char == '=') {
							token_str += next_char;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}else{
							index--;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}
						break;

					case '|':
						token_str += next_char;
						next_char = cur_line.at(index++);
						if (next_char == '|') {
							token_str += next_char;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}else{
							index--;
							fprintf(stderr, "unclear token : %s\n", token_str.c_str());
							SAFE_DELETE(tokens);
							return NULL;
						}
						break;

					case '&':
						token_str += next_char;
						next_char = cur_line.at(index++);
						if (next_char == '&') {
							token_str += next_char;
							next_token = new Token(token_str, TOK_SYMBOL, line_num);
						}else{
							index--;
							fprintf(stderr, "unclear token : %s\n", token_str.c_str());
							SAFE_DELETE(tokens);
							return NULL;
						}
						break;

					case ';':
						token_str += next_char;
						next_token = new Token(token_str, TOK_SYMBOL, line_num);
						break;

					case ',':
						token_str += next_char;
						next_token = new Token(token_str, TOK_SYMBOL, line_num);
						break;

					case '(':
						token_str += next_char;
						next_token = new Token(token_str, TOK_SYMBOL, line_num);
						break;

					case ')':
						token_str += next_char;
						next_token = new Token(token_str, TOK_SYMBOL, line_num);
						break;

					case '{':
						token_str += next_char;
						next_token = new Token(token_str, TOK_SYMBOL, line_num);
						break;

					case '}':
						token_str += next_char;
						next_token = new Token(token_str, TOK_SYMBOL, line_num);
						break;

					case '[':
						token_str += next_char;
						next_token = new Token(token_str, TOK_SYMBOL, line_num);
						break;

					case ']':
						token_str += next_char;
						next_token = new Token(token_str, TOK_SYMBOL, line_num);
						break;

					case '$':
						token_str += next_char;
						next_token = new Token(token_str, TOK_SYMBOL, line_num);
						break;

					case '?':
						token_str += next_char;
						next_token = new Token(token_str, TOK_SYMBOL, line_num);
						break;

					case '.':
						token_str += next_char;
						next_token = new Token(token_str, TOK_SYMBOL, line_num);
						break;

					default:
						fprintf(stderr, "unclear token : %s\n", token_str.c_str());
						SAFE_DELETE(tokens);
						return NULL;
				}
			}
			if (printLex) fprintf(stderr, "%s ", token_str.c_str());
			//Tokensに追加
			tokens->pushToken(next_token);
			token_str.clear();
		}
		tokens->pushToken(new Token("\n", TOK_NL, line_num));

		token_str.clear();
		line_num++;
	}


	//EOFの確認
	if(ifs.eof()){
		tokens->pushToken(
				new Token(token_str, TOK_EOF, line_num)
				);
	}

	if (printLex) fprintf(stderr, "\n");

	//クローズ
	ifs.close();
	return tokens;
}


/**
  * デストラクタ
  */
TokenStream::~TokenStream(){
	for(size_t i=0; i<Tokens.size(); i++){
		SAFE_DELETE(Tokens[i]);
	}
	Tokens.clear();
}


/**
  * トークン取得
  * @return CureIndex番目のToken
  */
Token TokenStream::getToken(){
	return *Tokens[CurIndex];
}


/**
  * インデックスを一つ増やして次のトークンに進める
  * @return 成功時：true　失敗時：false
  */
bool TokenStream::getNextToken(){
	int size=Tokens.size();
	if(--size==CurIndex){
		return false;
	}else if( CurIndex < size ){
		CurIndex++;
		return true;
	}else{
		return false;
	}
}

bool TokenStream::ungetToken(int times) {
	for (int i = 0;i < times;i++) {
		if (CurIndex == 0)
			return false;
		else
			CurIndex--;
	}
	return true;
}

bool TokenStream::printTokens() {
	std::vector<Token*>::iterator titer = Tokens.begin();
	while(titer != Tokens.end()) {
		fprintf(stdout, "%d: \n", (*titer)->getTokenType());
		if ((*titer)->getTokenType() != TOK_EOF)
			fprintf(stdout, "%s\n", (*titer)->getTokenString().c_str());
		++titer;
	}
	return true;
}


