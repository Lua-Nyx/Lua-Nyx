/*
* Copyright (C) 2011, William H. Welna All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY William H. Welna ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL William H. Welna BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "sqlite3.hpp"
#include <iostream>

using namespace std;

/* SQLite3_DB */
SQLite3_DB::SQLite3_DB() {

}

SQLite3_DB::SQLite3_DB(char *path) {
	open(path);
}

SQLite3_DB::SQLite3_DB(string path) {
	open(path);
}

void SQLite3_DB::open(string path) {
	int ret;
	if(ret=sqlite3_open(path.data(), &db)) {
		throw SQLite3_Exception(ret);
	}
}

void SQLite3_DB::open(char *path) {
	int ret;
	if(ret=sqlite3_open(path, &db)) {
		throw SQLite3_Exception(ret);
	}
}

SQLite3_Stmt *SQLite3_DB::prepare(string statement) {
	return new SQLite3_Stmt(statement, db);
}

SQLite3_Stmt *SQLite3_DB::prepare(char *statement) {
	return new SQLite3_Stmt(statement, db);
}

void SQLite3_DB::exec(string statement) {
	int ret;
	if((ret=sqlite3_exec(db, statement.data(), NULL, NULL, NULL)) != SQLITE_OK) {
		throw SQLite3_Exception(ret);
	}
}

void SQLite3_DB::exec(char *statement) {
	int ret;
	if((ret=sqlite3_exec(db, statement, NULL, NULL, NULL)) != SQLITE_OK) {
		throw SQLite3_Exception(ret);
	}
}

void SQLite3_DB::close() {
	int ret;
	if((ret=sqlite3_close(db)) != SQLITE_OK) {
		throw SQLite3_Exception(ret);
	}
}

SQLite3_DB::~SQLite3_DB() {
	close();
}

/* SQLite3_DB */

/* SQLite3_Backup */

SQLite3_Backup::SQLite3_Backup(SQLite3_DB &to, string destname, SQLite3_DB &from, string fromname) {
	SQLite3_Backup(to, destname.data(), from, fromname.data());
}

SQLite3_Backup::SQLite3_Backup(SQLite3_DB &to, char *destname, SQLite3_DB &from, char *fromname) {
	if((b=sqlite3_backup_init(to.db, destname, from.db, fromname))==NULL) {
		throw SQLite3_Exception(9999);
	}
}

int SQLite3_Backup::step(int n) {
	int ret;
	if((ret=sqlite3_backup_step(b, n)) != SQLITE_OK) {
		if(ret == SQLITE_DONE)
			return 0;
		throw SQLite3_Exception(ret);
	}
	return remaining();
}

int SQLite3_Backup::remaining() {
	return sqlite3_backup_remaining(b);
}

int SQLite3_Backup::total() {
	return sqlite3_backup_pagecount(b);
}

SQLite3_Backup::~SQLite3_Backup() {
	int ret;
	if((ret=sqlite3_backup_finish(b)) != SQLITE_OK) {
		throw SQLite3_Exception(ret);
	}
}

/* SQLite3_Backup */

/* SQLite3_Stmt */
SQLite3_Stmt::SQLite3_Stmt(char *statement, sqlite3 *db) {
	int ret;
	if((ret=sqlite3_prepare_v2(db, statement, strlen(statement), &stmt, NULL)) != SQLITE_OK) {
		throw SQLite3_Exception(ret);
	}
}

SQLite3_Stmt::SQLite3_Stmt(string statement, sqlite3 *db) {
	int ret;
	if((ret=sqlite3_prepare_v2(db, statement.data(), statement.size(), &stmt, NULL)) != SQLITE_OK) {
		throw SQLite3_Exception(ret);
	}
}

SQLite3_Stmt::SQLite3_Stmt(char *statement, SQLite3_DB &db) {
	int ret;
	if((ret=sqlite3_prepare_v2(db.db, statement, strlen(statement), &stmt, NULL)) != SQLITE_OK) {
		throw SQLite3_Exception(ret);
	}
}

SQLite3_Stmt::SQLite3_Stmt(string statement, SQLite3_DB &db) {
	int ret;
	if((ret=sqlite3_prepare_v2(db.db, statement.data(), statement.size(), &stmt, NULL)) != SQLITE_OK) {
		throw SQLite3_Exception(ret);
	}
}

int SQLite3_Stmt::next() {
	int ret;
	if((ret=sqlite3_step(stmt)) != SQLITE_ROW) {
		if(ret == SQLITE_DONE)
			return false;
		else
			throw SQLite3_Exception(ret);
	}
	return true;
}

void SQLite3_Stmt::reset() {
	int ret;
	ret=sqlite3_reset(stmt);
	if(ret != SQLITE_DONE)
		if(ret != SQLITE_ROW)
			if(ret != SQLITE_OK)
				throw SQLite3_Exception(ret);
}

void SQLite3_Stmt::bind(int col, double d) {
	int r;
	if((r=sqlite3_bind_double(stmt, col, d)) != SQLITE_OK) {
		throw SQLite3_Exception(r);
	}
}

void SQLite3_Stmt::bind(int col, int i) {
	int r;
	if((r=sqlite3_bind_int(stmt, col, i)) != SQLITE_OK) {
		throw SQLite3_Exception(r);
	}
}

void SQLite3_Stmt::bind(int col, char *s, int size) {
	int r;
	if((r=sqlite3_bind_blob(stmt, col, s, size, NULL)) != SQLITE_OK) {
		throw SQLite3_Exception(r);
	}
}

