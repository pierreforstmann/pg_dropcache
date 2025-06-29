
/*-------------------------------------------------------------------------
 *
 *  pg_dropcache.c
 *
 *
 *	Portions Copyright (c) 2018, Ildar Musin
 *  https://github.com/zilder/pg_dropcache
 *
 *   
 *  Portions Copyright (c) 2025, Pierre Forstmann
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"
#include "storage/bufmgr.h"
#include "utils/syscache.h"
#include "utils/builtins.h"
#include "access/htup_details.h"
#include "miscadmin.h"
#include "catalog/pg_class.h"
#include "storage/smgr.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(pg_dropcache);
PG_FUNCTION_INFO_V1(pg_drop_rel_cache);

Datum
pg_dropcache(PG_FUNCTION_ARGS)
{
	DropDatabaseBuffers(MyDatabaseId);
	PG_RETURN_VOID();
}

Datum
pg_drop_rel_cache(PG_FUNCTION_ARGS)
{
	Oid			relid = PG_GETARG_OID(0);
	HeapTuple	tp;
	RelFileLocatorBackend	rnode;
	BlockNumber	nblocks = 0;
    ForkNumber  forks[MAX_FORKNUM];
	SMgrRelation srel;
	int         nforks = 0;

	tp = SearchSysCache1(RELOID, ObjectIdGetDatum(relid));
	if (HeapTupleIsValid(tp))
	{
		Form_pg_class reltup = (Form_pg_class) GETSTRUCT(tp);
	
		rnode.locator.relNumber = reltup->relfilenode;
		rnode.locator.spcOid = (reltup->reltablespace == InvalidOid) ?
			MyDatabaseTableSpace :
			reltup->reltablespace;
		rnode.locator.dbOid = MyDatabaseId;
		rnode.backend = InvalidBackendId;

		ReleaseSysCache(tp);
	}
	else
	{
		elog(ERROR, "Unable to get cache for relation %u", relid);
		PG_RETURN_VOID();
	}

	/*
	 * see RelationTruncate (storage.c) 
	 *     -> smgrtruncate (smgr.c)
	 *        -> DropRelationBuffers (bufmgr.c)
	 */
	forks[nforks] = MAIN_FORKNUM;
	nforks = 1;
	/*
	 * TO DO: take into account FSM and visibility map
	 */
	srel  = smgropen(rnode.locator, InvalidBackendId);
	DropRelationBuffers(srel, forks, nforks, &nblocks);


	PG_RETURN_VOID();
}
