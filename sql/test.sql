create extension pg_dropcache;
create extension pg_buffercache;
create table t(x int);
insert into t select * from generate_series(1,1000000);
select page_count > 4000 from (
  select count(*) as page_count from pg_buffercache where relfilenode = pg_relation_filenode('t'::regclass)
) as foo;
select pg_drop_rel_cache('t'::regclass);
select count(*) from pg_buffercache where relfilenode = pg_relation_filenode('t'::regclass); 


