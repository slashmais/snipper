
#include "dbsnip.h"
#include <utilfuncs/utilfuncs.h>
#include "snippet.h"

#include <string>


//--------------------------------------------------------------------------------------------------
DB_SNIP DB;


//--------------------------------------------------------------------------------------------------(schema)
bool DB_SNIP::ImplementSchema()
{
	bool b=bDBOK;
	DBResult RS;
	std::string sSQL;

	if (b)
	{
		sSQL = "CREATE TABLE IF NOT EXISTS IDS (name, id, del)";
		ExecSQL(&RS, sSQL); //create/reuse ids: init_ids(tabelname) once after CREATE TABLE.. and new_id() / del_id() after
		b=NoError(this);
	}
	if (b) b = (MakeTable("snippets", "idsnip, name, snip")&&init_ids("snippets"));

	return b;
}

//--------------------------------------------------------------------------------------------------(snippets)
bool DB_SNIP::Save(Snippet &snippet)
{
	std::string sSQL;
	DBResult RS;
	//bool b=false;
	if (snippet.id)
	{
		sSQL=spf("UPDATE snippets SET",
					" name = ", SQLSafeQ(snippet.name),
					", snip = ", SQLSafeQ(snippet.snip),
					" WHERE idsnip = ", snippet.id);
		ExecSQL(&RS, sSQL);
	}
	else
	{
		snippet.id=new_id("snippets");
		sSQL=spf("INSERT INTO snippets(idsnip, name, snip) VALUES(",
					snippet.id,
					", ", SQLSafeQ(snippet.name),
					", ", SQLSafeQ(snippet.snip), ")");
		ExecSQL(&RS, sSQL);
	}
	return NoError(this);
}

bool DB_SNIP::Load(Snippets &snippets)
{
	DBResult RS;
	size_t i=0,n;

	snippets.clear();

	n = ExecSQL(&RS, "SELECT * FROM snippets");
	if (NoError(this))
	{
		while (i<n)
		{
			Snippet snippet;
			snippet.id = stot<size_t>(RS.GetVal("idsnip", i));
			snippet.name = SQLRestore(RS.GetVal("name", i));
			snippet.snip = SQLRestore(RS.GetVal("snip", i));
			snippets[snippet.id]=snippet;
			i++;
		}
		return true;
	}
	return false;
}

bool DB_SNIP::Delete(size_t idsnip)
{
	std::string sSQL;
	DBResult RS;
	sSQL=spf("DELETE FROM snippets WHERE idsnip = ", idsnip);
	ExecSQL(&RS, sSQL);
	if (NoError(this)) return del_id("snippets", idsnip);
	return false;
	//return NoError(this);
}

bool DB_SNIP::ValidateHasTable(const std::string &tname)
{
	DBResult RS;
	ExecSQL(&RS, spf("SELECT * FROM ", tname));
	return (NoError(this));
}

