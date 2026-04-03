\echo Use "CREATE EXTENSION pg_dropcache" to load this file. \quit

CREATE OR REPLACE FUNCTION pg_drop_cache()
RETURNS VOID
AS 'pg_dropcache', 'pg_drop_cache'
LANGUAGE C;

CREATE OR REPLACE FUNCTION pg_drop_rel_cache(relation regclass)
RETURNS VOID
AS 'pg_dropcache', 'pg_drop_rel_cache'
LANGUAGE C;
