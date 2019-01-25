#ifndef AST_HPP 
#define AST_HPP

#include<string>
#include<vector>
#include<llvm/Support/Casting.h>
#include"APP.hpp"

/************************************************
AST
************************************************/

/**
  * クラス宣言
  */
class BaseAST;
class TranslationUnitAST;
class ImportAST;
class FunctionAST;
class PrototypeAST;
class StatementsAST;
class FunctionStmtAST;
class VariableDeclAST;
class BinaryExprAST;
class SingleExprAST;
class CallExprAST;
class JumpStmtAST;
class IfStmtAST;
class WhileStmtAST;
class PlaceholderAST;
class VariableAST;
class CastExprAST;
class ValueAST;


/**
  * ASTの種類
  */
enum AstID{
	BaseID,
	ImportID,
	FunctionStmtID,
	StatementsID,
	VariableDeclID,
	SingleExprID,
	BinaryExprID,
	IfStmtID,
	WhileStmtID,
	CallExprID,
	JumpStmtID,
	CastExprID,
	VariableID,
	PlaceholderID,
	ValueID,
};

enum prim_type {
	Type_void,
	Type_bool,
	Type_number,
	Type_int,
	Type_uint,
	Type_float,
	Type_char,
	Type_all,
	Type_null,
	Type_none,
};

struct Types {
	prim_type Type;
	int bits;
	bool non_null = false;
	int ArraySize = 0;

	Types() : Type(Type_null), bits(0) {}
	Types(prim_type type, int bit=32, bool non_null = false)
		: Type(type), bits(bit), non_null(non_null) {if (type == Type_bool) bits = 1;}
	Types(prim_type type, int size, int bits=32, bool non_null = false)
		: Type(type), bits(bits), non_null(non_null), ArraySize(size) {}

	bool setNonNull(bool NonNull) {non_null = NonNull;return true;}
	bool setBits(int size) {bits = size;return true;}
	bool setArray(int size) {ArraySize = size;return true;}

	int getBits() {return bits;}
	prim_type getPrimType() {return Type;}
	bool getNonNull() {return non_null;}
	int getArraySize() {return ArraySize;}
	std::string printType();
	bool operator== (const Types &rhs) const;
	bool operator!= (const Types &rhs) const;
};

struct Seq {
	Types Type;
	std::string Name;

	Seq(Types type, std::string name) : Type(type), Name(name) {}
	bool operator< (const Seq &rhs) const;
	bool operator== (const Seq &rhs) const;
};



/**
  * ASTの基底クラス
  */
class BaseAST {
	AstID ID;

	public:
	BaseAST(AstID id):ID(id) {}
	virtual ~BaseAST() {}
	AstID getValueID() const {return ID;}
	Types getType();
	bool setType(Types type);
	void printAST(int nest);
};


/** 
  * ソースコードを表すAST
  */
class TranslationUnitAST {
	std::vector<PrototypeAST*> Prototypes;
	std::vector<FunctionAST*> Functions;
	std::vector<ImportAST*> Imports;

	public:
		TranslationUnitAST() {}
		~TranslationUnitAST();

		bool addPrototype(PrototypeAST *proto) {
			Prototypes.push_back(proto);
			return true;
		}
		bool addFunction(FunctionAST *func) {
			Functions.push_back(func);
			return true;
		}
		bool addImport(ImportAST *import) {
			Imports.push_back(import);
			return true;
		}
		bool empty() {
			if (Prototypes.size()==0 && Functions.size()==0)
				return true;
			else
				return false;
        }

		PrototypeAST *getPrototype(size_t i) {
			if (i < Prototypes.size())
				return Prototypes.at(i);
			else
			    return NULL;
		}
		FunctionAST *getFunction(size_t i) {
			if (i < Functions.size())
				return Functions.at(i);
			else
				return NULL;
		}
		ImportAST *getImport(size_t i) {
			if (i < Imports.size())
				return Imports.at(i);
			else
				return NULL;
		}
};


class ImportAST : public BaseAST {
	std::string filename;

