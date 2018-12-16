#ifndef AST_HPP 
#define AST_HPP


#include<string>
//#include<map>
#include<vector>
#include<llvm/Support/Casting.h>
#include"APP.hpp"
//using namespace llvm;

/************************************************
AST
************************************************/

/**
  * クラス宣言
  */
class BaseAST;
class TranslationUnitAST;
class FunctionAST;
class PrototypeAST;
class FunctionStmtAST;
class VariableDeclAST;
class BinaryExprAST;
class NullExprAST;
class CallExprAST;
class JumpStmtAST;
class VariableAST;
class IfExprAST;
class WhileExprAST;
class ArrayAST;
class TupleAST;
class NumberAST;
class BooleanAST;
class NoneAST;


/**
  * ASTの種類
  */
enum AstID{
	BaseID,
	VariableDeclID,
	BinaryExprID,
	NullExprID,
	CallExprID,
	JumpStmtID,
	VariableID,
	ArrayID,
	TupleID,
	NumberID,
	BooleanID,
	NoneID,
	IfExprID,
	WhileExprID,
};

enum prim_type {
	Type_bool,
	Type_int,
	Type_float,
	Type_char,
	Type_all,
	Type_null,
};

struct Types {
	prim_type Type;
	int bits;
	bool isArray = false;
	int ArraySize = 0;
	bool non_null = false;

	Types() : Type(prim_type::Type_null), bits(0){}
	Types(prim_type type, int bits=32, bool non_null=false)
		: Type(type), bits(bits), non_null(non_null), isArray(false), ArraySize(0){}
	Types(prim_type type, int size, int bits=32, bool non_null=false)
		: Type(type), bits(bits), non_null(non_null), isArray(true), ArraySize(size){}
	bool setArray(int size) {isArray = true;ArraySize = size;return true;}

	int getBits() {return bits;}
	prim_type getPrimType() {return Type;}
	bool getNonNull() {return non_null;}

	bool operator== (const Types &rhs) const {
		if (Type == Type_all || rhs.Type == Type_all) return true;
		if (Type == rhs.Type && non_null == rhs.non_null) return true;
		else return false;
	}
};

struct Seq {
	Types Type;
	std::string Name;

	Seq(Types type, std::string name) : Type(type), Name(name){}
	bool operator< (const Seq &rhs) const {
		if (Name < rhs.Name) return true;
		else return false;
	}
	bool operator== (const Seq &rhs) const {
		if (Type == rhs.Type && Name == rhs.Name) return true;
		else return false;
	}
};


struct Func {
	Seq function_seq;
	std::vector<Seq> param;
	Func(Types Type, std::string Name, std::vector<Seq> param) : function_seq(Seq(Type, Name)), param(param){}

	Func(Seq function, std::vector<Seq> param) : function_seq(function), param(param){}

	Types getType(){return this->function_seq.Type;}
	std::string getName(){return this->function_seq.Name;}
	int getParamNum(){return this->param.size();}
	Seq getParam(int index){return this->param[index];}

	bool operator== (const Func &rhs) const {
		if (function_seq.Name == rhs.function_seq.Name && function_seq.Type == rhs.function_seq.Type) {
			for(int i = 0;i < param.size();i++)
				if (param[i] == rhs.param[i]) return true;
		}else
			return false;
		return false;
	}
};


/**
  * ASTの基底クラス
  */
class BaseAST{
	AstID ID;

	public:
	BaseAST(AstID id):ID(id){}
	virtual ~BaseAST(){}
	AstID getValueID() const {return ID;}
	Types getType();
};


/** 
  * ソースコードを表すAST
  */
class TranslationUnitAST{
	std::vector<PrototypeAST*> Prototypes;
	std::vector<FunctionAST*> Functions;

	public:
		TranslationUnitAST(){}
		~TranslationUnitAST();

		bool addPrototype(PrototypeAST *proto);
		bool addFunction(FunctionAST *func);
		bool empty();

		PrototypeAST *getPrototype(int i){
			if(i<Prototypes.size())
				return Prototypes.at(i);
			else
			 return NULL;
		}
		FunctionAST *getFunction(int i){
			if(i<Functions.size())
				return Functions.at(i);
			else
				return NULL;
		}
};



/** 
  * 関数宣言を表すAST
  */
class PrototypeAST{
	Types Type;
	std::string Name;
	std::vector<Seq> Params;

	public:
	PrototypeAST(const Types &type, const std::string &name, const std::vector<Seq> &params)
		: Type(type), Name(name), Params(params){}

