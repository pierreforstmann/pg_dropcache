# pg_dropcache Makefile

MODULE_big = pg_dropcache
OBJS = pg_dropcache.o $(WIN32RES)

REGRESS=test
REGRESS_OPTS = --temp-instance=/tmp/5555 --port=5555

EXTENSION = pg_dropcache
DATA = pg_dropcache--1.0.0.sql
PGFILEDESC = "pg_dropcache - clears buffer cache"

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