void SQLite3_Stmt::bind(int col, string s) {
	int r;
	if((r=sqlite3_bind_text(stmt, col, s.data(), s.size(), SQLITE_STATIC)) != SQLITE_OK) {
		throw SQLite3_Exception(r);
	}
}

void SQLite3_Stmt::bind(int col) {
	int r;
	if((r=sqlite3_bind_null(stmt, col)) != SQLITE_OK) {
		throw SQLite3_Exception(r);
	}
}

void SQLite3_Stmt::column(int col, double *d) {
	double t = sqlite3_column_int(stmt, col);
	(*d) = t;
}

void SQLite3_Stmt::column(int col, int *i) {
	int t = sqlite3_column_int(stmt, col);
	(*i) = t;
}

void SQLite3_Stmt::column(int col, char *s, int size) {
	void *p = (void *)sqlite3_column_blob(stmt, col);
	int ps = sqlite3_column_bytes(stmt, col), n=0;
	if(ps > size)
		n=size;
	else
		n=ps;
	memcpy(s, p, n);
}

void SQLite3_Stmt::column(int col, string &s) {
	char *p = (char *)sqlite3_column_text(stmt, col);
	s.assign(p);
}

void SQLite3_Stmt::column_name(int col, char *s, int size) {
	memcpy(s, sqlite3_column_name(stmt, col), size);
}

void SQLite3_Stmt::column_name(int col, string &s) {
	s.assign(sqlite3_column_name(stmt, col));
}

string SQLite3_Stmt::column_name(int col) {
	string t = sqlite3_column_name(stmt, col);
	return t;
}

int SQLite3_Stmt::columns() {
	return sqlite3_column_count(stmt);
}

SQLite3_Stmt::~SQLite3_Stmt() {
	sqlite3_finalize(stmt);
}
/* SQLite3_Stmt */

/* SQLite3_Exception */
SQLite3_Exception::SQLite3_Exception(const int c) {
	code=c;
}

int SQLite3_Exception::error() {
	return code;
}

char *SQLite3_Exception::get_msg(const int c) {
	switch(c) {
		case SQLITE_ERROR:	return "[SQLITE_ERROR]: SQL error or missing database";
		case SQLITE_INTERNAL:	return "[SQLITE_INTERNAL]: Internal logic error in SQLite";
		case SQLITE_PERM: 	return "[SQLITE_PERM]: Access permission denied";
		case SQLITE_ABORT: 	return "[SQLITE_ABORT]: Callback routine requested an abort";
		case SQLITE_BUSY: 	return "[SQLITE_BUSY]: The database file is locked";
		case SQLITE_LOCKED: 	return "[SQLITE_LOCKED]: A table in the database is locked";
		case SQLITE_NOMEM: 	return "[SQLITE_NOMEM]: A malloc() failed";
		case SQLITE_READONLY: 	return "[SQLITE_READONLY]: Attempt to write a readonly database";
		case SQLITE_INTERRUPT: 	return "[SQLITE_INTERRUPT]: Operation terminated by sqlite3_interrupt()";
		case SQLITE_IOERR: 	return "[SQLITE_IOERR]: Some kind of disk I/O error occurred";
		case SQLITE_CORRUPT:	return "[SQLITE_CORRUPT]: The database disk image is malformed";
		case SQLITE_NOTFOUND: 	return "[SQLITE_NOTFOUND]: Unknown opcode in sqlite3_file_control()";
		case SQLITE_FULL: 	return "[SQLITE_FULL]:Insertion failed because database is full";
		case SQLITE_CANTOPEN: 	return "[SQLITE_CANTOPEN]: Unable to open the database file";
		case SQLITE_PROTOCOL: 	return "[SQLITE_PROTOCOL]: Database lock protocol error";
		case SQLITE_EMPTY: 	return "[SQLITE_EMPTY]: Database is empty";
		case SQLITE_SCHEMA: 	return "[SQLITE_SCHEMA]: The database schema changed";
		case SQLITE_TOOBIG: 	return "[SQLITE_TOOBIG]: String or BLOB exceeds size limit";
		case SQLITE_CONSTRAINT: return "[SQLITE_CONSTRAINT]: abort due to constraint violation";
		case SQLITE_MISMATCH: 	return "[SQLITE_MISMATCH]: Data type mismatch";
		case SQLITE_MISUSE: 	return "[SQLITE_MISUSE]: Library used incorrectly";
		case SQLITE_NOLFS: 	return "[SQLITE_NOLFS]: Uses OS features not supported on host";
		case SQLITE_AUTH: 	return "[SQLITE_AUTH]: Authorization denied";
		case SQLITE_FORMAT: 	return "[SQLITE_FORMAT]: Auxiliary database format error";
		case SQLITE_RANGE: 	return "[SQLITE_RANGE]: 2nd parameter to sqlite3_bind out of range";
		case SQLITE_NOTADB: 	return "[SQLITE_MOTADB]: File opened that is not a database file";
		case SQLITE_ROW: 	return "[SQLITE_ROW]: sqlite3_step() has another row ready";
		case SQLITE_DONE: 	return "[SQLITE_DONE]: sqlite3_step() has finished executing";
		case 9999: 		return "[SQLITE_BACKUP_API]: sqlite3_backup_init() failed";
		default: 		return "[SQLITE_WTF]: Unknown error";
	}
}

void SQLite3_Exception::msg(string &s) {
	s.assign(get_msg(code));
}

string SQLite3_Exception::msg() {
	return string(get_msg(code));
} 

SQLite3_Exception::~SQLite3_Exception() {
}
/* SQLite3_Exception */