	public:
	ImportAST(std::string filename) : BaseAST(ImportID), filename(filename) {}
	~ImportAST() {}
	static inline bool classof(ImportAST const*) {return true;}
	static inline bool classof(BaseAST const* base) {
		return base->getValueID() == ImportID;
	}

	std::string getFileName() {return filename;}
};


/** 
  * 関数宣言を表すAST
  */
class PrototypeAST {
	Types Type;
	std::string Name;
	std::vector<Seq> Params;

	public:
	PrototypeAST(const Types &type, const std::string &name, const std::vector<Seq> &params)
		: Type(type), Name(name), Params(params) {}

	bool setType(Types type) {Type = type;return true;}

	std::string getName() {return Name;}
	Types getType() {return Type;}
	std::string getParamName(size_t i) {if (i<Params.size())return Params.at(i).Name;return NULL;}
	Types getParamType(size_t i) {if (i<Params.size())return Params.at(i).Type;return Types(Type_null);}
	size_t getParamSize() {return Params.size();}
	std::vector<Seq> getParam() {return Params;}
};


/**
  * 関数定義を表すAST
  */
class FunctionAST {
	PrototypeAST *Proto;
	FunctionStmtAST *Body;

	public:
	FunctionAST(PrototypeAST *proto, FunctionStmtAST *body) : Proto(proto), Body(body) {}
	FunctionAST(PrototypeAST *proto) : Proto(proto) {}
	~FunctionAST();

	bool setType(Types type) {Proto->setType(type);return true;}
	Types getType() {return Proto->getType();}
	std::string getName() {return Proto->getName();}
	PrototypeAST *getPrototype() {return Proto;}
	FunctionStmtAST *getBody() {return Body;}
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
		Types Type;
		std::string Name;
		DeclType Decltype;

	public:
		VariableDeclAST(const Types &type, const std::string &name) : BaseAST(VariableDeclID), Type(type), Name(name) {}
		static inline bool classof(VariableDeclAST const*) {return true;}
		static inline bool classof(BaseAST const* base) {
			return base->getValueID() == VariableDeclID;
		}
		~VariableDeclAST() {}

		bool setDeclType(DeclType type) {Decltype = type;return true;};
		bool setType(Types type) {Type = type;return true;}

		std::string getName() {return Name;}
		Types getType() {return Type;}
		DeclType getDeclType() {return Decltype;}
};


class StatementsAST : BaseAST {
	std::vector<BaseAST*> Statements = {};
	std::vector<VariableDeclAST*> OldVars = {};
	std::vector<VariableDeclAST*> NewVars = {};

	public:
	StatementsAST() : BaseAST(StatementsID) {}
	~StatementsAST();
	static inline bool classof(StatementsAST const*) {return true;}
	static inline bool classof(BaseAST const* base) {
		return base->getValueID() == StatementsID;
	}

	bool setOldVars(std::vector<VariableDeclAST*> vars) {
		OldVars = vars;
		return true;
	}
	bool addStatement(BaseAST *stmt) {
		Statements.push_back(stmt);
		if (llvm::isa<VariableDeclAST>(stmt)) {
			NewVars.push_back(llvm::dyn_cast<VariableDeclAST>(stmt));
		}
		return true;
	}
	bool clear() {
		Statements.clear();
		NewVars.clear();
		OldVars.clear();
		return true;
	}
	size_t getSize() {return Statements.size();}
	
	BaseAST *getStatement(size_t i) {if (i<Statements.size())return Statements.at(i);else return NULL;}
	VariableDeclAST *getOldVar(size_t i) {if (i<OldVars.size())return OldVars.at(i);else return NULL;}
	VariableDeclAST *getNewVar(size_t i) {if (i<NewVars.size())return NewVars.at(i);else return NULL;}
	std::vector<VariableDeclAST*> getVars() {
		std::vector<VariableDeclAST*> vars = OldVars;
		for (int i = 0;;i++) {
			if (!this->getNewVar(i)) break;
			vars.push_back(this->getNewVar(i));
		}
		return vars;
	}
	std::vector<VariableDeclAST*> getOldVars() {return OldVars;}
	std::vector<VariableDeclAST*> getNewVars() {return NewVars;}
};