	std::string getName(){return Name;}
	Types getType(){return Type;}
	std::string getParamName(int i){if(i<Params.size())return Params.at(i).Name;return NULL;}
	Types getParamType(int i){if(i<Params.size())return Params.at(i).Type;return Types(Type_null);}
	int getParamNum(){return Params.size();}
	std::vector<Seq> getParam(){return Params;}
};



/**
  * 関数定義を表すAST
  */
class FunctionAST{
	PrototypeAST *Proto;
	FunctionStmtAST *Body;

	public:
	FunctionAST(PrototypeAST *proto, FunctionStmtAST *body) : Proto(proto), Body(body){}
	~FunctionAST();

	Types getType(){return Proto->getType();}
	std::string getName(){return Proto->getName();}
	PrototypeAST *getPrototype(){return Proto;}
	FunctionStmtAST *getBody(){return Body;}
};



/**
  * 関数定義(本文)を表すAST
  */
class FunctionStmtAST{
	std::vector<BaseAST*> StmtLists;

	public:
	FunctionStmtAST(){}
	~FunctionStmtAST();

	bool addStatement(BaseAST *stmt){StmtLists.push_back(stmt);}

	BaseAST *getStatement(int i){if(i<StmtLists.size())return StmtLists.at(i);else return NULL;}
};


/**
  * 変数宣言を表すAST
  */
class VariableDeclAST: public BaseAST {
	public:
		typedef enum{
			param,
			local
		}DeclType;

	private:
		std::vector<Types> Type;
		std::string Name;
		bool ImpType;
		int Size;
		DeclType Decltype;

	public:
		VariableDeclAST(const Types &type, const std::string &name, const int &size = 0) : BaseAST(VariableDeclID), Name(name), Size(size){std::vector<Types> a;a.push_back(type);Type = a;}
		VariableDeclAST(const std::vector<Types> &type, const std::string &name, const bool &imptype = false, const int &size = 0) : BaseAST(VariableDeclID), Type(type), Name(name), ImpType(imptype), Size(size){}
		static inline bool classof(VariableDeclAST const*){return true;}
		static inline bool classof(BaseAST const* base){
			return base->getValueID()==VariableDeclID;
		}
		~VariableDeclAST(){}

		bool setDeclType(DeclType type){Decltype=type;return true;};
		bool setType(Types type){Type[0] = type;return true;}

		std::string getName(){return Name;}
		std::vector<Types> getType(){return Type;}
		bool getImp(){return ImpType;}
		int getSize(){return Size;}
		DeclType getDeclType(){return Decltype;}
};


/** 
  * 二項演算を表すAST
  */
class  BinaryExprAST : public BaseAST{
	Types Type;
	std::string Op;
	BaseAST *LHS, *RHS;

	public:
	BinaryExprAST(std::string op, BaseAST *lhs, BaseAST *rhs, Types type)
		: BaseAST(BinaryExprID), Op(op), LHS(lhs), RHS(rhs), Type(type){
		}
	~BinaryExprAST(){SAFE_DELETE(LHS);SAFE_DELETE(RHS);}
	static inline bool classof(BinaryExprAST const*){return true;}
	static inline bool classof(BaseAST const* base){
		return base->getValueID()==BinaryExprID;
	}

	bool setType(Types type){Type = type;return true;}

	Types getType(){return Type;}
	std::string getOp(){return Op;}
	BaseAST *getLHS(){return LHS;}
	BaseAST *getRHS(){return RHS;}
};



/**
 * If文を表すAST
 */
class IfExprAST : public BaseAST {
	BaseAST *Cond;
	std::vector<BaseAST*> ThenStmt, ElseStmt;

	public:
	IfExprAST(BaseAST *Cond, std::vector<BaseAST*> Then, std::vector<BaseAST*> Else)
		: BaseAST(IfExprID),Cond(Cond), ThenStmt(Then), ElseStmt(Else){}
	~IfExprAST(){}
	static inline bool classof(IfExprAST const*){return true;}
	static inline bool classof(BaseAST const* base){
		return base->getValueID() == IfExprID;
	}

	BaseAST* getCond(){return Cond;}
	std::vector<BaseAST*> getThen(){return ThenStmt;}
	std::vector<BaseAST*> getElse(){return ElseStmt;}
};



/**
 * While文を表すAST
 */
class WhileExprAST : public BaseAST {
	BaseAST *Cond;
	std::vector<BaseAST*> LoopStmt;

	public:
	WhileExprAST(BaseAST *Cond, std::vector<BaseAST*> LoopStmt)
		: BaseAST(WhileExprID), Cond(Cond), LoopStmt(LoopStmt){}
	~WhileExprAST(){}
	static inline bool classof(WhileExprAST const*){return true;}
	static inline bool classof(BaseAST const* base){
		return base->getValueID() == WhileExprID;
	}

