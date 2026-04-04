# pg_dropcache

`pg_dropcache` is a PostgreSQL extension that invalidates `shared_buffers` cache.

PostgreSQL supported versions are 11, 12, 13, 14, 15 and 16.

PostgreSQL 17 and 18 are not supported because pg_buffercache extension provides new following functions:
- `pg_buffercache_evict()`
- `pg_buffercache_evict_relation()`
- `pg_buffercache_evict_all()`

## Installation

To install `pg_dropcache` clone this repository and run:

```
make install USE_PGXS=1
```

Then in psql (or any other client) execute:

```
create extension pg_dropcache;
```

## Usage

**WARNING**: Dirty pages will be just dropped, therefore they won't be flushed on the disk! It should be used with extreme caution!

To clear whole buffer cache run:

```
select pg_drop_cache();
```

To clear cache buffers for just a single relation:

```
select pg_drop_rel_cache(<relation oid>);
```

Use it at your own risk.
