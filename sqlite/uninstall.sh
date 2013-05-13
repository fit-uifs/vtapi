#!/bin/sh
sqlite3 vtapi_public.db < drop_public.sql
sqlite3 vtapi_vidte.db < drop_dataset.sql
