
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
#if PG_VERSION_NUM >= 120000
#include "access/relation.h"
#else
#include "nodes/relation.h"
#endif

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
	SMgrRelation 		srel;
#if PG_VERSION_NUM >= 120000
	Relation 		rel;
#endif
#if PG_VERSION_NUM >= 130000
	ForkNumber  		forks[MAX_FORKNUM];
	int         		nforks = 0;
#endif
#if PG_VERSION_NUM < 140000
	RelFileNodeBackend	relfnode;
#endif
	/*
	 * see RelationTruncate (storage.c) 
	 *     -> smgrtruncate (smgr.c)
	 *        -> DropRelationBuffers (bufmgr.c)
	 */

	for (ForkNumber fork = 0; fork < MAX_FORKNUM; fork++)
	{
#if PG_VERSION_NUM >= 130000
		forks[0] = fork;
		nforks = 1;
#endif
#if PG_VERSION_NUM >= 120000
		rel = relation_open(relid, AccessShareLock);
#else
		Relation rel = RelationIdGetRelation(relid);
		RelationOpenSmgr(rel);
#endif
#if PG_VERSION_NUM >= 160000
		srel = smgropen(rel->rd_locator, rel->rd_backend);
		DropRelationBuffers(srel, forks, nforks, &nblocks);
#elif PG_VERSION_NUM >=140000
		srel  = smgropen(rel->rd_node, InvalidBackendId);
		DropRelFileNodeBuffers(srel, forks, nforks, &nblocks);
#elif PG_VERSION_NUM >= 130000
		srel  = smgropen(rel->rd_node, InvalidBackendId);
		relfnode.node = rel->rd_node;
		relfnode.backend = InvalidBackendId;
		DropRelFileNodeBuffers(relfnode, forks, nforks, &nblocks);
#else
		srel  = smgropen(rel->rd_node, InvalidBackendId);
		relfnode.node = rel->rd_node;
		relfnode.backend = InvalidBackendId;
		DropRelFileNodeBuffers(relfnode, fork, nblocks);
#endif
		smgrclose(srel);
#if PG_VERSION_NUM >= 120000
		relation_close(rel, AccessShareLock);
#else
		RelationClose(rel);
#endif
	}

	PG_RETURN_VOID();
}
