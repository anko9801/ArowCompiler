#ifndef AST_HPP 
#define AST_HPP


#include<string>
#include<map>
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
class NumberAST;
class BooleanAST;


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
	NumberID,
	BooleanID,
	IfExprID,
};

struct Seq {
	std::string Type;
	std::string Name;

	Seq(std::string type, std::string name) : Type(type), Name(name){}
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
	std::string Type;
	std::string Name;
	std::vector<Seq> Params;

	public:
	PrototypeAST(const std::string &type, const std::string &name, const std::vector<Seq> &params)
		: Type(type), Name(name), Params(params){}

	std::string getName(){return Name;}
	std::string getType(){return Type;}
	std::string getParamName(int i){if(i<Params.size())return Params.at(i).Name;return NULL;}
	std::string getParamType(int i){if(i<Params.size())return Params.at(i).Type;return NULL;}
	int getParamNum(){return Params.size();}
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

	std::string getType(){return Proto->getType();}
	std::string getName(){return Proto->getName();}
	PrototypeAST *getPrototype(){return Proto;}
	FunctionStmtAST *getBody(){return Body;}
};


/** 
  * 関数定義(本文)を表すAST
  */
class FunctionStmtAST{
	std::vector<VariableDeclAST*> VariableDecls;
	std::vector<BaseAST*> StmtLists;

	public:
	FunctionStmtAST(){}
	~FunctionStmtAST();

	bool addVariableDeclaration(VariableDeclAST *vdecl);
	bool addStatement(BaseAST *stmt){StmtLists.push_back(stmt);}

	VariableDeclAST *getVariableDecl(int i){if(i<VariableDecls.size())return VariableDecls.at(i);else return NULL;}
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
		std::string Type;
		std::string Name;
		DeclType Decltype;
	public:
		VariableDeclAST(const std::string &type, const std::string &name) : BaseAST(VariableDeclID), Type(type), Name(name){
		}
		static inline bool classof(VariableDeclAST const*){return true;}
		static inline bool classof(BaseAST const* base){
			return base->getValueID()==VariableDeclID;
		}
		~VariableDeclAST(){}

		bool setDeclType(DeclType type){Decltype=type;return true;};

		std::string getName(){return Name;}
		std::string getType(){return Type;}
		DeclType getDeclType(){return Decltype;}
};



/** 
  * 二項演算を表すAST
  */
class  BinaryExprAST : public BaseAST{
	std::string Type;
	std::string Op;
	BaseAST *LHS, *RHS;

	public:
	BinaryExprAST(std::string op, BaseAST *lhs, BaseAST *rhs)
		: BaseAST(BinaryExprID), Op(op), LHS(lhs), RHS(rhs){
		}
	~BinaryExprAST(){SAFE_DELETE(LHS);SAFE_DELETE(RHS);}
	static inline bool classof(BinaryExprAST const*){return true;}
	static inline bool classof(BaseAST const* base){
		return base->getValueID()==BinaryExprID;
	}

	bool setType(std::string type){Type = type;return true;}

	std::string getOp(){return Op;}
	BaseAST *getLHS(){return LHS;}
	BaseAST *getRHS(){return RHS;}
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
	std::string Type;
	std::string Callee;
	std::vector<BaseAST*> Args;

	public:
	CallExprAST(const std::string &type, const std::string &callee, std::vector<BaseAST*> &args)
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
	std::string Type;
	std::string Name;

	public:
	VariableAST(const std::string type, const std::string &name) : BaseAST(VariableID), Type(type), Name(name){}
	~VariableAST(){}

	static inline bool classof(VariableAST const*){return true;}
	static inline bool classof(BaseAST const* base){
		return base->getValueID()==VariableID;
	}

	std::string getType(){return Type;}
	std::string getName(){return Name;}
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

	std::string getType(){return "int";}
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

	std::string getType(){return "bool";}
	bool getValue(){return Val;}
};


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


#endif
