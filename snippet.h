#ifndef _snipper_snippet_h_
#define _snipper_snippet_h_

#include <string>
#include <map>
#include <dbsqlite3/dbsqlite3types.h>


struct Snippet
{
//	size_t id;
	DB_ID_TYPE id;
	std::string name;
	std::string snip;
	
	Snippet();
	Snippet(const Snippet &R);
	virtual ~Snippet();
};


struct Snippets : public std::map<size_t, Snippet>
{
	Snippets();
	virtual ~Snippets();
};




#endif
