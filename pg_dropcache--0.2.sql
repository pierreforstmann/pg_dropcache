\echo Use "CREATE EXTENSION pg_dropcache" to load this file. \quit

do
$$
declare
 version int;
begin
 select current_setting('server_version_num')::int / 10000 into version AS major_version;
 if version >=17 then
   raise exception 'PostgreSQL version % not supported: use pg_buffercache_evict functions from pg_buffercache extension', version;
 end if;	
 if version < 11 then
   raise exception 'PostgreSQL version % not supported', version;
 end if;
end;
$$;


CREATE OR REPLACE FUNCTION pg_drop_cache()
RETURNS VOID
AS 'pg_dropcache', 'pg_drop_cache'
LANGUAGE C;

CREATE OR REPLACE FUNCTION pg_drop_rel_cache(relation regclass)
RETURNS VOID
AS 'pg_dropcache', 'pg_drop_rel_cache'
LANGUAGE C;