/**
  * 関数定義(本文)を表すAST
  */
class FunctionStmtAST : public BaseAST {
	StatementsAST *stmts = new StatementsAST();

	public:
	FunctionStmtAST() : BaseAST(FunctionStmtID) {}
	~FunctionStmtAST() {}
	static inline bool classof(FunctionStmtAST const*) {return true;}
	static inline bool classof(BaseAST const* base) {
		return base->getValueID() == FunctionStmtID;
	}
	bool addStatement(BaseAST *stmt) {stmts->addStatement(stmt);return true;}

	BaseAST *getStatement(size_t i) {return stmts->getStatement(i);}
	StatementsAST *getStatements() {return stmts;}
};


/** 
  * 二項演算を表すAST
  */
class BinaryExprAST : public BaseAST {
	Types Type;
	std::string Op;
	BaseAST *LHS, *RHS;

	public:
	BinaryExprAST(std::string op, BaseAST *lhs, BaseAST *rhs, Types type)
		: BaseAST(BinaryExprID), Type(type), Op(op), LHS(lhs), RHS(rhs) {}
	~BinaryExprAST();
	static inline bool classof(BinaryExprAST const*) {return true;}
	static inline bool classof(BaseAST const* base) {
		return base->getValueID() == BinaryExprID;
	}

	bool setType(Types type) {Type = type;return true;}

	Types getType() {return Type;}
	std::string getOp() {return Op;}
	BaseAST *getLHS() {return LHS;}
	BaseAST *getRHS() {return RHS;}
};


/**
 * If文を表すAST
 */
class IfStmtAST : public BaseAST {
	BaseAST *Cond;
	StatementsAST *ThenStmt = new StatementsAST();
	StatementsAST *ElseStmt = new StatementsAST();

	public:
	IfStmtAST(BaseAST *Cond) : BaseAST(IfStmtID), Cond(Cond) {}
	~IfStmtAST() {}
	static inline bool classof(IfStmtAST const*) {return true;}
	static inline bool classof(BaseAST const* base) {
		return base->getValueID() == IfStmtID;
	}

	BaseAST* getCond() {return Cond;}
	bool addThen(BaseAST *stmt) {ThenStmt->addStatement(stmt);return true;}
	BaseAST *getThen(size_t i) {return ThenStmt->getStatement(i);}
	StatementsAST *getThens() {return ThenStmt;}
	bool addElse(BaseAST *stmt) {ElseStmt->addStatement(stmt);return true;}
	BaseAST *getElse(size_t i) {return ElseStmt->getStatement(i);}
	StatementsAST *getElses() {return ElseStmt;}
};


/**
 * While文を表すAST
 */
class WhileStmtAST : public BaseAST {
	BaseAST *Cond;
	StatementsAST *LoopStmt = new StatementsAST();

	public:
	WhileStmtAST(BaseAST *Cond) : BaseAST(WhileStmtID), Cond(Cond) {}
	~WhileStmtAST() {}
	static inline bool classof(WhileStmtAST const*) {return true;}
	static inline bool classof(BaseAST const* base) {
		return base->getValueID() == WhileStmtID;
	}

	BaseAST *getCond() {return Cond;}
	bool addLoop(BaseAST *stmt) {LoopStmt->addStatement(stmt);return true;}
	StatementsAST *getLoops() {return LoopStmt;}
	BaseAST *getLoop(size_t i) {return LoopStmt->getStatement(i);}
};


/** 
  * 関数呼び出しを表すAST
  */
class CallExprAST : public BaseAST {
	Types Type;
	std::string Callee;
	std::vector<BaseAST*> Args;
	PrototypeAST *proto;

	public:
	CallExprAST(const Types &type, const std::string &callee, std::vector<BaseAST*> &args, PrototypeAST *proto)
		: BaseAST(CallExprID), Type(type), Callee(callee), Args(args), proto(proto) {}
	~CallExprAST() {}
	static inline bool classof(CallExprAST const*) {return true;}
	static inline bool classof(BaseAST const* base) {
		return base->getValueID() == CallExprID;
	}

