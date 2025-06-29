# pg_dropcache Makefile

MODULE_big = pg_dropcache
OBJS = pg_dropcache.o $(WIN32RES)

EXTENSION = pg_dropcache
DATA = pg_dropcache--0.2.sql
PGFILEDESC = "pg_dropcache - clears buffer cache"

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
