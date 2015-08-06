#!/bin/sh
sqlite3 vtapi_public.db < create_public.sql
sqlite3 vtapi_vidte.db < create_dataset.sql
