// Textsch.cpp: 定义控制台应用程序的入口点。
#include "textsch.h"

TextQuery::TextQuery(std::ifstream& is) :file(new std::vector<std::string>) {
	std::string text;
	using line_no = std::vector<std::string>::size_type;
	while (getline(is,text))
	{
		file->push_back(text);
		int n = file->size() - 1;
		istringstream line(text);
		string word;
		while (line >> word) {//将出现的每个单词所在行号，添加到单词到行号集合的映射中
			auto &lines = wm[word];
			if (!lines)
				lines.reset(new set<line_no>);
			lines->insert(n);
		}

	}
}

QueryResult TextQuery::query(const string &sought)const {
	static shared_ptr<set<line_no>> nodata(new set < line_no>);
	auto loc = wm.find(sought);
	if (loc == wm.end()) {
		return QueryResult(sought, nodata, file);
	}
	else {
		return QueryResult(sought, loc->second, file);
	}
}
string make_plural(int s, string t, string ts) {
	return s > 1 ? t + ts : t;
}
ostream& print(ostream& os, const QueryResult &qr) {
	os << qr.sought << " occurs " << qr.lines->size() << " " 
		<< make_plural(qr.lines->size(), "time", "s") << endl;
	for (auto num : *qr.lines) {
		os << "\t(line " << num + 1 << ") " << *(qr.file->begin() + num) << endl;
	}
	return os;
}

//Query的输出
std::ostream & operator<<(std::ostream &os, const Query &query) {
	return os << query.rep();
}

//OrQuery的eval函数
QueryResult OrQuery::eval(const TextQuery& text)const {
	auto right = rhs.eval(text), left = lhs.eval(text);
	auto ret_lines = make_shared<set<line_no>>(left.begin(), left.end());
	ret_lines->insert(right.begin(), right.end());
	return QueryResult(rep(), ret_lines, left.get_file());
}
//AndQuery的eval函数
QueryResult AndQuery::eval(const TextQuery& text)const {
	auto left = lhs.eval(text), right = rhs.eval(text);
	auto ret_lines = make_shared<set<line_no>>();
	set_intersection(left.begin(), left.end(), right.begin(), right.end(),
		inserter(*ret_lines, ret_lines->begin()));
	return QueryResult(rep(), ret_lines, left.get_file());
}
//NotQuery的eval函数
QueryResult NotQuery::eval(const TextQuery& text)const {
	auto result = query.eval(text);
	auto ret_lines = make_shared<set<line_no>>();
	auto beg = result.begin(), end = result.end();
	auto sz = result.get_file()->size();
	for (size_t n = 0; n != sz; ++n) {
		if (beg == end || *beg != n) {
			ret_lines->insert(n);
		}
		else if(beg != end) {
			++beg;
		}
	}
	return QueryResult(rep(), ret_lines, result.get_file());
}


