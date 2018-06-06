#ifndef H_textsch
#define H_textsch

// Textsch.cpp: 定义控制台应用程序的入口点。
//
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <set>
#include <iterator>
#include <algorithm>
#include <sstream>
using namespace std;

class QueryResult;
class TextQuery { //类
public:
	using line_no = std::vector<std::string>::size_type;
	TextQuery(std::ifstream&);
	QueryResult query(const std::string&) const;
private:
	std::shared_ptr<std::vector<std::string>> file;
	std::map<std::string, std::shared_ptr<std::set<line_no>>> wm;//单词到其出现的行号的映射
};

class QueryResult {
	//using line_no = std::vector<std::string>::size_type;
	using line_no = TextQuery::line_no;
	friend ostream& print(std::ostream&, const QueryResult&);
public:
	QueryResult(std::string s, std::shared_ptr<std::set<line_no>> p,
		std::shared_ptr<std::vector<std::string>> f) :sought(s), lines(p), file(f) {}
	set<line_no>::iterator begin() { return lines->begin(); }
	set<line_no>::iterator end() { return lines->end(); }
	shared_ptr<std::vector<std::string>> get_file() { return file; }

private:
	std::string sought;
	std::shared_ptr<std::set<line_no>> lines;
	std::shared_ptr<std::vector<std::string>> file;
};

//Query_base类
class Query_base {
	friend class Query;
protected:
	using line_no = TextQuery::line_no;
	virtual ~Query_base() = default;
private:
	virtual QueryResult eval(const TextQuery&) const = 0;
	virtual std::string rep() const = 0;
};

//Query类
class Query {
	friend Query operator~(const Query &);
	friend Query operator|(const Query &, const Query &);
	friend Query operator&(const Query &, const Query&);
public:
	Query(const std::string &);
	QueryResult eval(const TextQuery &t)const {
		return q->eval(t);
	}
	std::string rep()const { return q->rep(); }
private:
	Query(std::shared_ptr<Query_base> query) :q(query) {}
	std::shared_ptr<Query_base>q;
};

//WordQuery类
class WordQuery : public Query_base {
	friend class Query;
	WordQuery(const std::string &s) :query_word(s) {}
	QueryResult eval(const TextQuery &t)const {
		return t.query(query_word);
	}
	std::string rep()const { return query_word; }
	std::string query_word;
};

//Query的构造函数
inline Query::Query(const std::string &s) :q(new WordQuery(s)) {}

//NotQuery类
class NotQuery : public Query_base {
	friend Query operator~(const Query &);
	NotQuery(const Query &q) :query(q) {}
	std::string rep()const { return "~(" + query.rep() + ")"; }
	QueryResult eval(const TextQuery&) const;
	Query query;
};
inline Query operator~(const Query &operand) {
	return std::shared_ptr<Query_base>(new NotQuery(operand));
}

//BinaryQuery类,抽象类
class BinaryQuery : public Query_base {
protected:
	BinaryQuery(const Query &l, const Query &r, std::string s) :lhs(l), rhs(r), opSym(s) {}
	std::string rep() const { return "(" + lhs.rep() + " " + opSym + " " + rhs.rep() + ")"; }
	Query lhs, rhs;
	std::string opSym;
};

//AndQuery类，OrQuery类
class AndQuery : public BinaryQuery {
	friend Query operator& (const Query&, const Query&);
	AndQuery(const Query &left, const Query &right) :BinaryQuery(left, right, "&") {}
	QueryResult eval(const TextQuery &) const;
};
inline Query operator&(const Query &lhs, const Query &rhs) {
	return std::shared_ptr<Query_base>(new AndQuery(lhs, rhs));
}
class OrQuery : public BinaryQuery {
	friend Query operator|(const Query &, const Query &);
	OrQuery(const Query &left, const Query &right) :BinaryQuery(left, right, "|") {}
	QueryResult eval(const TextQuery&)const;
};
inline Query operator|(const Query &lhs, const Query &rhs) {
	return std::shared_ptr<Query_base>(new OrQuery(lhs, rhs));
}
#endif // !H_textsch
