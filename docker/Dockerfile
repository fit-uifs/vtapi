FROM ubuntu:16.04
MAINTAINER Tomáš Volf <ivolf@fit.vutbr.cz>

ENV DEBIAN_FRONTEND noninteractive

# Force upgrade dependencies
RUN apt-get -yq update \
    && apt-get install -y --no-install-recommends apt-utils \
    && apt-get dist-upgrade -y \
    && apt-get -yq update

# Install build and other useful tools
RUN apt-get -yqq install software-properties-common build-essential wget git cmake pkg-config locate \
    && apt-get -yq update

####################      INSTALL   DEPENDENCIES     ####################
# OpenCV3 PPA
RUN add-apt-repository -y ppa:lkoppel/opencv \
    && apt-get -yqq install libopencv-dev

# PostgreSQL & libpq
RUN apt-get -yqq install postgresql postgresql-server-dev-all postgresql-client libpq-dev libpq5

# libpqtypes
RUN apt-get -yqq install libpqtypes0 libpqtypes-dev

# SQLite
RUN apt-get -yqq install libsqlite3-dev

# Boost
RUN apt-get -yqq install libboost-thread-dev libboost-program-options-dev

# Protocol Buffers
RUN apt-get -yqq install libprotobuf-dev protobuf-compiler libprotoc-dev

# ZeroMQ
RUN apt-get -yqq install libzmq-dev 

# Python
RUN apt-get -yqq install python python-pip python-dev \
    && pip install --upgrade pip

####################      CONFIG   DEPENDENCIES     ####################
# PostgreSQL config_file
ENV POSTGRES_VERSION=9.5
ARG POSTGRES_USER=vtapi
ARG POSTGRES_PASSWORD=Docker
ARG POSTGRES_DB=vtapi
RUN echo "host    all    all    0.0.0.0/0   md5" >> /etc/postgresql/${POSTGRES_VERSION}/main/pg_hba.conf
RUN echo "local   all    postgres           md5" >> /etc/postgresql/${POSTGRES_VERSION}/main/pg_hba.conf
RUN echo "listen_addresses='*'" >> /etc/postgresql/${POSTGRES_VERSION}/main/postgresql.conf \
    && echo "timezone = 'Europe/Prague'" >> /etc/postgresql/${POSTGRES_VERSION}/main/postgresql.conf \
    && echo "hot_standby = on" >> /etc/postgresql/${POSTGRES_VERSION}/main/postgresql.conf
RUN echo "localhost:5432:${POSTGRES_DB}:${POSTGRES_USER}:${POSTGRES_PASSWORD}" >> ~/.pgpass \
    && chmod 0600 ~/.pgpass
USER postgres
RUN /etc/init.d/postgresql start \
    && psql --command "CREATE USER ${POSTGRES_USER} WITH SUPERUSER PASSWORD '${POSTGRES_PASSWORD}';" \
    && createdb -O ${POSTGRES_USER} ${POSTGRES_DB} \
    && /etc/init.d/postgresql stop
USER root
EXPOSE 5432

# Poco library
RUN cd \
    && wget http://pocoproject.org/releases/poco-1.6.1/poco-1.6.1.tar.gz \
    && tar -xzf poco-1.6.1.tar.gz \
    && cd poco-1.6.1 \
    && ./configure --no-tests --no-samples \
    && make \
    && make install

RUN apt-get -yq update

RUN echo "/usr/local/lib/" >> /etc/ld.so.conf.d/local.conf \
    && ldconfig
     
ENV PATH /usr/local/bin:$PATH

# VTApi, VTServer, RPCZ - clone repo & move to location
RUN cd  \
    && git clone https://github.com/fit-uifs/vtapi.git
# RPCZ
RUN cd ~/vtapi/3rdparty/rpcz \
    && git submodule init \
    && git submodule update \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make \
    && make install \
    && cd ../python \
    && python setup.py build \
    && pip install -e . \
    && ldconfig
# VTApi, VTServer
RUN cd ~/vtapi \
    && ./compile_interfaces.sh \
    && sed -i -- 's/sudo make install/make install/g' build_release.sh \
    && ./build_release.sh \
    && cd pyclient \
    && python setup.py install
# VTApi database infrastructure
RUN /etc/init.d/postgresql start \
    && psql -h localhost -f ~/vtapi/sql/postgresql/pg_createdb.sql vtapi vtapi \
    && psql -h localhost -f ~/vtapi/sql/postgresql/demo-schema.sql vtapi vtapi \
    && /etc/init.d/postgresql stop
# Config VTApi & VTServer
RUN mkdir -p /vtapi/data \
    && mkdir -p /vtapi/datasets \
    && cp ~/vtapi/vtapi_example.conf /vtapi/vtapi.conf \
    && sed -i -- 's/datasets_dir=\/mnt\/vtapi_data/datasets_dir=\/vtapimnt\/data/g' /vtapi/vtapi.conf \
    && sed -i -- 's/modules_dir=\/mnt\/vtapi_modules/modules_dir=\/vtapimnt\/modules/g' /vtapi/vtapi.conf \
    && sed -i -- "s/connection=postgresql:\/\/user@localhost:4321\/vidte/connection=postgresql:\/\/${POSTGRES_USER}@localhost:5432\/${POSTGRES_DB}/g" /vtapi/vtapi.conf

ENV DEBIAN_FRONTEND teletype

#############################      RUN   PHASE     #############################
ENTRYPOINT /etc/init.d/postgresql start && updatedb && (vtserver --config=/vtapi/vtapi.conf &) && /bin/bash
