
/*-------------------------------------------------------------------------
 *
 *  pg_dropcache.c
 *
 *
 *  Portions Copyright (c) 2018, Ildar Musin
 *  https://github.com/zilder/pg_dropcache
 *
 *   
 *  Portions Copyright (c) 2026, Pierre Forstmann
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
#include "common/relpath.h"
#include "utils/rel.h"
#include "access/relation.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(pg_drop_cache);
PG_FUNCTION_INFO_V1(pg_drop_rel_cache);

Datum
pg_drop_cache(PG_FUNCTION_ARGS)
{
	DropDatabaseBuffers(MyDatabaseId);
	PG_RETURN_VOID();
}

Datum
pg_drop_rel_cache(PG_FUNCTION_ARGS)
{
	Oid			relid = PG_GETARG_OID(0);
	BlockNumber		nblocks = 0;
	ForkNumber  		forks[MAX_FORKNUM];
	SMgrRelation 		srel;
	Relation 		rel;
	int         		nforks = 0;

	/*
	 * see RelationTruncate (storage.c) 
	 *     -> smgrtruncate (smgr.c)
	 *        -> DropRelationBuffers (bufmgr.c)
	 */

	for (ForkNumber fork = 0; fork < MAX_FORKNUM; fork++)
	{
		forks[0] = fork;
		nforks = 1;

		rel = relation_open(relid, AccessShareLock);
#if PG_VERSION_NUM >= 160000
		srel = smgropen(rel->rd_locator, rel->rd_backend);
		DropRelationBuffers(srel, forks, nforks, &nblocks);
#else
		srel  = smgropen(rel->rd_node, InvalidBackendId);
		DropRelFileNodeBuffers(srel, forks, nforks, &nblocks);
#endif
		smgrclose(srel);
		relation_close(rel, AccessShareLock);
	}

	PG_RETURN_VOID();
}
