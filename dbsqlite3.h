#ifndef _dbsqlite3_h_
#define _dbsqlite3_h_

#include <string>
#include <vector>
#include <sqlite3.h>

//--------------------------------------------------------------------------------------------------
//typedef size_t IDTYPE; //uint64_t?

//typedef std::vector<IDTYPE> IDLIST;

//--------------------------------------------------------------------------------------------------
struct DBsqlite3
{
	struct DBResult
	{
		struct ColVal
		{
			ColVal(const std::string sc,const std::string sv):col(sc),val(sv) {}
			std::string col, val;
		};
	
		typedef std::vector<ColVal> row_t;
	
		std::vector<row_t> Rows;
		std::string sQuery;
		void AddColVal(const std::string colname, const std::string value, row_t &r);
		void AddRow(row_t &r);
	
		DBResult();
		~DBResult();
		void Clear();
		const std::string GetQuery();
		int GetColCount();
		int GetRowCount();
		const std::string GetName(int nCol, int nRow);
		const std::string GetVal(int nCol, int nRow);
		const std::string GetVal(const std::string sCol, int nRow);
	//friend class DBsqlite3;
	};
	
	sqlite3 *pDB;
	std::string sDB, sLastError, sLastSQL;
	DBResult *pRS;
	
	static int Callback(void *pCaller, int argc, char **argv, char **azColName);

	bool bDBOK;

	DBsqlite3();
	virtual ~DBsqlite3();

	const char IDS_DELIM=(const char)',';
	bool init_ids(const std::string &tname);
	size_t new_id(const std::string &tablename);
	bool del_id(const std::string &tablename, size_t id);
	
	bool Open(const std::string sDBName);
	void Close();
	size_t ExecSQL(DBResult *pRSet, const std::string sSQL);
	bool MakeTable(const std::string sT, const std::string sF);
	bool ValidateHasTable(const std::string &stname);
	const std::string GetLastError();
};


//--------------------------------------------------------------------------------------------------
const std::string SQLSafeQ(const std::string &sval); //..Q=>returned string wrapped in single-quotes
const std::string SQLRestore(const std::string &sval);
bool NoError(DBsqlite3 *pdb);







#endif