	BaseAST* getCond(){return Cond;}
	std::vector<BaseAST*> getLoop(){return LoopStmt;}
};


/** 
  * ";"を表すAST
  */
class NullExprAST : public BaseAST{
	public:
		NullExprAST() : BaseAST(NullExprID){}
		static inline bool classof(NullExprAST const*){return true;}
		static inline bool classof(BaseAST const* base){
		return base->getValueID()==NullExprID;
	}
};


/** 
  * 関数呼び出しを表すAST
  */
class CallExprAST : public BaseAST{
	Types Type;
	std::string Callee;
	std::vector<BaseAST*> Args;

	public:
	CallExprAST(const Types &type, const std::string &callee, std::vector<BaseAST*> &args)
		: BaseAST(CallExprID), Type(type), Callee(callee), Args(args){}
	~CallExprAST();
	static inline bool classof(CallExprAST const*){return true;}
	static inline bool classof(BaseAST const* base){
		return base->getValueID()==CallExprID;
	}
	std::string getCallee(){return Callee;}
	BaseAST *getArgs(int i){if(i<Args.size())return Args.at(i);else return NULL;}
};


/** 
  * ジャンプ(今回はreturn)を表すAST
  */
class JumpStmtAST : public BaseAST{
	BaseAST *Expr;
	public:
		JumpStmtAST(BaseAST *expr) : BaseAST(JumpStmtID), Expr(expr){}
		~JumpStmtAST(){SAFE_DELETE(Expr);}
		static inline bool classof(JumpStmtAST const*){return true;}
		static inline bool classof(BaseAST const* base){
			return base->getValueID()==JumpStmtID;
		}
		BaseAST *getExpr(){return Expr;}
};


/** 
  * 変数参照を表すAST
  */
class VariableAST : public BaseAST{
	VariableDeclAST* Var;
	int Index;

	public:
	VariableAST(VariableDeclAST* var_decl, const int &index = 0) : BaseAST(VariableID), Var(var_decl), Index(index){}
	~VariableAST(){}

	static inline bool classof(VariableAST const*){return true;}
	static inline bool classof(BaseAST const* base){
		return base->getValueID()==VariableID;
	}

	std::vector<Types> getType(){return Var->getType();}
	std::string getName(){return Var->getName();}
	int getSize(){return Var->getSize();}
	int getIndex(){return Index;}
};



/**
 * タプルを表すAST
 */
class TupleAST : public BaseAST {
};


/**
  * 配列を表すAST
  */
class ArrayAST : public BaseAST {
	Types Type;
	int Size;
	std::vector<BaseAST*> Elements;

	public:
	ArrayAST(Types type, int size, std::vector<BaseAST*> elements) : BaseAST(ArrayID), Type(type), Size(size), Elements(elements){}
	~ArrayAST(){}

	static inline bool classof(ArrayAST const*){return true;}
	static inline bool classof(BaseAST const* base){
		return base->getValueID()==ArrayID;
	}

	Types getType(){return Type;}
	int getSize(){return Size;}
	BaseAST* getElement(int i){return Elements[i];}
};


/** 
  * 整数を表すAST
  */
class NumberAST : public BaseAST {
	int Val;

	public:
	NumberAST(int val) : BaseAST(NumberID), Val(val){};
	~NumberAST(){}
	static inline bool classof(NumberAST const*){return true;}
	static inline bool classof(BaseAST const* base){
		return base->getValueID()==NumberID;
	}

	Types getType(){return Types(Type_int, 32, true);}
	int getValue(){return Val;}
};


/**
  * 真偽値を表すAST
  */
class BooleanAST : public BaseAST {
	bool Val;

	public:
	BooleanAST(bool val) : BaseAST(BooleanID), Val(val){};
	~BooleanAST(){}
	static inline bool classof(BooleanAST const*){return true;}
	static inline bool classof(BaseAST const* base){
		return base->getValueID() == BooleanID;
	}

	Types getType(){return Types(Type_bool, 1, true);}
	bool getValue(){return Val;}
};


/**
  * Noneを表すAST
  */
class NoneAST : public BaseAST {
	Types Type;

	public:
	NoneAST(Types type) : BaseAST(NoneID), Type(type){};
	~NoneAST(){}
	static inline bool classof(NoneAST const*){return true;}
	static inline bool classof(BaseAST const* base){
		return base->getValueID()==NoneID;
	}

	Types getType(){return Types(Type_null);}
};


#endif
