#ifndef _snipper_dbsnip_h_
#define _snipper_dbsnip_h_


#include <dbsqlite3/dbsqlite3.h>


//--------------------------------------------------------------------------------------------------
struct Snippet;
struct Snippets;

struct DB_SNIP : public DBsqlite3
{
	virtual ~DB_SNIP() {}
	
	bool ImplementSchema();

	bool Save(Snippet &snippet);
	bool Load(Snippets &snippets);
	bool Delete(size_t idsnip);

	bool ValidateHasTable(const std::string &tname);
};

extern DB_SNIP DB;




#endif