	bool setType(Types type) {Type = type;return true;}

	std::string getCallee() {return Callee;}
	BaseAST *getArgs(size_t i) {if (i<Args.size())return Args.at(i);else return NULL;}
	Types getType() {return Type;}
	PrototypeAST *getProto() {return proto;}
};


/** 
  * ジャンプ(今回はreturn)を表すAST
  */
class JumpStmtAST : public BaseAST {
	BaseAST *Expr;

	public:
	JumpStmtAST(BaseAST *expr) : BaseAST(JumpStmtID), Expr(expr) {}
	~JumpStmtAST() {}
	static inline bool classof(JumpStmtAST const*) {return true;}
	static inline bool classof(BaseAST const* base) {
		return base->getValueID() == JumpStmtID;
	}

	BaseAST *getExpr() {return Expr;}
	Types getType() {return Expr->getType();}
};


/** 
  * 変数参照を表すAST
  */
class VariableAST : public BaseAST {
	VariableDeclAST* Var;
	int Index;

	public:
	VariableAST(VariableDeclAST* var_decl, const int &index = 0) : BaseAST(VariableID), Var(var_decl), Index(index) {}
	~VariableAST() {}

	static inline bool classof(VariableAST const*) {return true;}
	static inline bool classof(BaseAST const* base) {
		return base->getValueID() == VariableID;
	}

	bool setType(Types type) {Var->setType(type);return true;}

	Types getType() {return Var->getType();}
	std::string getName() {return Var->getName();}
	int getIndex() {return Index;}
};


/** 
  * キャストを表すAST
  */
class CastExprAST : public BaseAST {
	BaseAST *Source;
	Types DestType;
	bool Nestin;

	public:
	CastExprAST(BaseAST* source, Types DestType, bool Nestin = false) : BaseAST(CastExprID), Source(source), DestType(DestType), Nestin(Nestin) {}
	~CastExprAST() {}

	static inline bool classof(CastExprAST const*) {return true;}
	static inline bool classof(BaseAST const* base) {
		return base->getValueID() == CastExprID;
	}

	bool setDestType(Types type) {DestType = type;return true;}

	BaseAST *getSource() {return Source;}
	Types getType() {if (Nestin) return Types(Type_bool, 1, true);else return DestType;}
	Types getDestType() {return DestType;}
	bool getNestin() {return Nestin;}
};


/** 
  * １つ演算を表すAST
  */
class SingleExprAST : public BaseAST {
	Types Type;
	std::string Op;
	BaseAST *LHS;

	public:
	SingleExprAST(std::string op, BaseAST *lhs, Types type)
		: BaseAST(SingleExprID), Type(type), Op(op), LHS(lhs) {}
	~SingleExprAST();
	static inline bool classof(SingleExprAST const*) {return true;}
	static inline bool classof(BaseAST const* base) {
		return base->getValueID() == SingleExprID;
	}

	bool setType(Types type) {Type = type;return true;}

	Types getType() {return Type;}
	std::string getOp() {return Op;}
	BaseAST *getLHS() {return LHS;}
};


class PlaceholderAST : public BaseAST {
	public:
	PlaceholderAST() : BaseAST(PlaceholderID) {};
	~PlaceholderAST() {}
	static inline bool classof(PlaceholderAST const*) {return true;}
	static inline bool classof(BaseAST const* base) {
		return base->getValueID() == PlaceholderID;
	}
};

/** 
  * 値を表すAST
  */
class ValueAST : public BaseAST {
	Types Type;
	double Val;

	public:
	ValueAST(double val, Types Type) : BaseAST(ValueID), Type(Type), Val(val) {};
	~ValueAST() {}
	static inline bool classof(ValueAST const*) {return true;}
	static inline bool classof(BaseAST const* base) {
		return base->getValueID() == ValueID;
	}

	bool setType(Types type) {Type = type;return true;}
	Types getType() {return Type;}
	double getValue() {return Val;}
};


#endif